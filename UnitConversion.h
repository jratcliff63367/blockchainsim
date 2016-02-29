#ifndef UNIT_CONVERSION_H
#define UNIT_CONVERSION_H

// Helper routines to convert ASCII strings with unit notation into gaussian numbers

namespace blockchainsim
{

class Gauss;

// convert this input string into a gaussian representation of time as seconds
bool getGaussTime(const char *str,Gauss &g);

// convert input string into a size, whatever the notation, always represented as 'bytes' or indidual units
bool getGaussSize(const char *str,Gauss &g);

} // end of blockchainsim namespace


#endif
