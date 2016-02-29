#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

namespace blockchainsim
{


class SimulationSettings;

class BlockChain
{
public:
	static BlockChain *create(const SimulationSettings &s);
    // pump loop of the blockchain simulation; returns true if the simulation is still running; false if it is complete
    virtual bool pump(void) = 0;
	virtual void release(void) = 0;
protected:
	virtual ~BlockChain(void)
	{
	}
};

} // end of namespace

#endif
