#pragma once

#include "types.h"

namespace color
{
    constexpr u8 BLACK = 0x0;
    constexpr u8 BLUE = 0x1;
    constexpr u8 GREEN = 0x2;
    constexpr u8 CYAN = 0x3;
    constexpr u8 RED = 0x4;
    constexpr u8 MAGENTA = 0x5;
    constexpr u8 BROWN = 0x6;
    constexpr u8 LIGHT_GRAY = 0x7;
    constexpr u8 DARK_GRAY = 0x8;
    constexpr u8 LIGHT_BLUE = 0x9;
    constexpr u8 LIGHT_GREEN = 0xA;
    constexpr u8 LIGHT_CYAN = 0xB;
    constexpr u8 LIGHT_RED = 0xC;
    constexpr u8 LIGHT_MAGENTA = 0xD;
    constexpr u8 YELLOW = 0xE;
    constexpr u8 WHITE = 0xF;

    constexpr u8 WHITE_ON_BLACK = 0x0F;
    constexpr u8 LIGHT_MAGENTA_ON_BLACK = 0x0D;
    constexpr u8 DEFAULT = WHITE_ON_BLACK;

    constexpr u8 LOG_INFO = 0x0A;    // 绿色
    constexpr u8 LOG_WARN = 0x0E;    // 黄色
    constexpr u8 LOG_ERROR = 0x0C;   // 红色
    constexpr u8 PROMPT = 0x0B;      // 浅青色
    constexpr u8 DIR = 0x09;         // 浅蓝色
    constexpr u8 FILE = 0x07;        // 浅灰色
    constexpr u8 ERROR_MSG = 0x0C;   // 红色
    constexpr u8 HELP_HEADER = 0x0D; // 浅洋红
    constexpr u8 HELP_TEXT = 0x0F;   // 白色

    constexpr u8 EDITOR_BAR = 0x05;  // 洋红
    constexpr u8 EDITOR_TEXT = 0x0F; // 白色
}