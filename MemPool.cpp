#include "MemPool.h"
#include "Transaction.h"
#include "NsUserAllocated.h"
#include "NvAssert.h"
#include <set>

#pragma warning(disable:4100)

namespace blockchainsim
{

    typedef std::set< Transaction > TransactionSet;

    class MemPoolImpl : public MemPool, public UserAllocated
    {
    public:
        MemPoolImpl(void)
        {
            mCount = 0;
            mId = 0;
            mMemPoolSize = 0;
            mTotalValue = 0;
            mTotalFees = 0;
        }

        ~MemPoolImpl(void)
        {


        }

        // opportunity to drop transactions from the mempool if they are too old
        virtual void pump(uint32_t timeStamp)
        {

        }


        // add a transaction to the mempool
        virtual void addTransaction(const Transaction &_t)
        {
            Transaction t = _t;
            t.mID = ++mId;
            mMemPoolSize += t.mTransactionSize;
            mTotalFees += t.mFee;
            mTotalValue += t.mValue;
            mCount++;
            mTransactions.insert(t);
            NV_ASSERT(mCount == mTransactions.size());
        }

        // peek the next transaction with the highest fee; but don't remove it yet.
        virtual bool peekTransaction(Transaction &t)
        {
            bool ret = false;

            if (!mTransactions.empty())
            {
                TransactionSet::iterator i = mTransactions.begin();
                t = (*i);
                ret = true;
            }
            return ret;
        }


        // get the next transaction with the highest fee
        virtual bool getTransaction(Transaction &t)
        {
            bool ret = false;

            if (!mTransactions.empty())
            {
                TransactionSet::iterator i = mTransactions.begin();
                t = (*i);

                mCount--;
                mTransactions.erase(i);
                NV_ASSERT(mCount == mTransactions.size());

                mTotalFees -= t.mFee;
                mTotalValue -= t.mValue;
                mMemPoolSize -= t.mTransactionSize;
                NV_ASSERT(mTotalValue >= 0.0f);
                NV_ASSERT(mTotalFees >= 0.0f);
                ret = true;
            }

            return ret;
        }

        // report the size of the current mempool in bytes
        virtual uint32_t	getMemPoolSize(void) const
        {
            return mMemPoolSize;
        }

        // report the number of pending transactions in the mempool
        virtual uint32_t	getMemPoolCount(void) const
        {
            return uint32_t(mTransactions.size());
        }

        virtual double getMemPoolTotalValue(void) const
        {
            return mTotalValue;
        }

        virtual double getMemPoolTotalFees(void) const
        {
            return mTotalFees;
        }

        virtual void release(void)
        {
            delete this;
        }
    protected:
        size_t          mCount;
        uint32_t        mId;
        uint32_t        mMemPoolSize;
        double           mTotalValue;
        double           mTotalFees;
        TransactionSet  mTransactions;
    };


    MemPool *MemPool::create(void)
    {
        MemPoolImpl *m = NV_NEW(MemPoolImpl);
        return static_cast<MemPool *>(m);
    }

} // end of blockchainsim namespace