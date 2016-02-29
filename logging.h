#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

namespace blockchainsim
{

    void logMessage(const char *fmt, ...);
    const char *getDateString(uint32_t _t);
    const char *getTimeString(uint32_t timeStamp);

}

#endif
