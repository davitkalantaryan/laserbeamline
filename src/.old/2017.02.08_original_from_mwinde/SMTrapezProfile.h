// SMTrapezProfile.h
//

// M. Winde, DESY - Zeuthen, 10-Nov-2000

#ifndef __INCSMTrapezProfileh
#define __INCSMTrapezProfileh

#include "SMProfile.h"

class SMTrapezProfile : public SMProfile
{
public:
	// --- creator
	SMTrapezProfile(int initFrq, int flyFrq, int endDispl = 0);

private:
	int initialFrq;			// start/stop frequency [pps == pulses per second]
	int flightFrq;			// maximal frequency for movement [pps]
	int endDisplacement;	// the displacement == nb of pulses to go with initialFrq
							// before the target position is reached	
};

#endif	//__INCSMTrapezProfileh

