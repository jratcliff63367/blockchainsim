#include <stdio.h>
#include <stdlib.h>

int main(int argc,const char **argv)
{
	if ( argc == 1 )
	{
		printf("Usage: blockchainsim <simulation_file.ini>\n");
	}
	else
	{
		const char *simFile = argv[1];
		printf("Running simulation: %s\n", simFile);
	}
	return 0;
}
