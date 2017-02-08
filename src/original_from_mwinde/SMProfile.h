// SMProfile.h
//

// M. Winde, DESY - Zeuthen, 10-Nov-2000

#ifndef __INCSMProfileh
#define __INCSMProfileh

#include <StepperMotor.h>

class SMProfile	
	// class that defines the profile (ramp, velocity trajectory) for the motion of a 
	// stepper motor;
	// abstract class
{
public:
	SMProfile(){}
private:
};

typedef SMProfile * TypeSMProfilePtr;

class StepperMotorWithProfile : public StepperMotor
{
	//friend class SMProfile;
	
public:
	// --- constructor
	StepperMotorWithProfile(int maxNbOfRamps = 0);
		// any inherited class should call this creator with the number of ramps
		// its hardware is able to handle
	
	// --- destructor
	virtual ~StepperMotorWithProfile();
	
	// --- interface
	
	// select a ramp by SMProfile object,
	smStatus SelectRamp(SMProfile* theRamp);
	
	// allocate a ramp profile to a ramp number,
	// if possible store it as ramp "rampNb" into the motor controller hardware
	smStatus ProfileToRamp(SMProfile* theRamp, int rampNb);

	// select a ramp by ramp number
	smStatus SelectRamp(int rampNb);

protected:
	// write the parameters of the profile to the motor controler hardware
	virtual smStatus SelectRampHard(SMProfile* theRamp);

	// write the parameters of the profile to the motor controler hardware as a ramp;
	virtual smStatus ProfileToRampHard(SMProfile* theRamp, int rampNb);

	// select a ramp in the motor controller hardware
	virtual smStatus SelectRampHard(int rampNb);

private:
	SMProfile * selectedRamp;	// the ramp actually selected;
	int selectedRampNb;
	
	const int maxNumberedRamps;
        TypeSMProfilePtr * rampList;

};
#endif	//__INCSMProfileh

