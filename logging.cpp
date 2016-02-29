#include "logging.h"
#include "NsString.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>


#ifdef _MSC_VER
#include <conio.h>
#pragma warning(disable:4996)
#endif

namespace blockchainsim
{

    // This is a helper method to handle logging the output from scanning the blockchain
    void logMessage(const char *fmt, ...)
    {
        static FILE		*gLogFile = NULL;
        char wbuff[2048];
        va_list arg;
        va_start(arg, fmt);
        vsprintf(wbuff, fmt, arg);
        va_end(arg);
        printf("%s", wbuff);
        if (gLogFile == NULL)
        {
            gLogFile = fopen("blockchainsim.txt", "wb");
        }
        if (gLogFile)
        {
            fprintf(gLogFile, "%s", wbuff);
            fflush(gLogFile);
        }
    }


    const char *getDateString(uint32_t _t)
    {
        time_t t(_t);
        static char scratch[1024];
        struct tm *gtm = gmtime(&t);
        //	strftime(scratch, 1024, "%m, %d, %Y", gtm);
        sprintf(scratch, "%4d-%02d-%02d", gtm->tm_year + 1900, gtm->tm_mon + 1, gtm->tm_mday);
        return scratch;
    }

    const char *getTimeString(uint32_t timeStamp)
    {
        if (timeStamp == 0)
        {
            return "NEVER";
        }
        static char scratch[1024];
        time_t t(timeStamp);
        struct tm *gtm = gmtime(&t);
        strftime(scratch, 1024, "%m/%d/%Y %H:%M:%S", gtm);
        return scratch;
    }



} // end of namespace