#include "NsStringUtils.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "NsString.h"
#include "NvAssert.h"
#include "NvMath.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


namespace blockchainsim
{

char toLower(char c)
{
    if (c >= 'A' && c <= 'Z') c += 32;
    return c;
}

const char *stristr(const char *str, const char *key)       // case insensitive str str
{
    NV_ASSERT(strlen(str) < 2048, "AssertFailure");
    NV_ASSERT(strlen(key) < 2048, "AssertFailure");
    char istr[2048];
    char ikey[2048];
    strncpy(istr, str, 2048);
    strncpy(ikey, key, 2048);
    blockchainsim::strlwr(istr);
    blockchainsim::strlwr(ikey);

    char *foo = strstr(istr, ikey);
    if (foo)
    {
        uint32_t loc = (uint32_t)(foo - istr);
        foo = (char *)str + loc;
    }

    return foo;
}

bool isstristr(const char *str, const char *key)     // bool true/false based on case insenstive strstr
{
    bool ret = false;
    const char *found = strstr(str, key);
    if (found) ret = true;
    return ret;
}

uint8_t getHex(uint8_t c)
{
    uint8_t v = 0;
    c = toLower(c);
    if (c >= '0' && c <= '9')
        v = c - '0';
    else
    {
        if (c >= 'a' && c <= 'f')
        {
            v = 10 + c - 'a';
        }
    }
    return v;
}

uint8_t getHex1(const char *foo, const char **endptr)
{
    uint32_t ret = 0;

    ret = (getHex(foo[0]) << 4) | getHex(foo[1]);

    if (endptr)
    {
        *endptr = foo + 2;
    }

    return (uint8_t)ret;
}


uint16_t getHex2(const char *foo, const char **endptr)
{
    uint32_t ret = 0;

    ret = (getHex(foo[0]) << 12) | (getHex(foo[1]) << 8) | (getHex(foo[2]) << 4) | getHex(foo[3]);

    if (endptr)
    {
        *endptr = foo + 4;
    }

    return (uint16_t)ret;
}

uint32_t getHex4(const char *foo, const char **endptr)
{
    uint32_t ret = 0;

    for (uint32_t i = 0; i < 8; i++)
    {
        ret = (ret << 4) | getHex(foo[i]);
    }

    if (endptr)
    {
        *endptr = foo + 8;
    }

    return ret;
}

uint32_t getHex(const char *foo, const char **endptr)
{
    uint32_t ret = 0;

    while (*foo)
    {
        uint8_t c = toLower(*foo);
        uint32_t v = 0;
        if (c >= '0' && c <= '9')
            v = c - '0';
        else
        {
            if (c >= 'a' && c <= 'f')
            {
                v = 10 + c - 'a';
            }
            else
                break;
        }
        ret = (ret << 4) | v;
        foo++;
    }

    if (endptr) *endptr = foo;

    return ret;
}


bool isWhitespace(char c)
{
    if (c == ' ' || c == 9 || c == 13 || c == 10 || c == ',') return true;
    return false;
}


const char * skipWhitespace(const char *str)
{
    while (*str && isWhitespace(*str)) str++;
    return str;
}

#define MAXNUMERIC 32  // JWR  support up to 16 32 character long numeric formated strings
#define MAXFNUM    16

static  char  gFormat[MAXNUMERIC*MAXFNUM];
static int32_t    gIndex = 0;

const char * formatNumber(int32_t number) // JWR  format this integer into a fancy comma delimited string
{
    char * dest = &gFormat[gIndex*MAXNUMERIC];
    gIndex++;
    if (gIndex == MAXFNUM) gIndex = 0;

    char scratch[512];
    blockchainsim::stringFormat(scratch, 512, "%d", number);

    char *source = scratch;
    char *str = dest;
    uint32_t len = (uint32_t)strlen(scratch);
    if (scratch[0] == '-')
    {
        *str++ = '-';
        source++;
        len--;
    }
    for (uint32_t i = 0; i < len; i++)
    {
        int32_t place = (len - 1) - i;
        *str++ = source[i];
        if (place && (place % 3) == 0) *str++ = ',';
    }
    *str = 0;

    return dest;
}

const char * formatUnsignedNumber(uint32_t number) // JWR  format this integer into a fancy comma delimited string
{
    char * dest = &gFormat[gIndex*MAXNUMERIC];
    gIndex++;
    if (gIndex == MAXFNUM) gIndex = 0;

    char scratch[512];
    blockchainsim::stringFormat(scratch, 512, "%u", number);

    char *source = scratch;
    char *str = dest;
    uint32_t len = (uint32_t)strlen(scratch);
    if (scratch[0] == '-')
    {
        *str++ = '-';
        source++;
        len--;
    }
    for (uint32_t i = 0; i < len; i++)
    {
        int32_t place = (len - 1) - i;
        *str++ = source[i];
        if (place && (place % 3) == 0) *str++ = ',';
    }
    *str = 0;

    return dest;
}


bool striCmp(const char *a, const char *b)
{
    bool ret = false;

    if (a && b)
    {
        ret = blockchainsim::stricmp(a, b) == 0;
    }

    return ret;
}

bool isTrue(const char *s, bool &isTrueFalse)
{
    bool ret = false;
    isTrueFalse = false;
    if (s)
    {
        if (striCmp(s, "1") ||
            striCmp(s, "t") ||
            striCmp(s, "true") ||
            striCmp(s, "y") ||
            striCmp(s, "yes") ||
            striCmp(s, "on"))
        {
            ret = true;
            isTrueFalse = true;
        }
        else if (striCmp(s, "0") ||
            striCmp(s, "f") ||
            striCmp(s, "false") ||
            striCmp(s, "n") ||
            striCmp(s, "no") ||
            striCmp(s, "off") )
        {
            isTrueFalse = true;
        }
    }
    return ret;
}

// Returns the last slash in the source string (non-const version); if none returns null
char *lastSlash(char *str)
{
    char *ret = NULL;
    while (*str)
    {
        if (*str == '\\' || *str == '/')
        {
            ret = str;
        }
        str++;
    }
    return ret;
}

// Returns the last slash in the source string (const version); if none returns null
const char *lastSlash(const char *str)
{
    const char *ret = NULL;
    while (*str)
    {
        if (*str == '\\' || *str == '/')
        {
            ret = str;
        }
        str++;
    }
    return ret;

}

// Returns the last slash in the source string (non-const version); if none returns null
char *lastDot(char *str)
{
    char *ret = NULL;
    while (*str)
    {
        if (*str == '.')
        {
            ret = str;
        }
        str++;
    }
    return ret;
}

// Returns the last slash in the source string (const version); if none returns null
const char *lastDot(const char *str)
{
    const char *ret = NULL;
    while (*str)
    {
        if (*str == '.' )
        {
            ret = str;
        }
        str++;
    }
    return ret;

}


const char *isHexInt(const char *pf,bool &isHex,bool &isNegative)
{
    const char *ret = nullptr;

    isNegative = false;
    bool floatingPoint;
    isHex = false;
    bool negativeSign;
    if (pf[0] == '-')
    {
        isNegative = true;
        pf++;
    }
    if (pf[0] == '$' || toLower(pf[0]) == 'x')
    {
        pf++;
        if (isOnlyHex(pf))
        {
            isHex = true;
            ret = pf;
        }
    }
    else if (pf[0] == '0' && toLower(pf[1]) == 'x')
    {
        pf += 2;
        if (isOnlyHex(pf))
        {
            isHex = true;
            ret = pf;
        }
    }
    else if (isValidNumber(pf,negativeSign,floatingPoint) )
    {
        if (!floatingPoint)
        {
            ret = pf;
        }
    }
    return ret;
}

int32_t getHexInt(const char *pf)
{
    int32_t offset = 0;

    bool isHex;
    bool isNegative;
    pf = isHexInt(pf, isHex, isNegative);
    if (pf)
    {
        if (isHex)
        {
            offset = int32_t(getHex(pf, nullptr));
        }
        else
        {
            offset = atoi(pf);
        }
    }
    if (isNegative)
    {
        offset *= -1;
    }
    return offset;
}

const char * getBinaryString(uint32_t v)
{
    static char temp[37];
    uint32_t bit = (1U << 31);
    char *dest = temp;

    for (uint32_t i = 0; i < 32; i++)
    {
        if (bit & v)
        {
            *dest++ = '1';
        }
        else
        {
            *dest++ = '0';
        }
        bit = bit >> 1;
        if (((i+1) & 7) == 0)
        {
            *dest++ = ' ';
        }
    }
    *dest = 0;
    return temp;
}

bool isOnlyHex(const char *str) // returns true if this string contains only valid hex character.
{
    bool ret = true;

    while (*str)
    {
        char c = *str++;
        c = toLower(c);
        if (c >= '0' && c <= '9')
        {
            // it's ok..
        }
        else if (c >= 'a' && c <= 'f')
        {
            // it's ok.
        }
        else
        {
            ret = false;
            break;
        }
    }

    return ret;
}

bool isOnlyBinary(const char *str) // returns true if this string contains only ones and zeros.
{
    bool ret = true;

    while (*str)
    {
        char c = *str++;
        if (c == '0' || c == '1' || c == 32 )
        {
            // it's ok
        }
        else
        {
            ret = false;
            break;
        }
    }

    return ret;
}

bool isValidNumber(const char *str, bool &negativeSign, bool &floatingPoint) // valid number begins with or with out a negative sign.  Can have one decimal point (no more) and the rest are 0-9.  Not taking exponential notation into account at this time.
{
    bool ret = true;

    floatingPoint = false;
    uint32_t decimalCount = 0;
    uint32_t colonCount = 0;

    if (*str == '-') // if it begins with a negative sign...
    {
        negativeSign = true;
        str++;
    }
    else
    {
        negativeSign = false;
    }
    while (*str)
    {
        char c = *str++;
        if (c == '.')
        {
            floatingPoint = true;
            decimalCount++;
            if (decimalCount > 1)
            {
                ret = false;
                break;
            }
        }
        else if (c >= '0' && c <= '9')
        {
            // it's good
        }
        else if (c == ':' || c == ' ' )
        {
            // we are going to allow the 'colon' character as a valid separator for paired singles
            if (c == ':')
            {
                colonCount++;
                decimalCount = 0;
                if (colonCount > 1)
                {
                    ret = false;
                    break;
                }
                while (*str == 32) *str++; // skip spaces...
                if (*str == '-')
                {
                    str++;
                    negativeSign = true;
                }
            }
        }
        else
        {
            ret = false;
            break;
        }
    }
    return ret;
}

uint32_t getBinary(const char *str)
{
    uint32_t ret = 0;

    while (*str)
    {
        if (*str == 32)
        {
            str++;
        }
        else
        {
            ret = ret << 1;
            char c = *str++;
            if (c == '1')
            {
                ret |= 1;
            }
        }
    }
    return ret;
}

const char *isHexPrefix(const char *str)
{
    const char *ret = NULL;

    if (str[0] == '$')
    {
        str++;
        ret = str;
    }
    else if (toLower(str[0]) == 'x')
    {
        str++;
        ret = str;
    }
    else if (str[0] == '0' && toLower(str[1]) == 'x')
    {
        str += 2;
        ret = str;
    }

    return ret;
}

const char *isBinaryPrefix(const char *str)
{
    const char *ret = nullptr;

    if (toLower(str[0]) == 'b')
    {
        str++;
        ret = str;
    }
    else if (str[0] == '0' && toLower(str[1]) == 'b')
    {
        str += 2;
        ret = str;
    }

    return ret;
}

char *getLastSlash(char *c)
{
    char *ret = nullptr;

    while (*c)
    {
        if (*c == '/' || *c == '\\')
        {
            ret = c;
        }
        c++;
    }

    return ret;
}

const char *getLastSlash(const char *c)
{
    const char *ret = nullptr;

    while (*c)
    {
        if (*c == '/' || *c == '\\')
        {
            ret = c;
        }
        c++;
    }

    return ret;
}


bool hasSlash(const char *str)
{
    bool ret = false;

    while (*str)
    {
        if (*str == '/' || *str == '\\')
        {
            ret = true;
            break;
        }
        str++;
    }


    return ret;
}

const char * getFloatString(float v, bool binary,uint32_t stringLimit)
{
    static char data[64 * 16];
    static uint32_t  index = 0;

    char *ret = &data[index * 64];
    index++;
    if (index == 16) index = 0;

    if (!blockchainsim::NvIsFinite(v))
    {
        blockchainsim::stringFormat(ret, 64, "%.9f", v);
    }
    else if ( v == FLT_MAX )
    {
        strcpy(ret,"fltmax");
    }
    else if ( v == FLT_MIN )
    {
        strcpy(ret,"fltmin");
    }
    else if (v == 1)
    {
        strcpy(ret, "1");
    }
    else if (v == 0)
    {
        strcpy(ret, "0");
    }
    else if (v == -1)
    {
        strcpy(ret, "-1");
    }
    else
    {
        if (binary)
        {
            uint32_t *iv = (uint32_t *)&v;
            blockchainsim::stringFormat(ret,64, "%.4f$%x", v, *iv);
        }
        else
        {
            blockchainsim::stringFormat(ret,64, "%.9f", v);
            const char *dot = strstr(ret, ".");
            if (dot)
            {
                uint32_t len = (uint32_t)strlen(ret);
                char *foo = &ret[len - 1];
                while (*foo == '0') foo--;
                if (*foo == '.')
                    *foo = 0;
                else
                    foo[1] = 0;
            }
        }
    }
    size_t len = strlen(ret);
    if (len >= stringLimit)
    {
        uint32_t iv = *(uint32_t *)&v;
        blockchainsim::stringFormat(ret, 64, "$%08X", iv);
    }

    return ret;
}

const char *isNumericType(const char *pf, NumericType &type,uint32_t &intValue,float &floatValue)
{
    const char *ret = nullptr;

    bool negativeSign;
    bool floatingPoint;
    type = NT_NOT_NUMERIC;
    intValue = 0;
    floatValue = 0;
    if (pf[0] == '$' || toLower(pf[0]) == 'x')
    {
        if (isOnlyHex(pf+1))
        {
            type = NT_HEX;
            ret = pf+1;
            intValue = getHex(ret, nullptr);
        }
    }
    else if (pf[0] == '0' && toLower(pf[1]) == 'x')
    {
        if (isOnlyHex(pf+2))
        {
            type = NT_HEX;
            ret = pf+2;
            intValue = getHex(ret, nullptr);
        }
    }
    else if ( toLower(pf[0]) == 'b')
    {
        if (isOnlyBinary(pf + 1))
        {
            type = NT_BINARY;
            ret = pf + 1;
            intValue = getBinary(ret);
        }
    }
    else if (pf[0] == '0' && toLower(pf[1]) == 'b')
    {
        if (isOnlyBinary(pf + 2))
        {
            type = NT_HEX;
            ret = pf + 2;
            intValue = getBinary(ret);
        }
    }
    else if (isValidNumber(pf, negativeSign, floatingPoint))
    {
        ret = pf;
        if ( floatingPoint )
        {
            type = NT_FLOAT;
            floatValue = (float)atof(ret);
        }
        else if (negativeSign)
        {
            type = NT_SIGNED_INTEGER;
            intValue = (uint32_t)atoi(ret);
        }
        else
        {
            type = NT_UNSIGNED_INTEGER;
            intValue = (uint32_t)atoi(ret);
        }
    }


    return ret;
}

void backslashToForwardslash(char *str)
{
    while (*str)
    {
        if (*str == '\\')
        {
            *str = '/';
        }
        str++;
    }
}

void forwardSlashToBackslash(char *str)
{
    while (*str)
    {
        if (*str == '/')
        {
            *str = '\\';
        }
        str++;
    }
}

char *pathRemoveBackslash(char *path)
{
    char *ret = path;
    if (path)
    {
        size_t len = strlen(path);
        if (len)
        {
            char *lastChar = &path[len - 1];
            if (*lastChar == '\\' || *lastChar == '/')
            {
                *lastChar = 0;
                ret = lastChar;
            }
            else
            {
                ret = lastChar;
            }
        }
    }
    return ret;
}

bool isSlash(char c)
{
    return (c == '/' || c == '\\');
}

// Implements this Windows API call: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773569(v=vs.85).aspx
bool pathCanonicalize(char *dest, const char *src)
{
    bool valid = true;

#define MAX_SLASHES 32
    char *slashes[MAX_SLASHES];
    uint32_t slashCount = 0;
    while (*src)
    {
        if (isSlash(src[0]))
        {
            if (src[1] == '.' && src[2] == '.')
            {
                if (slashCount)
                {
                    dest = slashes[slashCount - 1];
                    slashCount--;
                }
                else
                {
                    *dest++ = src[0];
                    valid = false;
                }
                src += 3;
            }
            else if (src[1] == '.')
            {
                if (slashCount == 0)
                {
                    *dest++ = src[0];
                    valid = false;
                }
                src += 2; // just skip \.
            }
            else
            {
                if (slashCount < MAX_SLASHES)
                {
                    slashes[slashCount] = dest;
                    slashCount++;
                }
                else
                {
                    valid = false;
                }
                *dest++ = *src++;
            }
        }
        else
        {
            *dest++ = *src++;
        }
    }
    *dest = 0;
    return valid;

}

// Implements this Windows API call: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773589(v=vs.85).aspx
const char *pathFindFileName(const char *path)
{
    const char *ret = path;
    while (*path)
    {
        if (isSlash(path[0]) && path[1])
        {
            ret = path;
        }
        path++;
    }
    if (isSlash(*ret))
    {
        ret++;
    }
    return ret;
}

// Implements this Windows API call: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773589(v=vs.85).aspx
char *pathFindFileName(char *path)
{
    char *ret = path;
    while (*path)
    {
        if (isSlash(path[0]) && path[1])
        {
            ret = path;
        }
        path++;
    }
    if (isSlash(*ret))
    {
        ret++;
    }
    return ret;
}



bool isAbsolutePath(const char *str)
{
    const char *found = strchr(str, ':');
    return found ? true : false;
}

const char *getFirstSlash(const char *c)
{
    const char *ret = nullptr;

    while (*c)
    {
        if (isSlash(*c))
        {
            ret = c;
            break;
        }
        c++;
    }

    return ret;
}

// True if it is an absolute or relative path
bool isAbsoluteOrRelativePath(const char *str)
{
    bool ret = false;

    while (*str)
    {
        char c = *str++;
        if (c == ':' || isSlash(c) )
        {
            ret = true;
            break;
        }
    }

    return ret;
}

char *nextSlash(char *str)
{
    char *ret = nullptr;

    while (*str)
    {
        if (isSlash(*str))
        {
            ret = str;
            break;
        }
        str++;
    }

    return ret;
}

void normalizePath(char *dest, const char *src)
{
    strcpy(dest, src);
    blockchainsim::strlwr(dest);
    backslashToForwardslash(dest);
}

const char *patchFileName(const char *_prefix, const char *_replacement, const char *_fileName)
{
    const char *ret = _fileName;

    static char scratch[512];
    char prefix[512];
    char fileName[512];

    normalizePath(prefix, _prefix);
    normalizePath(fileName, _fileName);
    size_t len = strlen(prefix);
    if (strncmp(prefix, fileName, len) == 0)
    {
        strcpy(scratch, _replacement);
        strcat(scratch, &_fileName[len]);
        ret = scratch;
    }
    return ret;
}

const char *normalizePathForwardSlash(const char *fname)
{
    static char scratch[512];
    pathCanonicalize(scratch, fname);
    backslashToForwardslash(scratch);
    pathRemoveBackslash(scratch);
    return scratch;
}
const char *normalizePathBackSlash(const char *fname)
{
    static char scratch[512];
    pathCanonicalize(scratch, fname);
    forwardSlashToBackslash(scratch);
    pathRemoveBackslash(scratch);
    return scratch;

}

// Returns how many forward or backslashes or in this path name.
uint32_t getSlashCount(const char *path)
{
    uint32_t ret = 0;

    while (*path)
    {
        if (isSlash(*path))
        {
            ret++;
        }
        path++;
    }

    return ret;
}

// Return a pointer to the n(th) slash
const char * getSlashIndex(const char *path,uint32_t slashNo)
{
    const char *ret = nullptr;

    uint32_t index = 0;
    while (*path)
    {
        if (isSlash(*path))
        {
            if (slashNo == index)
            {
                ret = path;
                break;
            }
            index++;
        }
        path++;
    }
    return ret;
}

// Normalize slashes; use backslash on windows; forwardslash on android
void normalizeSlashes(char *path)
{
#if NV_WINDOWS_FAMILY
    forwardSlashToBackslash(path);
#else
    backslashToForwardslash(path);
#endif
}

} // end of lingcod namespace
