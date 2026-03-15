#pragma once

#include "../common/types.h"
#include "../common/string_util.h"
#include "../common/utils.h"
#include "../common/color.h"
#include "../common/list.h"
#include "vga.h"
#include "log.h"
#include "fs.h"
#include "editor.h"

namespace shell
{
    const size_t MAX_INPUT = 256;
    const size_t MAX_ARGS = 8;

    struct Shell
    {
        fs::FS* fs;
        editor::Editor* editor;

        char           input[MAX_INPUT];
        size_t         input_len;

        const char* argv[MAX_ARGS];
        size_t         argc;
    };

    // Initialize Shell
    inline void shell_init(Shell* sh, fs::FS* fs, editor::Editor* editor)
    {
        if (!sh) return;
        sh->fs = fs;
        sh->editor = editor;
        sh->input_len = 0;
        memset(sh->input, 0, MAX_INPUT);

        vga::set_color(color::DEFAULT);
        log::log("FoxOS Shell [COLOR MODE] loaded", log::INFO);
    }

    // Split input into arguments
    inline void shell_split_args(Shell* sh)
    {
        if (!sh) return;
        sh->argc = 0;
        memset(sh->argv, 0, sizeof(sh->argv));

        size_t i = 0;
        while (i < sh->input_len && sh->argc < MAX_ARGS - 1)
        {
            while (i < sh->input_len && sh->input[i] == ' ') i++;
            if (i >= sh->input_len) break;

            sh->argv[sh->argc++] = &sh->input[i];

            while (i < sh->input_len && sh->input[i] != ' ') i++;
            if (i < sh->input_len) sh->input[i++] = 0;
        }
        sh->argv[sh->argc] = nullptr;
    }

    // Print shell prompt
    inline void shell_prompt(Shell* sh)
    {
        if (!sh || !sh->fs) return;

        vga::set_color(color::PROMPT);
        vga::write_str("\n[");
        vga::set_color(color::DIR);
        vga::write_str(sh->fs->current->name);
        vga::set_color(color::PROMPT);
        vga::write_str("] $ ");

        vga::set_color(color::DEFAULT);
    }

    // Show help info
    inline void shell_help()
    {
        vga::set_color(color::HELP_HEADER);
        vga::write_str("\n========== FoxOS Shell ==========\n");

        vga::set_color(color::HELP_TEXT);
        vga::write_str(" ls                  List entries\n");
        vga::write_str(" mkdir [name]        Create directory\n");
        vga::write_str(" cd [name|..]        Change directory\n");
        vga::write_str(" create [name]       Create .ft file\n");
        vga::write_str(" rm [name]           Delete entry\n");
        vga::write_str(" write [file] [txt]  Write content\n");
        vga::write_str(" read [file]         Read content\n");
        vga::write_str(" edit [file]         Edit file\n");
        vga::write_str(" clear               Clear screen\n");
        vga::write_str(" help                Show help\n");

        vga::set_color(color::HELP_HEADER);
        vga::write_str("==================================\n");
        vga::set_color(color::DEFAULT);
    }

    // List directory contents
    inline void shell_ls(Shell* sh)
    {
        if (!sh) return;
        auto& list = sh->fs->current->children;

        if (list::empty(&list))
        {
            vga::set_color(color::LOG_WARN);
            vga::write_str("> Empty directory\n");
            vga::set_color(color::DEFAULT);
            return;
        }

        vga::set_color(color::WHITE);
        vga::write_str("> Contents of ");
        vga::set_color(color::DIR);
        vga::write_str(sh->fs->current->name);
        vga::set_color(color::WHITE);
        vga::write_str(":\n");

        auto cb = [](void* data) {
            fs::File* f = (fs::File*)data;
            if (f->is_dir)
            {
                vga::set_color(color::DIR);
                vga::write_str("[DIR]  ");
            }
            else
            {
                vga::set_color(color::FILE);
                vga::write_str("[FILE] ");
            }
            vga::write_str(f->name);
            vga::put_char('\n');
            };

        list::foreach(&list, cb);
        vga::set_color(color::DEFAULT);
    }

    // Print usage error
    inline void shell_usage_error(const char* msg)
    {
        vga::set_color(color::ERROR_MSG);
        vga::write_str("Usage: ");
        vga::write_str(msg);
        vga::put_char('\n');
        vga::set_color(color::DEFAULT);
    }

    inline void shell_exec(Shell* sh)
    {
        if (!sh || sh->argc == 0) return;
        const char* cmd = sh->argv[0];

        if (string_util::strcmp(cmd, "help") == 0)
        {
            shell_help();
        }
        else if (string_util::strcmp(cmd, "ls") == 0)
        {
            shell_ls(sh);
        }
        else if (string_util::strcmp(cmd, "clear") == 0)
        {
            vga::clear();
        }
        else if (string_util::strcmp(cmd, "mkdir") == 0)
        {
            if (sh->argc < 2) shell_usage_error("mkdir [name]");
            else fs::fs_mkdir(sh->fs, sh->argv[1], fs::ABORT);
        }
        else if (string_util::strcmp(cmd, "cd") == 0)
        {
            if (sh->argc < 2) shell_usage_error("cd [name|..]");
            else fs::fs_cd(sh->fs, sh->argv[1]);
        }
        else if (string_util::strcmp(cmd, "create") == 0)
        {
            if (sh->argc < 2) shell_usage_error("create [name]");
            else fs::fs_create(sh->fs, sh->argv[1], fs::ABORT);
        }
        else if (string_util::strcmp(cmd, "rm") == 0)
        {
            if (sh->argc < 2) shell_usage_error("rm [name]");
            else fs::fs_rm(sh->fs, sh->argv[1]);
        }
        else if (string_util::strcmp(cmd, "read") == 0)
        {
            if (sh->argc < 2) shell_usage_error("read [file]");
            else fs::fs_read(sh->fs, sh->argv[1]);
        }
        else if (string_util::strcmp(cmd, "write") == 0)
        {
            if (sh->argc < 3) shell_usage_error("write [file] [content]");
            else fs::fs_write(sh->fs, sh->argv[1], sh->argv[2]);
        }
        else if (string_util::strcmp(cmd, "edit") == 0)
        {
            if (sh->argc < 2) shell_usage_error("edit [file]");
            else
            {
                if (editor::editor_open(sh->editor, sh->fs, sh->argv[1]))
                {
                    editor::editor_render(sh->editor);
                    // 编辑器交互逻辑可在此扩展
                }
            }
        }
        else
        {
            vga::set_color(color::ERROR_MSG);
            vga::write_str("Unknown command: ");
            vga::write_str(cmd);
            vga::put_char('\n');
            vga::set_color(color::DEFAULT);
        }
    }

    // Process input and execute
    inline void shell_input(Shell* sh, const char* str)
    {
        if (!sh || !str) return;
        string_util::strncpy(sh->input, str, MAX_INPUT - 1);
        sh->input_len = string_util::strlen(sh->input);
        shell_split_args(sh);
        shell_exec(sh);
    }

    inline void shell_run(Shell* sh, const char* user_input)
    {
        if (!sh) return;
        shell_input(sh, user_input);
        shell_prompt(sh);
    }
}