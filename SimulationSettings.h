#ifndef SIMULATION_SETTINGS_H
#define SIMULATION_SETTINGS_H

namespace blockchainsim
{

    class Gauss;

    class SimulationSettings
    {
    public:
        static SimulationSettings *create(const char *fname);

        // Returns the time between blocks (in seconds)
        virtual const Gauss& getBlockTime(void) const = 0;

        // returns the maximum block size (in bytes)
        virtual const Gauss& getMaxBlockSize(void) const = 0;

        // return the transaction size
        virtual const Gauss& getTransactionSize(void) const = 0;

        virtual const Gauss& getBlockCount(void) const = 0;

        virtual void release(void) = 0;
    protected:
        virtual ~SimulationSettings(void)
        {
        }
    };

}

#endif
