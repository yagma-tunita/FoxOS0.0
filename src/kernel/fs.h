#pragma once

#include "../common/types.h"
#include "../common/list.h"
#include "../common/string_util.h"
#include "../common/utils.h"
#include "vga.h"
#include "log.h"

namespace fs
{
    struct File;
    inline void free_file(File* file);

    enum ConflictPolicy
    {
        ABORT,
        OVERWRITE,
        RENAME_AUTO
    };

    struct File
    {
        char name[32];
        bool is_dir;
        char content[512];
        size_t size;
        list::List children;
        File* parent;
    };

    struct FS
    {
        File* root;
        File* current;
    };

    inline void fs_rm(FS* fs, const char* name)
    {
        if (!fs || !name)
        {
            log::log("Invalid argument", log::ERROR);
            return;
        }
        list::Node* n = fs->current->children.head;
        list::Node* p = nullptr;
        while (n)
        {
            File* f = (File*)n->data;
            if (string_util::strcmp(f->name, name) == 0)
            {
                p ? p->next = n->next : fs->current->children.head = n->next;
                free_file(f);
                delete n;
                fs->current->children.length--;
                log::log("Removed", log::INFO);
                return;
            }
            p = n;
            n = n->next;
        }
        log::log("Not found", log::ERROR);
    }

    inline void generate_auto_name(char* out, const char* base, int num)
    {
        if (!out || !base) return;

        char name_no_ext[32];
        const char* ext = string_util::strchr(base, '.');
        size_t name_len;

        if (ext)
        {
            name_len = ext - base;
            string_util::strncpy(name_no_ext, base, name_len);
            name_no_ext[name_len] = 0;
        }
        else
        {
            string_util::strcpy(name_no_ext, base);
            name_len = string_util::strlen(base);
        }

        string_util::strcpy(out, name_no_ext);
        char num_buf[16];
        num_buf[0] = '_';
        int n = num;
        int i = 0;
        do
        {
            num_buf[++i] = '0' + (n % 10);
            n /= 10;
        } while (n > 0);
        num_buf[i + 1] = 0;

        string_util::strcat(out, num_buf);
        if (ext) string_util::strcat(out, ext);
    }

    inline bool exists(list::List* children, const char* name)
    {
        list::Node* n = children->head;
        while (n)
        {
            File* f = (File*)n->data;
            if (string_util::strcmp(f->name, name) == 0) return true;
            n = n->next;
        }
        return false;
    }

    inline void fs_init(FS* fs)
    {
        if (!fs) return;
        File* root = new File;
        string_util::strcpy(root->name, "/");
        root->is_dir = true;
        root->size = 0;
        root->parent = nullptr;
        list::init(&root->children);
        fs->root = root;
        fs->current = root;
        log::log("Filesystem initialized", log::INFO);
    }

    inline void free_file(File* file)
    {
        if (!file) return;
        if (file->is_dir)
        {
            list::Node* n = file->children.head;
            while (n)
            {
                free_file((File*)n->data);
                n = n->next;
            }
            list::clear(&file->children);
        }
        delete file;
    }

    inline bool fs_mkdir(FS* fs, const char* name, ConflictPolicy policy = ABORT)
    {
        if (!fs || !name || !*name)
        {
            log::log("Invalid argument", log::ERROR);
            return false;
        }

        if (!exists(&fs->current->children, name))
        {
            File* d = new File;
            string_util::strcpy(d->name, name);
            d->is_dir = true;
            d->size = 0;
            d->parent = fs->current;
            list::init(&d->children);
            list::push_back(&fs->current->children, d);
            log::log("Directory created", log::INFO);
            return true;
        }

        if (policy == ABORT)
        {
            log::log("Directory exists - aborted", log::ERROR);
            return false;
        }
        if (policy == OVERWRITE)
        {
            fs_rm(fs, name); // 现在 fs_rm 已提前声明
            File* d = new File;
            string_util::strcpy(d->name, name);
            d->is_dir = true;
            d->parent = fs->current;
            list::init(&d->children);
            list::push_back(&fs->current->children, d);
            log::log("Directory overwritten", log::INFO);
            return true;
        }
        if (policy == RENAME_AUTO)
        {
            char new_name[32];
            int idx = 1;
            do
            {
                generate_auto_name(new_name, name, idx++);
            } while (exists(&fs->current->children, new_name));

            File* d = new File;
            string_util::strcpy(d->name, new_name);
            d->is_dir = true;
            d->parent = fs->current;
            list::init(&d->children);
            list::push_back(&fs->current->children, d);
            log::log("Created with auto name", log::INFO);
            return true;
        }
        return false;
    }

    inline bool fs_create(FS* fs, const char* name, ConflictPolicy policy = ABORT)
    {
        if (!fs || !name || !*name)
        {
            log::log("Invalid argument", log::ERROR);
            return false;
        }

        if (!exists(&fs->current->children, name))
        {
            File* f = new File;
            string_util::strcpy(f->name, name);
            f->is_dir = false;
            f->size = 0;
            f->parent = fs->current;
            string_util::strcpy(f->content, "");
            list::push_back(&fs->current->children, f);
            log::log("File created", log::INFO);
            return true;
        }

        if (policy == ABORT)
        {
            log::log("File exists - aborted", log::ERROR);
            return false;
        }
        if (policy == OVERWRITE)
        {
            fs_rm(fs, name); // 现在 fs_rm 已提前声明
            File* f = new File;
            string_util::strcpy(f->name, name);
            f->is_dir = false;
            f->size = 0;
            f->parent = fs->current;
            string_util::strcpy(f->content, "");
            list::push_back(&fs->current->children, f);
            log::log("File overwritten", log::INFO);
            return true;
        }
        if (policy == RENAME_AUTO)
        {
            char new_name[32];
            int idx = 1;
            do
            {
                generate_auto_name(new_name, name, idx++);
            } while (exists(&fs->current->children, new_name));

            File* f = new File;
            string_util::strcpy(f->name, new_name);
            f->is_dir = false;
            f->parent = fs->current;
            string_util::strcpy(f->content, "");
            list::push_back(&fs->current->children, f);
            log::log("Created with auto name", log::INFO);
            return true;
        }
        return false;
    }

    inline void fs_ls(FS* fs)
    {
        if (!fs)
        {
            log::log("FS is null", log::ERROR);
            return;
        }
        if (list::empty(&fs->current->children))
        {
            vga::write_str("> Empty directory\n");
            return;
        }
        vga::write_str("> ");
        vga::write_str(fs->current->name);
        vga::write_str(":\n");
        auto cb = [](void* data)
            {
                File* f = (File*)data;
                f->is_dir ? vga::write_str("[DIR]  ") : vga::write_str("[FILE] ");
                vga::write_str(f->name);
                vga::put_char('\n');
            };
        list::foreach(&fs->current->children, cb);
    }

    inline void fs_cd(FS* fs, const char* target)
    {
        if (!fs || !target)
        {
            log::log("Invalid argument", log::ERROR);
            return;
        }
        if (string_util::strcmp(target, "..") == 0)
        {
            if (fs->current->parent)
            {
                fs->current = fs->current->parent;
                log::log("Changed to parent", log::INFO);
            }
            else
            {
                log::log("Already at root", log::WARN);
            }
            return;
        }
        list::Node* n = fs->current->children.head;
        while (n)
        {
            File* f = (File*)n->data;
            if (f->is_dir && string_util::strcmp(f->name, target) == 0)
            {
                fs->current = f;
                log::log("Directory changed", log::INFO);
                return;
            }
            n = n->next;
        }
        log::log("Not found", log::ERROR);
    }

    inline void fs_write(FS* fs, const char* name, const char* content)
    {
        if (!fs || !name || !content)
        {
            log::log("Invalid argument", log::ERROR);
            return;
        }
        list::Node* n = fs->current->children.head;
        while (n)
        {
            File* f = (File*)n->data;
            if (!f->is_dir && string_util::strcmp(f->name, name) == 0)
            {
                string_util::strncpy(f->content, content, 511);
                f->content[511] = 0;
                f->size = string_util::strlen(f->content);
                log::log("Write success", log::INFO);
                return;
            }
            n = n->next;
        }
        log::log("Not found", log::ERROR);
    }

    inline void fs_read(FS* fs, const char* name)
    {
        if (!fs || !name)
        {
            log::log("Invalid argument", log::ERROR);
            return;
        }
        list::Node* n = fs->current->children.head;
        while (n)
        {
            File* f = (File*)n->data;
            if (!f->is_dir && string_util::strcmp(f->name, name) == 0)
            {
                vga::write_str("> ");
                vga::write_str(f->content);
                vga::put_char('\n');
                return;
            }
            n = n->next;
        }
        log::log("Not found", log::ERROR);
    }
}