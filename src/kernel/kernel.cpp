#include "kernel.h"

// ==========================
// 内核堆与new/delete实现（核心修复）
// ==========================
#define KERNEL_HEAP_START 0x100000  // 1MB起始地址
#define KERNEL_HEAP_SIZE  0x100000   // 1MB堆大小

static u8* heap_ptr = (u8*)KERNEL_HEAP_START;
static u8* heap_end = (u8*)(KERNEL_HEAP_START + KERNEL_HEAP_SIZE);

// 修复1：显式转换NULL为void*，添加throw()声明
void* operator new(unsigned int size) throw() {
    // 检查堆内存是否充足
    if (heap_ptr + size > heap_end) {
        return (void*)0;  // 显式转换为void*，避免类型错误
    }
    void* ptr = heap_ptr;

    // 修复2：内存对齐（先转u32计算，再转回u8*，避免类型不匹配）
    u32 aligned_addr = ((u32)heap_ptr + size + 3) & ~3;
    heap_ptr = (u8*)aligned_addr;

    return ptr;
}

// 匹配的operator delete实现
void operator delete(void* ptr, unsigned int size) throw() {
    (void)ptr;
    (void)size;
}

// 无参数版delete（兼容编译器）
void operator delete(void* ptr) throw() {
    (void)ptr;
}

// ==========================
// 原有全局变量与键盘映射
// ==========================
fs::FS        g_fs;
editor::Editor g_editor;
shell::Shell   g_shell;

const char scan_code_table[] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,  'a','s','d','f','g','h','j','k','l',';','\'','`',0,
    '\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    '-',0,0,0,'+',0,0,0,0,0,0,0,0,0,0,0
};

// ==========================
// 键盘读取函数
// ==========================
inline unsigned char read_keyboard() {
    unsigned char sc;
    asm volatile ("inb $0x60, %0" : "=a"(sc));
    return sc;
}

char get_char() {
    static bool released = false;
    while (true) {
        unsigned char sc = read_keyboard();
        if (sc >= 0x80) {
            released = true;
            continue;
        }
        if (released) {
            released = false;
            if (sc < sizeof(scan_code_table)) {
                char c = scan_code_table[sc];
                if (c != 0) return c;
            }
        }
    }
}

// ==========================
// 输入缓冲区与字符处理
// ==========================
constexpr int INPUT_BUFFER_SIZE = 256;
char input_buffer[INPUT_BUFFER_SIZE] = { 0 };
int  buffer_len = 0;

void kernel_process_char(char c) {
    if (c == '\b') {
        if (buffer_len > 0) {
            buffer_len--;
            input_buffer[buffer_len] = 0;
            vga::put_char('\b');
            vga::put_char(' ');
            vga::put_char('\b');
        }
        return;
    }
    if (c == '\n') {
        vga::put_char('\n');
        shell::shell_run(&g_shell, input_buffer);
        buffer_len = 0;
        memset(input_buffer, 0, INPUT_BUFFER_SIZE);
        return;
    }
    if (c >= 32 && c <= 126 && buffer_len < INPUT_BUFFER_SIZE - 1) {
        input_buffer[buffer_len++] = c;
        vga::put_char(c);
    }
}

// ==========================
// 内核入口函数（修复write_string拼写错误）
// ==========================
extern "C" void kernel_main() {
    // 初始化VGA
    vga::init();
    vga::set_color(color::WHITE_ON_BLACK);

    // 初始化文件系统、编辑器、Shell
    fs::fs_init(&g_fs);
    editor::editor_init(&g_editor);
    shell::shell_init(&g_shell, &g_fs, &g_editor);

    vga::set_color(color::LIGHT_MAGENTA_ON_BLACK);
    vga::write_str("\n=====================================\n");
    vga::write_str("           FoxOS 64-bit              \n");
    vga::write_str("=====================================\n");
    vga::set_color(color::WHITE_ON_BLACK);

    // 显示Shell提示符
    shell::shell_prompt(&g_shell);

    // 主循环处理键盘输入
    while (true) {
        char c = get_char();
        kernel_process_char(c);
    }
}