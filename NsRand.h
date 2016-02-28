#ifndef NS_RAND_H
#define NS_RAND_H

/*************************************************************************************//**
*
* @brief Helper class to produce deterministic random numbers of pull random numbers from a pool
*
*****************************************************************************************/


#include <stdint.h>
#include "NsUserAllocated.h"

namespace blockchainsim
{
        class Rand
        {
        public:

            Rand(int32_t seed = 0)
            {
                mCurrent = seed;
            };

            int32_t get(void)
            {
                return((mCurrent = mCurrent * 214013L + 2531011L) & 0x7fffffff);
            };

            // random number between 0.0 and 1.0
            float ranf(void)
            {
                int32_t v = get() & 0x7FFF;
                return (float)v*(1.0f / 32767.0f);
            };

            float ranf(float low, float high)
            {
                return (high - low)*ranf() + low;
            }

            void setSeed(int32_t seed)
            {
                mCurrent = seed;
            };

        private:
            int32_t mCurrent;
        };

        class RandPool
        {
        public:
            RandPool(int32_t size, int32_t seed)  // size of random number bool.
            {
                mRand.setSeed(seed);       // init random number generator.
                mData = (int32_t *)NV_ALLOC(sizeof(int32_t)*size, "RandPool"); // allocate memory for random number bool.
                mSize = size;
                mTop = mSize;
                for (int32_t i = 0; i < mSize; i++) mData[i] = i;
            }

            ~RandPool(void)
            {
                NV_FREE(mData);
            };

            // pull a number from the random number pool, will never return the
            // same number twice until the 'deck' (pool) has been exhausted.
            // Will set the shuffled flag to true if the deck/pool was exhausted
            // on this call.
            int32_t get(bool &shuffled)
            {
                if (mTop == 0) // deck exhausted, shuffle deck.
                {
                    shuffled = true;
                    mTop = mSize;
                }
                else
                    shuffled = false;
                int32_t entry = mRand.get() % mTop;
                mTop--;
                int32_t ret = mData[entry]; // swap top of pool with entry
                mData[entry] = mData[mTop];  // returned
                mData[mTop] = ret;
                return ret;
            };

            float ranf(void) { return mRand.ranf(); };

        private:
            Rand mRand;  // random number generator.
            int32_t  *mData;  // random number bool.
            int32_t   mSize;  // size of random number pool.
            int32_t   mTop;   // current top of the random number pool.
        };
}


#endif
