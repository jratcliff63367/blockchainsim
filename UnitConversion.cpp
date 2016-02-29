#include "UnitConversion.h"
#include "NvPreprocessor.h"
#include "NsString.h"
#include "gauss.h"

namespace blockchainsim
{

    enum TimeUnit
    {
        TU_SECOND,
        TU_MINUTE,
        TU_HOUR,
        TU_DAY,
        TU_MONTH,
        TU_YEAR
    };

    TimeUnit getTimeUnit(const char *t)
    {
        TimeUnit ret = TU_SECOND;

        if (strcmp(t, "s") == 0 || strcmp(t, "sec") == 0 || strcmp(t, "second") == 0)
        {
            ret = TU_SECOND;
        }
        else if (strcmp(t, "m") == 0 || strcmp(t, "minute") == 0 || strcmp(t, "minutes") == 0 )
        {
            ret = TU_MINUTE;
        }
        else if (strcmp(t, "h") == 0 || strcmp(t, "hour") == 0 || strcmp(t, "hours") == 0)
        {
            ret = TU_HOUR;
        }
        else if (strcmp(t, "d") == 0 || strcmp(t, "day") == 0 || strcmp(t, "days") == 0)
        {
            ret = TU_DAY;
        }
        else if (strcmp(t, "m") == 0 || strcmp(t, "month") == 0 || strcmp(t, "months") == 0)
        {
            ret = TU_MONTH;
        }
        else if (strcmp(t, "y") == 0 || strcmp(t, "year") == 0 || strcmp(t, "years") == 0)
        {
            ret = TU_YEAR;
        }

        return ret;
    }

    enum SizeUnit
    {
        SU_BYTE,
        SU_KB,
        SU_MB,
        SU_GB
    };

    SizeUnit getSizeUnit(const char *t)
    {
        SizeUnit ret = SU_BYTE;

        if (strcmp(t, "b") == 0 || strcmp(t, "byte") == 0 || strcmp(t, "bytes") == 0)
        {
            ret = SU_BYTE;
        }
        else if (strcmp(t, "k") == 0 || strcmp(t, "kn") == 0 || strcmp(t, "kilobyte") == 0)
        {
            ret = SU_KB;
        }
        else if (strcmp(t, "m") == 0 || strcmp(t, "mb") == 0 || strcmp(t, "megabyte") == 0)
        {
            ret = SU_MB;
        }
        else if (strcmp(t, "g") == 0 || strcmp(t, "gb") == 0 || strcmp(t, "gigabyte") == 0)
        {
            ret = SU_GB;
        }

        return ret;
    }


    static bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    void splitNumber(const char *n, const char *&prefix, const char *&postfix)
    {
        prefix = nullptr;
        postfix = nullptr;

        static char gPrefix[512];
        static char gPostfix[512];
        gPrefix[0] = 0;
        gPostfix[0] = 0;

        const char *source = n;
        if (*source)
        {
            char *dest = gPrefix;
            while (*source)
            {
                char c = *source;
                if (isDigit(c) || c == '.' || c == '-' || c == '+' || c == '!' || c == ':' || c == '<' || c == '>')
                {
                    *dest++ = c;
                    source++;
                }
                else if (c == ',') // we just ignore commas and essentially treat them as comments
                {
                    source++;
                }
                else
                {
                    break;
                }
            }
            *dest = 0;
            if (gPrefix[0])
            {
                prefix = gPrefix;
            }
            if (*source) // if there is more stuff after..
            {
                dest = gPostfix;
                while (*source)
                {
                    *dest++ = *source++;
                }
                *dest = 0;
                postfix = gPostfix;
            }
        }
        strlwr(gPrefix);
        strlwr(gPostfix);
    }

// convert this input string into a Gaussian representation of time as seconds
    bool getGaussTime(const char *str,Gauss &g)
    {
        bool ret = false;
        const char *prefix;
        const char *postfix;

        splitNumber(str, prefix, postfix);
        if (prefix)
        {
            ret = true;
            g.Set(prefix);
            if (postfix)
            {
                TimeUnit t = getTimeUnit(postfix);
                float scale = 1; //default
                switch (t)
                {
                    case TU_SECOND:
                        scale = 1;
                        break;
                    case TU_MINUTE:
                        scale = 60;
                        break;
                    case TU_HOUR:
                        scale = 60 * 60;
                        break;
                    case TU_DAY:
                        scale = 60 * 60 * 24;
                        break;
                    case TU_MONTH:
                        scale = (60 * 60 * 24) * 30;
                        break;
                    case TU_YEAR:
                        scale = (60 * 60 * 24) * 365;
                        break;
                }
                g.applyScale(scale);
            }
        }
        return ret;
    }

    // convert input string into a size, whatever the notation, always represented as 'bytes' or indidual units
    bool getGaussSize(const char *str, Gauss &g)
    {
        bool ret = false;
        const char *prefix;
        const char *postfix;

        splitNumber(str, prefix, postfix);
        if (prefix)
        {
            ret = true;
            g.Set(prefix);
            if (postfix)
            {
                SizeUnit t = getSizeUnit(postfix);
                float scale = 1; //default
                switch (t)
                {
                case SU_BYTE:
                    scale = 1;
                    break;
                case SU_KB:
                    scale = 1024;
                    break;
                case SU_MB:
                    scale = 1024 * 1024;
                    break;
                case SU_GB:
                    scale = 1024 * 1024 * 1024;
                    break;
                }
                g.applyScale(scale);
            }
        }
        return ret;
    }


}
