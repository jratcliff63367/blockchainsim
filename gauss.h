#ifndef GAUSS_H

#define GAUSS_H

// ** This class is a Gaussian Random Number Generator
// ** A Gaussian number is expressed as having a mean and standard
// ** deviation and generates random numbers which conform to the standard
// ** bell curve distribution.  Additionally this class applies an optional
// ** minimum and maximum clamping range for all numbers generated.
// ** The Guassian class inherits the deterministic random number class.
// ** Therefore all gaussian sequences can be deterministic given the same
// ** initial random number seed.
// **
// ** The ASCII format for a gaussian number is as follows:
// **
// ** mean:std<min:max>
// **
// ** Examples of valid gaussian numbers.
// **
// **  30          (Means 30 with a standard deviation of 0)
// **  30:5        (Means 30 with a standard deviation of +/- 5)
// **  30:5<10>    (Means 30, stdev +/-5 minimum value of 10
// **  30:5<10:40> (Means 30, stdev +/-5, min value 10 max value 40)
// ** !30:5        (Means 30, +/-5 using a straight linear function!

#include "NvPreprocessor.h"
#include "NsRand.h"
#include <string>

namespace blockchainsim
{

typedef std::string String;

enum GaussFlag
{
  GF_MEAN    = (1<<0),
  GF_STDEV   = (1<<1),
  GF_MIN     = (1<<2),
  GF_MAX     = (1<<3),
  GF_LINEAR  = (1<<4),
  GF_SECOND  = (1<<5)
};

// Implementation of gaussian numbers.
class Gauss : public Rand
{
public:
  Gauss(void);
  Gauss(const char *value);
  Gauss(float f); // has no random deviation

  void SetGaussFlag(GaussFlag f)
  {
    mFlags|=f;
  }

  void ClearGaussFlag(GaussFlag f)
  {
    mFlags&=~f;
  }

  bool HasGaussFlag(GaussFlag f) const
  {
    if ( mFlags &f ) return true;
    return false;
  }


  float Get(Rand &r); // generate *deterministic* gaussian number using a specific random number seed.
  float Get(void);    // generate a random number
  float GetCurrent(void) const; // last generated value.

  void GetString(String &str) const; // get string representation

  const char * Set(const char *arg); // set from asciiz string.

  float RandGauss(Rand *r); // construct and return gaussian number.

  // convert string to gaussian number.  Return code
  // indicates number of arguments found.
  int32_t strtogmd(const char* spec,
               char** end,
               float& mean,
               float& deviation,
               float& min,
               float& max,
               bool &linear ) const;

  float GetMean(void)              const { return mMean; };
  float GetStandardDeviation(void) const { return mStandardDeviation; };
  float GetMin(void)               const { return mMin; };
  float GetMax(void)               const { return mMax; };

  void srand(void);

  void Reset(void);

  void SetMin(float m)
  {
    mMin = m;
    SetGaussFlag(GF_MIN);
  }

  void SetMax(float m)
  {
    mMax = m;
    SetGaussFlag(GF_MAX);
  }

  void SetMean(float m)
  {
    mMean = m;
    if ( mMean == 0.0f )
    {
      ClearGaussFlag(GF_MEAN);
    }
    else
    {
      SetGaussFlag(GF_MAX);
    }
  }

  void SetStandardDeviation(float sd)
  {
    mStandardDeviation = sd;
    if ( sd == 0.0 )
      ClearGaussFlag(GF_STDEV);
    else
      SetGaussFlag(GF_STDEV);
  }

  void applyScale(float scale)
  {
      mMean *= scale;
      mStandardDeviation *= scale;
      mMin *= scale;
      mMax *= scale;
  }

private:
  int32_t   mFlags;
  float mMean;               // gaussian number has mean and
  float mStandardDeviation;  // standard deviation, also
  float mMin;                // min/max clamping values
  float mMax;

  float mCurrent;           // last got value.
  float mGauss1;            // 1st gaussian
  float mGauss2;            // 2nd gaussian
};

float ranfloat(void);

};

#endif
