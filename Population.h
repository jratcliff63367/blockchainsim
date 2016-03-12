#ifndef POPULATION_H
#define POPULATION_H

#include <stdint.h>

namespace blockchainsim
{

class MemPool;

class Population
{
public:
	static Population *create(void);


	// process once per logical second
	virtual bool pump(uint32_t timeStamp,MemPool *mp) = 0;


	virtual void release(void) = 0;
protected:
	virtual ~Population(void)
	{
	}
};

}

#endif
