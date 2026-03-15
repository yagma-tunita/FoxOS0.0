#pragma once

#include "../common/types.h"
#include "../common/utils.h"
#include "../common/color.h"

namespace vga
{
	constexpr u32 VGA_WIDTH = 80;
	constexpr u32 VGA_HEIGHT = 25;

	static u8 _current_color = color::DEFAULT;
	static u16* _buffer = reinterpret_cast<u16*>(0xB8000);
	static u32 _cursor_x = 0;
	static u32 _cursor_y = 0;


	inline void clear()
	{
		for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		{
			_buffer[i] = (static_cast<u16>(color::DEFAULT) << 8) | ' ';
		}
		_cursor_x = 0;
		_cursor_y = 0;
	}

	inline void init()
	{
		_cursor_x = 0;
		_cursor_y = 0;
		clear(); 
	}

	inline void set_color(u8 color)
	{
		_current_color = color;
	}

	inline void put_char(char c, u8 color = color::DEFAULT)
	{
		if (c == '\n')
		{
			_cursor_x = 0;
			_cursor_y++;
			if (_cursor_y >= VGA_HEIGHT) _cursor_y = 0;
			return;
		}
		size_t index = VGA_WIDTH * _cursor_y + _cursor_x;

		_buffer[index] = (static_cast<u16>(color) << 8) | static_cast<u16>(c);
		_cursor_x++;

		if (_cursor_x >= VGA_WIDTH)
		{
			_cursor_x = 0;
			_cursor_y++;
			if (_cursor_y >= VGA_HEIGHT) _cursor_y = 0;
		}
	}

	inline void write_str(const char* str, u8 color = color::DEFAULT)
	{
		while (*str)
		{
			put_char(*str, color);
			str++;
		}
	}
}