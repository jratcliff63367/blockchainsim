#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stdint.h>

namespace blockchainsim
{

    class Transaction;

    class MemPool
    {
    public:
        static MemPool *create(void);

        // opportunity to drop transactions from the mempool if they are too old
        virtual void pump(uint32_t timeStamp) = 0;

        // add a transaction to the mempool
        virtual void addTransaction(const Transaction &t) = 0;

        // peek the next transaction with the highest fee; but don't remove it yet.
        virtual bool peekTransaction(Transaction &t) = 0;

        // get the next transaction with the highest fee
        virtual bool getTransaction(Transaction &t) = 0;

        // report the size of the current mempool in bytes
        virtual uint32_t	getMemPoolSize(void) const = 0;

        // report the number of pending transactions in the mempool
        virtual uint32_t	getMemPoolCount(void) const = 0;

        virtual double getMemPoolTotalValue(void) const = 0;
        virtual double getMemPoolTotalFees(void) const = 0;


        virtual void release(void) = 0;
    protected:
        virtual ~MemPool(void)
        {
        }
    };

} // end of blockchainsim namespace

#endif
