#include "SimulationSettings.h"
#include "NsKeyValueIni.h"
#include "NsUserAllocated.h"
#include "logging.h"
#include "gauss.h"
#include "UnitConversion.h"
#include "NvAssert.h"
#include <stdio.h>

namespace blockchainsim
{

    class SimulationSettingsImpl : public SimulationSettings, public KeyValueResource, public UserAllocated
    {
    public:
        SimulationSettingsImpl(const char *simName)
        {
            mINI = nullptr;
            uint32_t len;
            const void *mem = getIniResource(simName, len);
            if (mem)
            {
                mINI = KeyValueIni::create(simName, mem, len, nullptr, this);
                if (mINI)
                {
                    initProperties();
                    if (mError)
                    {
                        logMessage("Failed to initialize the INI settings for '%s'\n", simName);
                    }
                    else
                    {
                        logMessage("Successfully initialized the INI settings from '%s'\n", simName);
                    }
                }
                else
                {
                    logMessage("Failed to create INI file '%s'\n", simName);
                }
                releaseIniResource(mem);
            }
            else
            {
                logMessage("Failed to open INI file '%s'\n", simName);
            }
        }

        virtual ~SimulationSettingsImpl(void)
        {

        }

        void initProperties(void)
        {
            mError = false;
            getTime("BLOCKCHAIN", "BLOCK_TIME", mBlockTime);
            getSize("BLOCKCHAIN", "MAX_BLOCK_SIZE", mMaxBlockSize);
            getSize("BLOCKCHAIN", "TRANSACTION_SIZE", mTransactionSize);
            getSize("BLOCKCHAIN", "BLOCK_COUNT", mBlockCount);
        }

        bool getTime(const char *section, const char *key,Gauss &g)
        {
            bool ret = false;

            const char *value = mINI->getKeyValue(section,key);
            if (value)
            {
                if (getGaussTime(value,g))
                {
                    ret = true;
                }
                else
                {
                    logMessage("Failed to convert '%s' into a valid Gaussian number.\n", value);
                    mError = true;
                }
            }
            else
            {
                logMessage("ERROR: Failed to locate value for '%s' from section '%s'\n", key, section);
                mError = true;
            }

            return ret;
        }

        bool getSize(const char *section, const char *key, Gauss &g)
        {
            bool ret = false;

            const char *value = mINI->getKeyValue(section, key);
            if (value)
            {
                if (getGaussSize(value, g))
                {
                    ret = true;
                }
                else
                {
                    logMessage("Failed to convert '%s' into a valid Gaussian number.\n", value);
                    mError = true;
                }
            }
            else
            {
                logMessage("ERROR: Failed to locate value for '%s' from section '%s'\n", key, section);
                mError = true;
            }

            return ret;
        }



        virtual const void*   getIniResource(const char *resourceName, uint32_t &resourceLen) final
        {
            void *ret = nullptr;
            resourceLen = 0;

            FILE *fph = fopen(resourceName, "rb");
            if (fph)
            {
                fseek(fph, 0L, SEEK_END);
                resourceLen = uint32_t(ftell(fph));
                if (resourceLen)
                {
                    fseek(fph, 0L, SEEK_SET);
                    ret = NV_ALLOC(resourceLen, "INI Resource");
                    fread(ret, resourceLen, 1, fph);
                }
                fclose(fph);
            }
            else
            {
                logMessage("Failed to open resource file '%s'\n", resourceName);
            }

            return ret;
        }

        virtual void          releaseIniResource(const void *mem) final
        {
            NV_FREE((void *)mem);
        }

        bool isError(void) const
        {
            return mError;
        }

        virtual void release(void) final
        {
            delete this;
        }

        // Returns the time between blocks (in seconds)
        virtual const Gauss& getBlockTime(void) const final
        {
            return mBlockTime;
        }

        // returns the maximum block size (in bytes)
        virtual const Gauss& getMaxBlockSize(void) const
        {
            return mMaxBlockSize;
        }

        // return the transaction size
        virtual const Gauss& getTransactionSize(void) const
        {
            return mTransactionSize;
        }

        virtual const Gauss& getBlockCount(void) const
        {
            return mBlockCount;
        }


    protected:
        bool             mError;
        KeyValueIni     *mINI;
        Gauss           mBlockTime;
        Gauss           mMaxBlockSize;
        Gauss           mTransactionSize;
        Gauss           mBlockCount;
    };

    SimulationSettings *SimulationSettings::create(const char *fname)
    {
        SimulationSettingsImpl *ss = NV_NEW(SimulationSettingsImpl)(fname);
        if (ss->isError())
        {
            delete ss;
            ss = nullptr;
        }
        return static_cast<SimulationSettings *>(ss);
    }


} // end of namespace