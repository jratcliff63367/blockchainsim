#ifndef NS_STRING_UTILS_H

#define NS_STRING_UTILS_H

/*************************************************************************************//**
*
* @brief  General purpose string utilities
*
*****************************************************************************************/


#include <stdint.h>
#include "NvPreprocessor.h"

namespace blockchainsim
{

// Lower case a character
char toLower(char c);
// A case insensitive strstr (2048 maximum string length)
const char *stristr(const char *str, const char *key);

// Returns true if the 'key' is find in the source 'str'
bool isstristr(const char *str, const char *key);     // bool true/false based on case insensitive strstr

// Convert a single ASCII character to a hex value
uint8_t getHex(uint8_t c);

// Covnert one source character from hex; returns the new endpointer
uint8_t getHex1(const char *foo, const char **endptr);

// Convert two source ASCII hex values into a 16 bit return.
uint16_t getHex2(const char *foo, const char **endptr);

// Convert four ASCII hex characters into a uint32_t
uint32_t getHex4(const char *foo, const char **endptr);

// Convert up to end of string, or last value hex value, ASCII to HEX
uint32_t getHex(const char *foo, const char **endptr);

// Returns true if this is a whitespace character
bool isWhitespace(char c);

// Skip to next non-whitespace character or EOS
const char * skipWhitespace(const char *str);

const char * formatNumber(int32_t number);
const char * formatUnsignedNumber(uint32_t number);

// Case inensitive string compare
bool striCmp(const char *a, const char *b);

// Returns true/false where true = '1', 't', 'true', 'y', or 'yes' (case-insensitive)
// false = '0', 'f', 'false', 'n', 'no',
// isTrueFalse is true if it was any of these combinations; otherwise it is false
bool isTrue(const char *s,bool &isTrueFalse);

// Returns the last slash in the source string (non-const version); if none returns null
char *lastSlash(char *str);

// Returns the last slash in the source string (const version); if none returns null
const char *lastSlash(const char *str);

// Returns the last period in the source string (non-const version); if none returns null
char *lastDot(char *str);

// Returns the last period in the source string (const version); if none returns null
const char *lastDot(const char *str);


// Returns true if it uses the '$' or 'x' or '0x' prefix, it is assumed to be hex; otherwise base 10 integer.
const char *isHexInt(const char *pf,bool &isHex);

// Converts the input string into an integer.  If it uses the '$' or 'x' or '0x' prefix, it is assumed to be hex; otherwise base 10 integer.
int32_t getHexInt(const char *str);

// converts to 32 bit binary string of zeros and ones.
const char * getBinaryString(uint32_t v);
uint32_t getBinary(const char *str);

bool isOnlyHex(const char *str); // returns true if this string contains only valid hex character.
bool isOnlyBinary(const char *str); // returns true if this string contains only ones and zeros.
bool isValidNumber(const char *str,bool &negativeSign,bool &floatingPoint); // valid number begins with or with out a negative sign.  Can have one decimal point (no more) and the rest are 0-9.  Not taking exponential notation into account at this time.


// See if this string begins with '$' or 'x' or '0x' all considered valid prefixes for hex values.  If true, returns a pointer skipping past the prefix
const char *isHexPrefix(const char *str);
const char *isBinaryPrefix(const char *str);

enum NumericType
{
    NT_HEX,
    NT_BINARY,
    NT_UNSIGNED_INTEGER,
    NT_SIGNED_INTEGER,
    NT_FLOAT,
    NT_NOT_NUMERIC
};

const char *isNumericType(const char *str, NumericType &type,uint32_t &intValue, float &floatValue);

char *getLastSlash(char *c);
const char *getLastSlash(const char *c);
const char *getFirstSlash(const char *c);
bool hasSlash(const char *str); // returns true if this string has slashes in it, suggesting it is a fully qualified path name
bool isSlash(char c);

const char * getFloatString(float v, bool binary,uint32_t stringLimit);

// converts any backslashes to forward slashes; typically used to normalize fully qualified path names
void backslashToForwardslash(char *str);

// converts any forward slashes to backslashes
void forwardSlashToBackslash(char *str);

// Implements this Windows API call in C: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773743(v=vs.85).aspx
char *pathRemoveBackslash(char *path);

// Implements this Windows API call: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773589(v=vs.85).aspx
const char *pathFindFileName(const char *path);

// (non-const version) Implements this Windows API call: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773589(v=vs.85).aspx
char *pathFindFileName(char *path);


// Implements this Windows API call: https://msdn.microsoft.com/en-us/library/windows/desktop/bb773569(v=vs.85).aspx
bool pathCanonicalize(char *outputPath,const char *inputPath);

// returns true if this is an absolute, rather than a relative path.
bool isAbsolutePath(const char *str);

// True if it is an absolute or relative path
bool isAbsoluteOrRelativePath(const char *str);

char *nextSlash(char *str);

// Remaps the root path name
const char *patchFileName(const char *prefix,const char *replacement,const char *fileName);

const char *normalizePathForwardSlash(const char *fname);
const char *normalizePathBackSlash(const char *fname);

NV_FORCE_INLINE uint64_t ptrToUint64(const char *ptr)
{
    uintptr_t number = (uintptr_t)ptr;
    return uint64_t(number);
}

// Returns how many forward or backslashes or in this path name.
uint32_t getSlashCount(const char *path);

// Return a pointer to the n(th) slash
const char * getSlashIndex(const char *path,uint32_t slashNo);

// Normalize slashes; use backslash on windows; forwardslash on android
void normalizeSlashes(char *path);

}

#endif
