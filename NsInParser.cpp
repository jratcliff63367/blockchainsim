#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include "NsInParser.h"

#include "NvAssert.h"
#include "NsUserAllocated.h"

/** @file inparser.cpp
 * @brief        Parse ASCII text, in place, very quickly.
 *
 * This class provides for high speed in-place (destructive) parsing of an ASCII text file.
 * This class will either load an ASCII text file from disk, or can be constructed with a pointer to
 * a piece of ASCII text in memory.  It can only be called once, and the contents are destroyed.
 * To speed the process of parsing, it simply builds pointers to the original ascii data and replaces the
 * seperators with a zero byte to indicate end of string.  It performs callbacks to parse each line, in argc/argv format,
 * offering the option to cancel the parsing process at any time.
 *
 *
 * By default the only valid seperator is whitespace.  It will not treat commas or any other symbol as a separator.
 * However, you can specify up to 32 'hard' seperators, such as a comma, equal sign, etc. and these will act as valid
 * seperators and come back as part of the argc/argv data list.
 *
 * To use the parser simply inherit the pure virtual base class 'InPlaceParserInterface'.  Define the method 'ParseLine'.
 * When you invoke the Parse method on the InPlaceParser class, you will get an ARGC - ARGV style callback for each line
 * in the source file.  If you return 'false' at any time, it will abort parsing.  The entire thing is stack based, so you
 * can recursively call multiple parser instances.
 *
 * It is important to note.  Since this parser is 'in place' it writes 'zero bytes' (EOS marker) on top of the whitespace.
 * While it can handle text in quotes, it does not handle escape sequences.  This is a limitation which could be resolved.
 * There is a hard coded maximum limit of 512 arguments per line.
 *
 * Here is the full example usage:
 *
 *  InPlaceParser ipp("parse_me.txt");
 *
 *    ipp.Parse(this);
 *
 *  That's it, and you will receive an ARGC - ARGV callback for every line in the file.
 *
 *  If you want to parse some text in memory of your own. (It *MUST* be terminated by a zero byte, and lines seperated by carriage return
 *  or line-feed.  You will receive an assertion if it does not.  If you specify the source data than *you* are responsible for that memory
 *  and must de-allocate it yourself.  If the data was loaded from a file on disk, then it is automatically de-allocated by the InPlaceParser.
 *
 *  You can also construct the InPlaceParser without passing any data, so you can simply pass it a line of data at a time yourself.  The
 *  line of data should be zero-byte terminated.
*/

//==================================================================================

namespace blockchainsim
{

void InPlaceParser::SetFile(const char *fname)
{
    if ( mMyAlloc )
    {
        NV_FREE(mData);
    }
    mData = 0;
    mLen  = 0;
    mMyAlloc = false;

    FILE *fph = fopen(fname,"rb");
    if ( fph )
    {
        fseek(fph,0L,SEEK_END);
        mLen = ftell(fph);
        fseek(fph,0L,SEEK_SET);

        if ( mLen )
        {
            mData = (char *) NV_ALLOC(sizeof(char)*(mLen+1), NV_DEBUG_EXP("InPlaceParser"));
            int32_t read = (int32_t)fread(mData,(size_t)mLen,1,fph);
            if ( !read )
            {
                NV_FREE(mData);
                mData = 0;
            }
            else
            {
                mData[mLen] = 0; // zero byte terminate end of file marker.
                mMyAlloc = true;
            }
        }
        fclose(fph);
    }
}

//==================================================================================
InPlaceParser::~InPlaceParser(void)
{
    if ( mMyAlloc )
    {
        NV_FREE(mData);
    }
}

//==================================================================================
bool InPlaceParser::IsHard(char c)
{
    return mHard[(unsigned char)c] == ST_HARD;
}

//==================================================================================
char * InPlaceParser::AddHard(int32_t &argc,const char **argv,SeparatorType *types,char *foo)
{
    while ( IsHard(*foo) )
    {
        const char *hard = &mHardString[*foo*2];
        if ( argc < MAXARGS )
        {
            types[argc] = ST_HARD;
            argv[argc++] = hard;
        }
        ++foo;
    }
    return foo;
}

//==================================================================================
bool   InPlaceParser::IsWhiteSpace(char c)
{
    return mHard[(unsigned char)c] == ST_SOFT;
}

//==================================================================================
char * InPlaceParser::SkipSpaces(char *foo)
{
    while ( !EOS(*foo) && IsWhiteSpace(*foo) )
        ++foo;
    return foo;
}

//==================================================================================
bool InPlaceParser::IsNonSeparator(char c)
{
    return ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 );
}

//==================================================================================
int32_t InPlaceParser::ProcessLine(int32_t lineno,char *line,InPlaceParserInterface *callback)
{
    int32_t ret = 0;

    const char *argv[MAXARGS];
    SeparatorType types[MAXARGS];
    int32_t argc = 0;

    char *foo = line;

    while ( !EOS(*foo) && argc < MAXARGS )
    {
        foo = SkipSpaces(foo); // skip any leading spaces

        if ( EOS(*foo) )
            break;

        if ( *foo == mQuoteChar ) // if it is an open quote
        {
            ++foo;
            if ( argc < MAXARGS )
            {
                types[argc] = ST_QUOTE;
                argv[argc++] = foo;
            }
            while ( !EOS(*foo) && *foo != mQuoteChar )
                ++foo;
            if ( !EOS(*foo) )
            {
                *foo = 0; // replace close quote with zero byte EOS
                ++foo;
            }
        }
        else
        {
            foo = AddHard(argc,argv,types,foo); // add any hard separators, skip any spaces

            if ( IsNonSeparator(*foo) )  // add non-hard argument.
            {
                bool quote  = false;
                if ( *foo == mQuoteChar )
                {
                    ++foo;
                    quote = true;
                }

                if ( argc < MAXARGS )
                {
                    types[argc] = ST_DATA;
                    argv[argc++] = foo;
                }

                if ( quote )
                {
                    while (*foo && *foo != mQuoteChar )
                        ++foo;
                    if ( *foo )
                        *foo = 32;
                }

                // continue..until we hit an eos ..
                while ( !EOS(*foo) ) // until we hit EOS
                {
                    if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
                    {
                        *foo = 0;
                        ++foo;
                        break;
                    }
                    else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
                    {
                        const char *hard = &mHardString[*foo*2];
                        *foo = 0;
                        if ( argc < MAXARGS )
                        {
                            types[argc] = ST_HARD;
                            argv[argc++] = hard;
                        }
                        ++foo;
                        break;
                    }
                    ++foo;
                } // end of while loop...
            }
        }
    }

    if ( argc )
    {
        ret = callback->ParseLine(lineno, argc, argv, types );
    }

    return ret;
}


int32_t  InPlaceParser::Parse(const char *str,InPlaceParserInterface *callback) // returns true if entire file was parsed, false if it aborted for some reason
{
  int32_t ret = 0;

  mLen = (int32_t)strlen(str);
  if ( mLen )
  {
    mData = (char *)NV_ALLOC((size_t)mLen+1, NV_DEBUG_EXP("InPlaceParser"));
    strcpy(mData,str);
    mMyAlloc = true;
    ret = Parse(callback);
  }
  return ret;
}


int32_t InPlaceParser::internalProcessLine(int32_t lineno, char *begin, InPlaceParserInterface *callback)
{
    int32_t ret = 0;
    bool snarfed = callback->preParseLine(lineno, begin);
    if (!snarfed)
    {
        if (mIgnoreCComments)
        {
            char *scan = begin;
            // First look for any end-of-line comments; stomp a zero byte if one exists.
            while (*scan)
            {
                if (scan[0] == '/' && scan[1] == '/')
                {
                    *scan = 0;
                    break;
                }
                scan++;
            }
            // Now we copy all of the data which is not nested inside a comment block
            char *dest = begin;
            const char *source = begin;
            while (*source)
            {
                if (source[0] == '/' && source[1] == '*')
                {
                    mInsideCommentBlock = true;
                    source+=2;
                }
                else if (source[0] == '*' && source[1] == '/')
                {
                    mInsideCommentBlock = false;
                    source += 2;
                }
                else if (!mInsideCommentBlock)
                {
                    *dest++ = *source++;
                }
                else
                {
                    source++;
                }
            }
            *dest = 0;
        }
        if (*begin)
        {
            ret = ProcessLine(lineno, begin, callback);
        }
    }
    return ret;
}

//==================================================================================
// returns true if entire file was parsed, false if it aborted for some reason
//==================================================================================
int32_t  InPlaceParser::Parse(InPlaceParserInterface *callback)
{
    int32_t ret = 0;
    NV_ASSERT(callback, "AssertFailure");
    if ( mData )
    {
        int32_t lineno = 0;

        char *foo   = mData;
        char *begin = foo;

        while ( *foo )
        {
            if ( isLineFeed(*foo) )
            {
                ++lineno;
                *foo = 0;
                if ( *begin ) // if there is any data to parse at all...
                {
                    int32_t v = internalProcessLine(lineno,begin,callback);
                    if ( v )
                    {
                        ret = v;
                    }
                }

                ++foo;
                if (*foo == 10)
                {
                    ++foo; // skip line feed, if it is in the carriage-return line-feed format...
                }
                begin = foo;
            }
            else
            {
                ++foo;
            }
        }

        lineno++; // last line.

        int32_t v = internalProcessLine(lineno,begin,callback);
        if (v)
        {
            ret = v;
        }
    }
    return ret;
}

//==================================================================================
void InPlaceParser::DefaultSymbols(void)
{
    SetHardSeparator(',');
    SetHardSeparator('(');
    SetHardSeparator(')');
    SetHardSeparator('=');
    SetHardSeparator('[');
    SetHardSeparator(']');
    SetHardSeparator('{');
    SetHardSeparator('}');
    SetCommentSymbol('#');
}

//==================================================================================
// convert source string into an arg list, this is a destructive parse.
//==================================================================================
const char ** InPlaceParser::GetArglist(char *line,int32_t &count)
{
    const char **ret = 0;

    int32_t argc = 0;

    char *foo = line;

    while ( !EOS(*foo) && argc < MAXARGS )
    {
        foo = SkipSpaces(foo); // skip any leading spaces

        if ( EOS(*foo) )
            break;

        if ( *foo == mQuoteChar ) // if it is an open quote
        {
            ++foo;
            if ( argc < MAXARGS )
            {
                mTypes[argc] = ST_QUOTE;
                mArgv[argc++] = foo;
            }
            while ( !EOS(*foo) && *foo != mQuoteChar )
                ++foo;
            if ( !EOS(*foo) )
            {
                *foo = 0; // replace close quote with zero byte EOS
                ++foo;
            }
        }
        else
        {
            foo = AddHard(argc,mArgv,mTypes,foo); // add any hard separators, skip any spaces

            if ( IsNonSeparator(*foo) )  // add non-hard argument.
            {
                bool quote  = false;
                if ( *foo == mQuoteChar )
                {
                    ++foo;
                    quote = true;
                }

                if ( argc < MAXARGS )
                {
                    mTypes[argc] = ST_DATA;
                    mArgv[argc++] = foo;
                }

                if ( quote )
                {
                    while (*foo && *foo != mQuoteChar )
                        ++foo;
                    if ( *foo )
                        *foo = 32;
                }

                // continue..until we hit an eos ..
                while ( !EOS(*foo) ) // until we hit EOS
                {
                    if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
                    {
                        *foo = 0;
                        ++foo;
                        break;
                    }
                    else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
                    {
                        const char *hard = &mHardString[*foo*2];
                        *foo = 0;
                        if ( argc < MAXARGS )
                        {
                            mTypes[argc] = ST_HARD;
                            mArgv[argc++] = hard;
                        }
                        ++foo;
                        break;
                    }
                    ++foo;
                } // end of while loop...
            }
        }
    }

    count = argc;
    if ( argc )
    {
        ret = mArgv;
    }

    return ret;
}


}
