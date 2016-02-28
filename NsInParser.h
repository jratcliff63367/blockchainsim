#ifndef EXT_INPARSER_H
#define EXT_INPARSER_H

#define MAXARGS 512
#include <stdint.h>

/** @file inparser.h
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
 * You can specify any character to be  a 'hard' seperator, such as an '=' for example and that will come back as a
 * distinct argument string.
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

#include "NsUserAllocated.h"

namespace blockchainsim
{

enum SeparatorType
{
    ST_DATA,        // is data
    ST_HARD,        // is a hard separator
    ST_SOFT,        // is a soft separator
    ST_EOS,          // is a comment symbol, and everything past this character should be ignored
    ST_QUOTE,       // a quotated string
    ST_LINE_FEED
};


class InPlaceParserInterface
{
public:
    virtual int32_t ParseLine(int32_t lineno,int32_t argc,const char **argv,SeparatorType *types) =0;  // return TRUE to continue parsing, return FALSE to abort parsing process
    virtual bool preParseLine(int32_t /* lineno */,const char * /* line */)  { return false; } // optional chance to pre-parse the line as raw data.  If you return 'true' the line will be skipped assuming you snarfed it.
protected:
    virtual ~InPlaceParserInterface(void) {}
};


class InPlaceParser : public blockchainsim::UserAllocated
{
public:
    InPlaceParser(void)
    {
        Init();
    }

    InPlaceParser(char *data,int32_t len)
    {
        Init();
        SetSourceData(data,len);
    }

    InPlaceParser(const char *fname)
    {
        Init();
        SetFile(fname);
    }

    ~InPlaceParser(void);

    void Init(void)
    {
        mInsideCommentBlock = false;
        mIgnoreCComments = false;
        mQuoteChar = 34;
        mData = 0;
        mLen  = 0;
        mMyAlloc = false;
        for (int32_t i=0; i<256; i++)
        {
            mHard[i] = ST_DATA;
            mHardString[i*2] = (char)i;
            mHardString[i*2+1] = 0;
        }
        mHard[0]  = ST_EOS;
        mHard[32] = ST_SOFT;
        mHard[9]  = ST_SOFT;
        mHard[13] = ST_LINE_FEED;
        mHard[10] = ST_LINE_FEED;
    }

    void SetFile(const char *fname);

    void SetSourceData(char *data,int32_t len)
    {
        mData = data;
        mLen  = len;
        mMyAlloc = false;
    }

    void setMyAlloc(bool state)
    {
        mMyAlloc = state;
    }

    int32_t  Parse(const char *str,InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason
    int32_t  Parse(InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason

    int32_t ProcessLine(int32_t lineno,char *line,InPlaceParserInterface *callback);

    const char ** GetArglist(char *source,int32_t &count); // convert source string into an arg list, this is a destructive parse.

    void SetHardSeparator(char c) // add a hard separator
    {
        mHard[(unsigned char)c] = ST_HARD;
    }

    void SetHard(char c) // add a hard separator
    {
        mHard[(unsigned char)c] = ST_HARD;
    }

    void SetSoft(char c) // add a hard separator
    {
        mHard[(unsigned char)c] = ST_SOFT;
    }


    void SetCommentSymbol(char c) // comment character, treated as 'end of string'
    {
        mHard[(unsigned char)c] = ST_EOS;
    }

    void ClearHardSeparator(char c)
    {
        mHard[(unsigned char)c] = ST_DATA;
    }


    void DefaultSymbols(void); // set up default symbols for hard seperator and comment symbol of the '#' character.

    bool EOS(char c)
    {
        if ( mHard[(unsigned char)c] == ST_EOS )
        {
            return true;
        }
        return false;
    }

    void SetQuoteChar(char c)
    {
        mQuoteChar = c;
    }

    bool HasData( void ) const
    {
        return ( mData != 0 );
    }

  void setLineFeed(char c)
  {
    mHard[(unsigned char)c] = ST_LINE_FEED;
  }

  bool isLineFeed(char c)
  {
    if ( mHard[(unsigned char)c] == ST_LINE_FEED ) return true;
    return false;
  }

  void setIgnoreCComments(bool state)
  {
      mIgnoreCComments = state;
  }

private:
    int32_t internalProcessLine(int32_t lineno, char *line, InPlaceParserInterface *callback);

    inline char * AddHard(int32_t &argc,const char **argv,SeparatorType *types,char *foo);
    inline bool   IsHard(char c);
    inline char * SkipSpaces(char *foo);
    inline bool   IsWhiteSpace(char c);
    inline bool   IsNonSeparator(char c); // non separator,neither hard nor soft

    bool            mInsideCommentBlock;
    bool            mIgnoreCComments; // ignore C style comments!
    bool            mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
    char            *mData;  // ascii data to parse.
    int32_t         mLen;   // length of data
    SeparatorType   mHard[256];
    char            mHardString[256*2];
    char            mQuoteChar;
    SeparatorType   mTypes[MAXARGS];
    const char      *mArgv[MAXARGS];
};

}

#endif
