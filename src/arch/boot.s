[BITS 32]
[GLOBAL start]
[EXTERN kernel_main]

MBOOT_MAGIC  equ 0x1BADB002
MBOOT_FLAGS  equ 0x0
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384
stack_top:

section .text
start:
    mov esp, stack_top
    call kernel_main
    cli
hang:
    hlt
    jmp hang