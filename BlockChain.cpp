#include "BlockChain.h"
#include "gauss.h"
#include "SimulationSettings.h"
#include "NsUserAllocated.h"
#include "logging.h"
#include "NsStringUtils.h"
#include "NsString.h"
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

            time_t t;
            time(&t);
            mStartTime = uint32_t(t);
            mBlockCount = 360;
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

        }

        virtual bool pump(void) final
        {
            bool ret = false;

            // Simulate one second of time passing
            if (mSecondsRemaining)
            {
                mSimulationTime++;
                mSecondsRemaining--;
                ret = true;
            }
            else
            {
                if (mBlockCount)    // if we are still processing blocks....
                {
                    mBlocks.push_back(mCurrentBlock);
                    uint32_t blockNumber = uint32_t(mBlocks.size());
                    float dtime = float(mBlockGenerationTime) / 60.0f;
                    char temp[512];
                    stringFormat(temp, "%0.2f", dtime);
                    logMessage("Mined block %6s took %5s minutes on %s\n", formatNumber(blockNumber),temp,getTimeString(mSimulationTime));
                    mCurrentBlock.mTimeStamp = mSimulationTime;
                    mBlockCount--;
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

        void getNextBlockTime(void)
        {
            mBlockGenerationTime = mSecondsRemaining = uint32_t(mBlockTime.Get()); // how many seconds until the next block is discovered!
            mCurrentBlock.init();
        }

        BlockInfo                   mCurrentBlock;
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
    };

    BlockChain * BlockChain::create(const SimulationSettings &s)
    {
        BlockChainImpl *b = NV_NEW(BlockChainImpl)(s);
        return static_cast<BlockChain *>(b);
    }
}
