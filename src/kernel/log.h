#pragma once

#include "vga.h"
#include "../common/color.h"

namespace log
{
    enum Level
    {
        ERROR = -1,
        WARN = 0,
        INFO = 1
    };

    inline void log(const char* str, int level)
    {
        if (!str) return;

        switch (level) {
        case INFO:
            vga::write_str("[INFO]  ", color::LOG_INFO);
            break;
        case WARN:
            vga::write_str("[WARN]  ", color::LOG_WARN);
            break;
        case ERROR:
            vga::write_str("[ERROR] ", color::LOG_ERROR);
            break;
        default:
            vga::write_str("[UNKNOWN] ", color::DARK_GRAY);
            break;
        }

        vga::write_str(str);
        vga::put_char('\n');
    }
}