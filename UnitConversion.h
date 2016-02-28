#ifndef UNIT_CONVERSION_H
#define UNIT_CONVERSION_H

// Helper routines to convert ASCII strings with unit notation into gaussian numbers
#include "gauss.h"

namespace blockchainsim
{

// convert this input string into a gaussian representation of time as seconds
Gauss getTime(const char *str);

// convert input string into a size, whatever the notation, always represented as 'bytes' or indidual units
Gauss getSize(const char *str);

} // end of blockchainsim namespace


#endif
