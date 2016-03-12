#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>

namespace blockchainsim
{

    class Transaction
    {
    public:
        Transaction(void)
        {
            mID = 0;
            mValue = 0;
            mFee = 0;
            mTimestamp = 0;
            mTransactionSize = 0;
        }

        bool operator<(const Transaction &a) const
        {
            if (a.mFee != mFee )
            {
                return a.mFee < mFee;
            }
            return a.mID < mID;
        }

        bool operator==(const Transaction &a) const
        {
            return a.mFee == mFee && a.mID == mID;
        }

        uint32_t	mID;		// transaction ID
        double		mValue;		// The value of this transaction.
        double		mFee;		// the fee of the transaction
        uint32_t	mTimestamp; // the timestamp that the transaction was issued.
        uint32_t	mTransactionSize;	// The size of this transaction in bytes.
    };

}

#endif
