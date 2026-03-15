#pragma once

#include "types.h"
#include "utils.h"

namespace string_util
{
    // ×Ö·û´®³¤¶È
    inline size_t strlen(const char* str)
    {
        if (!str) return 0;
        size_t len = 0;
        while (str[len] != '\0') len++;
        return len;
    }

    // ×Ö·û´®¿½±´
    inline char* strcpy(char* dest, const char* src)
    {
        if (!dest || !src) return dest;
        char* d = dest;
        while (*src != '\0')
        {
            *d = *src;
            d++;
            src++;
        }
        *d = '\0';
        return dest;
    }

    // ×Ö·û´®ÓÐÏÞ¿½±´
    inline char* strncpy(char* dest, const char* src, size_t n)
    {
        if (!dest || !src) return dest;
        size_t i;
        for (i = 0; i < n && src[i] != '\0'; i++)
        {
            dest[i] = src[i];
        }
        for (; i < n; i++)
        {
            dest[i] = '\0';
        }
        return dest;
    }

    // ×Ö·û´®Æ´½Ó
    inline char* strcat(char* dest, const char* src)
    {
        if (!dest || !src) return dest;
        size_t dest_len = strlen(dest);
        size_t i = 0;
        while (src[i] != '\0')
        {
            dest[dest_len + i] = src[i];
            i++;
        }
        dest[dest_len + i] = '\0';
        return dest;
    }

    // ×Ö·û´®±È½Ï
    inline int strcmp(const char* str1, const char* str2)
    {
        if (!str1 || !str2) return str1 ? 1 : (str2 ? -1 : 0);
        while (*str1 && *str2 && *str1 == *str2)
        {
            str1++;
            str2++;
        }
        return (unsigned char)*str1 - (unsigned char)*str2;
    }

    // ²éÕÒ×Ö·û
    inline const char* strchr(const char* str, int c)
    {
        if (!str) return nullptr;
        while (*str != '\0')
        {
            if (*str == (char)c) return str;
            str++;
        }
        return (c == '\0') ? str : nullptr;
    }

    // ÕûÊý×ª×Ö·û´®
    inline char* itoa(int num)
    {
        static char buf[16];
        memset(buf, 0, 16);
        if (num == 0)
        {
            buf[0] = '0';
            return buf;
        }

        int i = 0;
        bool negative = false;
        if (num < 0)
        {
            negative = true;
            num = -num;
        }

        while (num > 0)
        {
            buf[i++] = '0' + (num % 10);
            num /= 10;
        }

        if (negative)
            buf[i++] = '-';

        size_t len = i;
        for (size_t j = 0; j < len / 2; j++)
        {
            char temp = buf[j];
            buf[j] = buf[len - j - 1];
            buf[len - j - 1] = temp;
        }

        return buf;
    }
}