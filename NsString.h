#ifndef NV_NSFOUNDATION_NSSTRING_H
#define NV_NSFOUNDATION_NSSTRING_H

#include "NvPreprocessor.h"
#include <stdarg.h>
#include <stdint.h>

namespace blockchainsim
{

// the following functions have C99 semantics. Note that C99 requires for stringFormat and stringFormatV:
// * the resulting string is always NULL-terminated regardless of truncation.
// * in the case of truncation the return value is the number of characters that would have been created.

 int32_t sscanf(const char* buffer, const char* format, ...);
 int32_t stringCompare(const char* str1, const char* str2);
 int32_t stringCompareN(const char* str1, const char* str2, size_t count);
 int32_t stringFormat(char* dst, size_t dstSize, const char* format, ...);
 int32_t stringFormatV(char* dst, size_t dstSize, const char* src, va_list arg);

// These versions probe the size of the buffer preventing basic copy and paste errors, and errors due to changing
// a char buffer[x] to char *buffer.
template <size_t _Size>
inline int32_t stringFormat(char (&dst)[_Size], const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = stringFormatV(dst, _Size, format, args);
    va_end(args);
    return ret;
}

template <size_t _Size>
inline int32_t stringFormatV(char(&dst)[_Size], const char *src, va_list args)
{
    return stringFormatV(dst, _Size, src, args);
}


// stringCat and stringCopy have BSD semantics:
// * dstSize is always the size of the destination buffer
// * the resulting string is always NULL-terminated regardless of truncation
// * in the case of truncation the return value is the length of the string that would have been created

 size_t stringCat(char* dst, size_t dstSize, const char* src);
 size_t stringCopy(char* dst, size_t dstSize, const char* src);

template <size_t _Size>
NV_FORCE_INLINE size_t stringCat(char(&dst)[_Size], const char* src)
{
    return stringCat(dst, _Size, src);
}

template <size_t _Size>
NV_FORCE_INLINE size_t stringCopy(char(&dst)[_Size], const char* src)
{
    return stringCopy(dst, _Size, src);
}

// case-insensitive string comparison
 int32_t stricmp(const char* str1, const char* str2);
 int32_t strnicmp(const char* str1, const char* str2, size_t count);

// in-place string case conversion
 void strlwr(char* str);
 void strupr(char* str);

 char tolower(char c);
 char toupper(char c);


}
#endif // #ifndef NV_NSFOUNDATION_NSSTRING_H
