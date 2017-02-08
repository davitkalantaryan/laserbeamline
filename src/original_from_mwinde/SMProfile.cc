// SMProfile.cpp
//

// M. Winde, DESY - Zeuthen, 10-Nov-2000

#include "SMProfile.h"

// --- StepperMotorWithProfile ---

StepperMotorWithProfile::StepperMotorWithProfile(int maxNbOfRamps) :
	selectedRamp(NULL),
	selectedRampNb(-1),
	maxNumberedRamps(maxNbOfRamps),
	rampList(NULL)
{
	if(maxNumberedRamps > 0) {
                //rampList = new (SMProfile**)[maxNumberedRamps];
                //rampList = new SMProfile[maxNumberedRamps];
                rampList = new TypeSMProfilePtr[maxNumberedRamps];
		for(int i=0; i<maxNbOfRamps; i++) rampList[i] = NULL;
	}
}

StepperMotorWithProfile::~StepperMotorWithProfile()
{
	delete rampList;
}

smStatus StepperMotorWithProfile::SelectRamp(SMProfile* theRamp) {
	if(theRamp == NULL)  return ErrIllParam;
	smStatus status = 0;
	if (theRamp != selectedRamp) {
		status = SelectRampHard(theRamp);
		selectedRamp = (status==0) ? theRamp:NULL;
	}
	selectedRampNb = -1; // we do not know whether the profile is connected to a ramp nb.
	return status;
}

smStatus StepperMotorWithProfile::SelectRamp(int rampNb) {
	if((rampNb<0) || (rampNb>=maxNumberedRamps)) return ErrIllParam;
	smStatus status = 0;
	if (rampNb != selectedRampNb) {
		status = SelectRampHard(rampNb);
		if (status==0) {
			selectedRampNb = rampNb;
			selectedRamp = rampList[rampNb];	// this is NULL if the ramp was not allocated to a profile
		} else {
			selectedRampNb = -1;
			selectedRamp = NULL;
		}
		if ((status == ErrIllParam) && (rampList[rampNb] != NULL) ) {
			// SelectRampHard(rampNb) returned "hardware not able to select by ramp number",
			// so select by profile object
			status = SelectRampHard(rampList[rampNb]);
			if (status==0) selectedRamp = rampList[rampNb];
		} 
	}
	
	return status;
}

smStatus StepperMotorWithProfile::ProfileToRamp(SMProfile* theRamp, int rampNb) {
	// allocates a ramp profile to a ramp number
	if((rampNb<0) || (rampNb>=maxNumberedRamps)) return ErrIllParam;
	smStatus status = 0;
	rampList[rampNb] =theRamp;
	if(theRamp != NULL)
		status = ProfileToRampHard(theRamp, rampNb);
	return status;
}

smStatus StepperMotorWithProfile::SelectRampHard(SMProfile* theRamp) {
	// write the parameters of the profile to the motor controler hardware;
	// default version does nothing
	return 0;
}

smStatus StepperMotorWithProfile::SelectRampHard(int /*rampNb*/) {
	// select a ramp in the motor controller hardware;
	// default version does nothing but returns an error to advice SelectRamp(int)
	// to use the profile object instead
	return ErrIllParam;
}

smStatus StepperMotorWithProfile::ProfileToRampHard(SMProfile* /*theRamp*/, int /*rampNb*/) {
	// write the parameters of the profile to the motor controler hardware as a ramp;
	// default version does nothing
	return 0;
}

