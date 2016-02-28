#ifndef SIMULATION_SETTINGS_H
#define SIMULATION_SETTINGS_H

class SimulationSettings
{
public:
	static SimulationSettings *create(const char *fname);
	virtual void release(void) = 0;
protected:
	virtual ~SimulationSettings(void)
	{
	}
};

#endif
