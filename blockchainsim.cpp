#include <stdio.h>
#include <stdlib.h>

#include "SimulationSettings.h"
#include "BlockChain.h"

using namespace blockchainsim;

int main(int argc,const char **argv)
{
	if ( argc == 1 )
	{
		printf("Usage: blockchainsim <simulation_file.ini>\n");
	}
	else
	{
		const char *simFile = argv[1];
        SimulationSettings *ss = SimulationSettings::create(simFile);
        if (ss)
        {
            BlockChain *b = BlockChain::create(*ss);
            if (b)
            {
                bool running = true;
                while (running)
                {
                    running = b->pump();
                }
                b->release();
            }
            ss->release();
        }
	}
	return 0;
}
