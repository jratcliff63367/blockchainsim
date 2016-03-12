#include "Population.h"
#include "Transaction.h"
#include "MemPool.h"
#include "NsUserAllocated.h"
#include "gauss.h"
#include "NvAssert.h"
#include <time.h>

#pragma warning(disable:4100)

namespace blockchainsim
{

class MemPool;

class PopulationImpl : public Population, public UserAllocated
{
public:
    PopulationImpl(void)
    {
        mTransactionPendingCount = 0;
        mTransactionsPerSecond.Set("20:10<0:60>");
        mAverageFee.Set("0.04:0.011<0:0.25>");
        mAverageValue.Set("8:10<0.01:1000>");
        mAverageSize.Set("550:150<250:1000>");
    }

    virtual ~PopulationImpl(void)
    {

    }

	// process once per logical second
    virtual bool pump(uint32_t timeStamp, MemPool *mp)
    {
        bool ret = true;

        time_t t(timeStamp);
        struct tm *gtm = gmtime(&t);
        if (gtm->tm_hour >= 8 && gtm->tm_hour <= 12)
        {
            mTransactionPendingCount += mTransactionsPerSecond.Get();
            if (mTransactionPendingCount > 1.0f)
            {
                uint32_t count = uint32_t(mTransactionPendingCount);
                mTransactionPendingCount -= float(count);
                for (uint32_t i = 0; i < count; i++)
                {
                    generateTransaction(mp, timeStamp);
                }
            }
        }

        return ret;
    }

    void generateTransaction(MemPool *p,uint32_t timeStamp)
    {
        Transaction t;
        t.mFee              = mAverageFee.Get();
        t.mValue            = mAverageValue.Get();
        t.mTransactionSize  = uint32_t(mAverageSize.Get());
        t.mTimestamp        = timeStamp;
        NV_ASSERT(t.mFee >= 0);
        NV_ASSERT(t.mValue >= 0);
        p->addTransaction(t);
    }


    virtual void release(void)
    {
        delete this;
    }


    float   mTransactionPendingCount;
    Gauss   mTransactionsPerSecond;
    Gauss   mAverageFee;
    Gauss   mAverageValue;
    Gauss   mAverageSize;
};

Population *Population::create(void)
{
    PopulationImpl *p = NV_NEW(PopulationImpl);
    return static_cast<Population *>(p);
}




}

