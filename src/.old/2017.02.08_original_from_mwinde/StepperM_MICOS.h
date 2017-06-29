// StepperM_MICOS.h

// Implements a StepperMotor controlled by a "Phytron Schrittmotor Steuerung" (MICOS).
// M. Winde, DESY - Zeuthen, 19-Sep-2000

// For details see also "Manual MA 1086-A006 D" at www.phytron.de

// Every motor has its own MICOS.
// Up to 16 MICOS (motors) my be driven through one RS485 interface.
// We call the class, that drives the all MICOS at an RS485 interface
// a "MICOS_Controller".
// The MICOS_Controller will be instantiated once for all MICOS connected through
// one RS485 bus. To allow access to the RS485 bus via various RS485-Interfaces,
// MICOS_Controller uses a class "RS485_Controller".
// Every RS485_Controller is implemented in a file of its own
// (as an example see IP_OctalPlus_485.h / .cpp).
//
// Thus the usual calling sequence to instantiate stepper motors is:
//
//	RS485_Controller * theRS485 = new IP500_Acromag(VME_Addr); 		 //create an RS485_Controller
//	MICOS_Controller * theMICOS = new MICOS_Controller(theRS485);			 //create the MICOS_Controller
//  StepperM_MICOS * stepperMotor0 = new StepperM_MICOS(theMICOS, 0);
//  StepperM_MICOS * stepperMotor1 = new StepperM_MICOS(theMICOS, 1) ;

#ifndef __INCStepperM_MICOSh
#define __INCStepperM_MICOSh

//#include "MICOS_Controller.h"
#include "SMTrapezProfile.h"
#include "RS485_Controller.h"
#include <stdlib.h>
#include <stdio.h>

class StepperM_MICOS : public StepperMotorWithProfile
{
	friend class MICOS_Controller;

public:
	// --- creator
        StepperM_MICOS(class MICOS_Controller* theSMController, int theMICOS_Addr);
	~StepperM_MICOS();

	//MICOS_Controller * obj2;

	// --- Init, Reset
	smStatus Reset(int partToReset, bool wait);
	smStatus Reset(int partToReset = 0){return Reset(partToReset, true);}
	smStatus Init();
	
	void SetPositionKnown(bool theActPosKnown, int theActualPosition);
	
	// --- move
	smStatus GoSteps(int steps = 1);
	smStatus GetError();
	//?? smStatus Calibrate();
	smStatus Status();
	smStatus GetPosition();
	smStatus GoToStep(int position);
	smStatus GoToInitiator(int paras);
	smStatus FreeRun(int paras);


	// --- Stop
	smStatus Stop(int rampNb = 0);

	// --- completion of move and Stop operations
	virtual smStatus Wait(double seconds, int inquiries = 10);
	smStatus Wait(double seconds, double &secondsLeft, int inquiries = 10);

	// --- get information
        const char * GetVersion();
	smStatus GetStatus();
	smStatus GetStatus(uint32_t &statusBits);
	smStatus GetSteps(int &position);
	
	smStatus GetLimitSwitches(int& lMinus, int& lPlus);
	
	smStatus CheckCable();

	// --- set position
	smStatus SetSteps(int position = 0);
	virtual void SetMaxSteps(int steps);

	// --- Set and Get Parameters
	smStatus SetParameter(const char * const para, const char * const value);
	smStatus GetParameter(const char * const para, char * value);
	//smStatus SetParameterLinear(bool value);
	//smStatus GetParameterLinear(bool & value);

	smStatus SetIPar(const char* command, int ipar, const char* additionalText = 0);

	// get the error string set
	virtual NumberedStrings * ErrorTexts() {return errorTexts;}

	// additional commands
	smStatus SetStopCurrent(int iStop);	// typical values: 500..3000
	smStatus GetStopCurrent(int& iStop);
	smStatus SetMoveCurrent(int iMove);	// typical values: 50..400
	smStatus GetMoveCurrent(int& iMove);
	smStatus SetCalVelocity(int steps_per_sec_to, int steps_per_sec_back);
	smStatus SetVelocity(int steps_per_sec);
	smStatus GetVelocity(int& steps_per_sec);
	smStatus SetAcceleration(int steps_per_sec2);
	smStatus GetAcceleration(int& steps_per_sec2);
	
	MICOS_Controller* ItsController() { return itsController;}	// the MICOS that controls this motor

	
	
	// additional smStatus codes
	static const int ErrCabel = ErrUnknown-1;	// Err, cable test failed
	static const int ErrInternal = ErrUnknown-2;	// internal error of the MICOS controler firmware


private:

	int StepsFrom_mm();
	char* nmFromSteps(int steps);
	
	smStatus GetIPar(const char* command, int& ipar);
	smStatus SetFPar(const char* command, float fpar, const char* additionalText = 0);
	
	char nm_string[30];
	
	MICOS_Controller * itsController;	// the MICOS that controls this motor
public:
	int itsMICOS_Addr;				// address of the MICOS
private:

	bool globalReset;	// set bei Reset(0), cleared by GetStatus

	char versionText[200];
	float moveVelocity;
	float iniVelocity;

	static int instanceCounter;
	static NumberedStrings * errorTexts;	// set of error texts


};


#endif // __INCStepperM_MICOSh
