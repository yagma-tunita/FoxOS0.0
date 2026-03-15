#pragma once

#include "../common/types.h"
#include "../common/string_util.h"
#include "../common/utils.h"
#include "../common/list.h"
#include "fs.h"
#include "vga.h"
#include "log.h"

namespace editor
{
    struct Editor
    {
        fs::File* file;
        bool     active;
        bool     modified;

        char     buffer[512];
        size_t   len;

        size_t   cursor;
        size_t   screen_x;
        size_t   screen_y;
    };

    // Ç°ÖÃÉùÃ÷
    inline void update_cursor_screen(Editor* e);

    inline void editor_reset(Editor* e)
    {
        if (!e) return;
        e->file = nullptr;
        e->active = false;
        e->modified = false;
        e->len = 0;
        e->cursor = 0;
        e->screen_x = 0;
        e->screen_y = 0;
        memset(e->buffer, 0, 512);
    }

    inline void editor_init(Editor* e)
    {
        if (!e) return;
        editor_reset(e);
        log::log("FoxText Editor (Stable) loaded", log::INFO);
    }

    inline bool editor_open(Editor* e, fs::FS* fs, const char* name)
    {
        if (!e || !fs || !name) return false;

        list::Node* n = fs->current->children.head;
        while (n)
        {
            fs::File* f = (fs::File*)n->data;
            if (!f->is_dir && string_util::strcmp(f->name, name) == 0)
            {
                editor_reset(e);
                e->file = f;
                e->active = true;
                string_util::strncpy(e->buffer, f->content, 511);
                e->len = string_util::strlen(e->buffer);
                e->cursor = 0;
                update_cursor_screen(e);
                return true;
            }
            n = n->next;
        }
        log::log("File not found for editing", log::ERROR);
        return false;
    }

    // ¼æÈÝº¯Êý
    inline bool open_file(Editor* e, fs::FS* fs, const char* name)
    {
        return editor_open(e, fs, name);
    }

    inline void update_cursor_screen(Editor* e)
    {
        if (!e) return;
        e->screen_x = 0;
        e->screen_y = 0;
        for (size_t i = 0; i < e->cursor && i < e->len; ++i)
        {
            if (e->buffer[i] == '\n')
            {
                e->screen_y++;
                e->screen_x = 0;
            }
            else
            {
                e->screen_x++;
            }
        }
    }

    inline void move_left(Editor* e)
    {
        if (!e || !e->active || e->cursor == 0) return;
        e->cursor--;
        update_cursor_screen(e);
    }

    inline void move_right(Editor* e)
    {
        if (!e || !e->active || e->cursor >= e->len) return;
        e->cursor++;
        update_cursor_screen(e);
    }

    inline void move_up(Editor* e)
    {
        if (!e || !e->active || e->cursor == 0) return;

        size_t pos = e->cursor;
        while (pos > 0 && e->buffer[pos - 1] != '\n') pos--;
        if (pos == 0) return;

        size_t prev_line_end = pos - 1;
        while (prev_line_end > 0 && e->buffer[prev_line_end - 1] != '\n') prev_line_end--;

        size_t current_x = e->screen_x;
        pos = prev_line_end;
        for (size_t i = 0; i < current_x && pos < e->len && e->buffer[pos] != '\n'; i++)
            pos++;

        e->cursor = pos;
        update_cursor_screen(e);
    }

    inline void move_down(Editor* e)
    {
        if (!e || !e->active) return;

        size_t pos = e->cursor;
        while (pos < e->len && e->buffer[pos] != '\n') pos++;
        if (pos >= e->len) return;

        pos++;
        size_t current_x = e->screen_x;
        for (size_t i = 0; i < current_x && pos < e->len && e->buffer[pos] != '\n'; i++)
            pos++;

        e->cursor = pos;
        update_cursor_screen(e);
    }

    inline void backspace(Editor* e)
    {
        if (!e || !e->active || e->cursor == 0 || e->len == 0) return;

        size_t pos = e->cursor - 1;
        for (size_t i = pos; i < e->len - 1; i++)
            e->buffer[i] = e->buffer[i + 1];

        e->len--;
        e->cursor--;
        e->buffer[e->len] = 0;
        e->modified = true;
        update_cursor_screen(e);
    }

    inline void insert_char(Editor* e, char c)
    {
        if (!e || !e->active || e->len >= 510) return;

        size_t pos = e->cursor;
        for (size_t i = e->len; i > pos; i--)
            e->buffer[i] = e->buffer[i - 1];

        e->buffer[pos] = c;
        e->len++;
        e->cursor++;
        e->modified = true;
        update_cursor_screen(e);
    }

    inline void save(Editor* e)
    {
        if (!e || !e->active || !e->file) return;
        string_util::strncpy(e->file->content, e->buffer, 511);
        e->file->content[511] = 0;
        e->file->size = e->len;
        e->modified = false;
        log::log("File saved", log::INFO);
    }

    inline void editor_render(Editor* e)
    {
        if (!e || !e->active)
        {
            vga::set_color(color::EDITOR_TEXT);
            vga::write_str("Editor: No open file\n");
            return;
        }

        vga::clear();

        // Editor header
        vga::set_color(color::EDITOR_BAR);
        vga::write_str("====== FoxText Editor (Stable) ======\n");

        // File info
        vga::set_color(color::EDITOR_TEXT);
        vga::write_str("File: ");
        vga::set_color(color::FILE);
        vga::write_str(e->file->name);
        vga::set_color(color::EDITOR_TEXT);
        vga::write_str(" | Modified: ");
        vga::put_char(e->modified ? 'Y' : 'N');
        vga::write_str("\n=====================================\n");

        // Render content
        vga::set_color(color::EDITOR_TEXT);
        vga::write_str(e->buffer);

        // Footer
        vga::set_color(color::EDITOR_BAR);
        vga::write_str("\n=====================================\n");
        vga::set_color(color::EDITOR_TEXT);

        // Cursor info
        vga::write_str("Cursor: (");
        vga::write_str(string_util::itoa(e->screen_x));
        vga::write_str(",");
        vga::write_str(string_util::itoa(e->screen_y));
        vga::write_str(") | Press Ctrl+S to save\n");
    }

    // ¼æÈÝº¯Êý
    inline void render(Editor* e)
    {
        editor_render(e);
    }

    inline void handle_input(Editor* e, char c)
    {
        if (!e || !e->active) return;

        switch (c)
        {
        case '\b': // Backspace
            backspace(e);
            break;
        case '\x13': // Ctrl+S (Save)
            save(e);
            break;
        case '\x10': // Ctrl+P (Up)
            move_up(e);
            break;
        case '\x0e': // Ctrl+N (Down)
            move_down(e);
            break;
        case '\x02': // Ctrl+B (Left)
            move_left(e);
            break;
        case '\x06': // Ctrl+F (Right)
            move_right(e);
            break;
        case '\n': // Enter
            insert_char(e, '\n');
            break;
        default: // Printable chars
            if (c >= 32 && c <= 126)
                insert_char(e, c);
            break;
        }
    }
}