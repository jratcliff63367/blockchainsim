#include "BlockChain.h"
#include "gauss.h"
#include "SimulationSettings.h"
#include "NsUserAllocated.h"
#include "logging.h"
#include "NsStringUtils.h"
#include "NsString.h"
#include "MemPool.h"
#include "Population.h"
#include "Transaction.h"
#include <time.h>
#include <vector>

namespace blockchainsim
{

    class BlockInfo
    {
    public:
        BlockInfo(void)
        {
            init();
        }
        void init(void)
        {
            mTimeStamp = 0;
            mTransactionCount = 0;
            mBlockSize = 0;
        }
        uint32_t    mTimeStamp;             // time stamp for this block
        uint32_t    mTransactionCount;      // how many transactions are in this block
        uint32_t    mBlockSize;             // size of the block
    };

    typedef std::vector< BlockInfo > BlockInfoVector;

    class BlockChainImpl : public BlockChain, public UserAllocated
    {
    public:
        BlockChainImpl(const SimulationSettings &s) : mSimulationSettings(s)
        {
            mBlockFees = 0;
            mBlockValue = 0;
            mBlockChainReport = fopen("BlockChain.csv", "wb");
            if (mBlockChainReport)
            {
                fprintf(mBlockChainReport, "Time,BlockTime,BlockSize,TPS,TransactionCount,BlockValue,BlockFees,MemPoolCount,MemPoolSize,MemPoolFees,MemPoolValue\r\n");
                fflush(mBlockChainReport);
            }
            mMemPool = MemPool::create();
            mPopulation = Population::create();
            time_t t;
            time(&t);
            mStartTime = uint32_t(t);
            mBlockCount = 10000;
            mSimulationTime = mStartTime;
            mBlockTime = mSimulationSettings.getBlockTime();
            Gauss g = mSimulationSettings.getMaxBlockSize();
            mMaxBlockSize = uint32_t(g.Get());
            g = mSimulationSettings.getBlockCount();
            mBlockCount = uint32_t(g.Get());
            mTransactionSize = mSimulationSettings.getTransactionSize();
            getNextBlockTime();
        }

        virtual ~BlockChainImpl(void)
        {
            if (mBlockChainReport)
            {
                fclose(mBlockChainReport);
            }
            if (mMemPool)
            {
                mMemPool->release();
            }
            if (mPopulation)
            {
                mPopulation->release();
            }
        }

        virtual bool pump(void) final
        {
            bool ret = false;

            // Simulate one second of time passing
            if (mSecondsRemaining)
            {
                mSimulationTime++;
                mSecondsRemaining--;

                mPopulation->pump(mSimulationTime, mMemPool);

                ret = true;
            }
            else
            {
                if (mBlockCount)    // if we are still processing blocks....
                {
                    uint32_t transactionCount = 0;
                    uint32_t blockSize = processTransactions(transactionCount);
                    mBlocks.push_back(mCurrentBlock);
                    uint32_t blockNumber = uint32_t(mBlocks.size());
                    float dtime = float(mBlockGenerationTime) / 60.0f;
                    char temp[512];
                    stringFormat(temp, "%0.2f", dtime);
                    logMessage("Mined block %6s took %5s minutes on %s : Size: %s : TransactionCount: %d\n", formatNumber(blockNumber), temp, getTimeString(mSimulationTime), formatNumber(blockSize), formatNumber(transactionCount));
                    mCurrentBlock.mTimeStamp = mSimulationTime;
                    mBlockCount--;
                    if (mBlockChainReport)
                    {
                        fprintf(mBlockChainReport, "%s,", getTimeString(mSimulationTime));
                        fprintf(mBlockChainReport, "%f,", float(mBlockGenerationTime) / 60.0f);
                        fprintf(mBlockChainReport, "%d,", blockSize);

                        double tps = double(transactionCount) / double(mBlockGenerationTime);
                        fprintf(mBlockChainReport, "%f,", tps);
                        fprintf(mBlockChainReport, "%d,", transactionCount);
                        fprintf(mBlockChainReport, "%f,", mBlockValue);
                        fprintf(mBlockChainReport, "%f,", mBlockFees);
                        fprintf(mBlockChainReport, "%d,", mMemPool->getMemPoolCount());
                        fprintf(mBlockChainReport, "%d,", mMemPool->getMemPoolSize());
                        fprintf(mBlockChainReport, "%f,", mMemPool->getMemPoolTotalFees());
                        fprintf(mBlockChainReport, "%f,", mMemPool->getMemPoolTotalValue());
                        fprintf(mBlockChainReport, "\r\n");
                        fflush(mBlockChainReport);
                    }
                    mMemPool->pump(mSimulationTime);
                    getNextBlockTime();
                    ret = true;
                }
            }
            return ret;
        }

        virtual void release(void) final
        {
            delete this;
        }

        uint32_t processTransactions(uint32_t &transactionCount)
        {
            uint32_t blockSize = 0;
            transactionCount = 0;

            for (;;)
            {
                Transaction t;
                bool haveWork = mMemPool->peekTransaction(t);
                if (!haveWork)
                {
                    break;
                }
                if ((blockSize + t.mTransactionSize) > mMaxBlockSize)
                {
                    break;
                }
                mMemPool->getTransaction(t);
                transactionCount++;
                blockSize += t.mTransactionSize;
                mBlockFees += t.mFee;
                mBlockValue += t.mValue;
            }

            return blockSize;
        }

        void getNextBlockTime(void)
        {
            mBlockValue = 0;
            mBlockFees = 0;
            mBlockGenerationTime = mSecondsRemaining = uint32_t(mBlockTime.Get()); // how many seconds until the next block is discovered!
            mCurrentBlock.init();
        }

        BlockInfo                   mCurrentBlock;
        double                      mBlockValue;
        double                      mBlockFees;
        uint32_t                    mBlockCount;            // how many blocks to simulate mining
        uint32_t                    mStartTime;             // time we started running this simulation...
        uint32_t                    mSimulationTime;        // how many seconds we have been running the simulation
        uint32_t                    mBlockGenerationTime;
        uint32_t                    mSecondsRemaining;      // how many seconds left to run..
        const SimulationSettings  &mSimulationSettings;
        Gauss                       mBlockTime;
        uint32_t                    mMaxBlockSize;          // maximum block-size in bytes
        Gauss                       mTransactionSize;
        BlockInfoVector             mBlocks;                // simulated mined blocks
        Population                  *mPopulation;
        MemPool                     *mMemPool;
        FILE                        *mBlockChainReport;
    };

    BlockChain * BlockChain::create(const SimulationSettings &s)
    {
        BlockChainImpl *b = NV_NEW(BlockChainImpl)(s);
        return static_cast<BlockChain *>(b);
    }
}
