#include "NsString.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if NV_WINDOWS_FAMILY
#pragma warning(push)
#pragma warning(disable : 4996) // unsafe string functions
#endif

#if NV_PS4 || NV_APPLE_FAMILY
#pragma clang diagnostic push
// error : format string is not a string literal
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

namespace blockchainsim
{
// cross-platform implementations

int32_t stringCompare(const char* str1, const char* str2)
{
    return ::strcmp(str1, str2);
}

int32_t stringCompareN(const char* str1, const char* str2, size_t count)
{
    return ::strncmp(str1, str2, count);
}

int32_t stringFormat(char* dst, size_t dstSize, const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int32_t r = blockchainsim::stringFormatV(dst, dstSize, format, arg);
    va_end(arg);
    return r;
}

int32_t sscanf(const char* buffer, const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
#if NV_VC
    int32_t r = ::sscanf(buffer, format, arg);
#else
    int32_t r = ::vsscanf(buffer, format, arg);
#endif
    va_end(arg);

    return r;
}

size_t stringCopy(char* dst, size_t dstSize, const char* src)
{
    size_t i = 0;
    if(dst && dstSize)
    {
        for(; i + 1 < dstSize && src[i]; i++) // copy up to dstSize-1 bytes
            dst[i] = src[i];
        dst[i] = 0; // always null-terminate
    }

    while(src[i]) // read any remaining characters in the src string to get the length
        i++;

    return i;
}

size_t stringCat(char* dst, size_t dstSize, const char* src)
{
    size_t i = 0, s = 0;
    if(dst && dstSize)
    {
        s = strlen(dst);
        for(; i + s + 1 < dstSize && src[i]; i++) // copy until total is at most dstSize-1
            dst[i + s] = src[i];
        dst[i + s] = 0; // always null-terminate
    }

    while(src[i]) // read any remaining characters in the src string to get the length
        i++;

    return i + s;
}

char tolower(char c)
{
    if (c >= 'A' && c <= 'Z')
        c += 32;
    return c;
}

char toupper(char c)
{
    if (c >= 'a' && c <= 'z')
        c -= 32;
    return c;
}

void strlwr(char* str)
{
    for (; *str; str++)
        *str = (char) tolower(*str);
}

void strupr(char* str)
{
    for(; *str; str++)
        *str = (char) toupper(*str);
}

int32_t stringFormatV(char* dst, size_t dstSize, const char* src, va_list arg)
{

#if NV_VC // MSVC is not C99-compliant...
    int32_t result = dst ? ::vsnprintf(dst, dstSize, src, arg) : -1;
    if(dst && (result == int32_t(dstSize) || result < 0))
        dst[dstSize - 1] = 0; // string was truncated or there wasn't room for the NULL
    if(result < 0)
        result = _vscprintf(src, arg); // work out how long the answer would have been.
#else
    int32_t result = ::vsnprintf(dst, dstSize, src, arg);
#endif
    return result;
}

int32_t stricmp(const char* str, const char* str1)
{
#if NV_VC
    return (::_stricmp(str, str1));
#else
    return (::strcasecmp(str, str1));
#endif
}

int32_t strnicmp(const char* str, const char* str1, size_t n)
{
#if NV_VC
    return (::_strnicmp(str, str1, n));
#else
    return (::strncasecmp(str, str1, n));
#endif
}

}

#if NV_PS4 || NV_APPLE_FAMILY
#pragma clang diagnostic pop
#endif

#if NV_WINDOWS_FAMILY
#pragma warning(pop)
#endif
