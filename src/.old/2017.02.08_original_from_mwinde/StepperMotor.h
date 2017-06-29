// StepperMotor.h
//

// Drives a single stepper motor.
// Virtual class
// Both the stepper motor as well as the connection between the program
// and the motor have to be implemented in a derived class.
// (as an example see StepperM_PSS.h / .cpp)

// M. Winde, DESY - Zeuthen, 19-Sep-2000

#ifndef __INCStepperMotorh
#define __INCStepperMotorh

#include <stdint.h>
#include <sys/types.h>
#include "NumberedStrings.h"
#include "timeClass.h"

#define smStatus int

class StepperMotor
{
// 	implementation hints:

//	- There are 3 pure virtual functions  which have to be implemented:
//	  GoSteps(int), Stop(), GetStatus().
//	- The default version of Reset() does nothing, thus it usually has to be
//	  overridden too.
//	- The default version of GoToInitiator() does nothing. It should be overridden
//	  if an initiator (e. g. an end position switch) is provided by the hardware.
//  - It is highly recommended to override GetVersion().
//	- The default versions of SetParameter() and GetParameter() do nothing. They can
//	  be overridden for "character oriented" motor controllers, to provide an interface
//	  for an interactiv command interpreter (in a test phase, e. g.). Avoid to call these
//	  functions in an "ordinary" application (calls will hardly work with different
//	  controllers).
//	- All other virtual functions should be re-implemented for a given piece of
//	  hardware, if that hardware provides a better or more efficient way to perform
//	  the function.

	public:
	// --- creator, destructuctor
	StepperMotor();
	virtual ~StepperMotor();
	
	// --- Init, Reset
	
	// - perform initialisation, this does NOT include Reset(0);
	//   it does NOT include finding the zero position
	virtual smStatus Init();

	// - perform a reset of part "partToReset" (partial reset)
	//   if partToReset == 0: reset all parts (total reset)
	virtual smStatus Reset(int partToReset = 0){return Reset(partToReset, true);}
	
	// Usually Reset() waits some seconds after having reset the hardware.
	// Call Reset(n, false) to avoid waiting. (This is usefull if you reset a number of
	// motors, e. g. - you may want to wait only once.)
	virtual smStatus Reset(int /*partToReset*/, bool /*wait*/){return Ready;}
	
	// --- Go (run, move)

	// - move relative by "steps" steps;
	// +++ pure virtual function
	//   i) must update "actualPosition"
	//   ii) should do nothing on steps==0
	virtual smStatus GoSteps(int steps = 1) = 0;

	// - move absolute to "position" steps
	virtual smStatus GoToStep(int position) {return GoSteps(position-actualPosition);}
	
	// - go to the initiator in (positive / negative) "direction"			
	//   if "direction" >= 0: go into increasing absolute position
	//   else				  go into <0: decreasing absolute position
	virtual smStatus GoToInitiator(int direction);
	
	// - run into (positive / negative) "direction", stopped by Stop()
	virtual smStatus FreeRun(int direction);		
	
	// --- Stop
	// - stop movement, use speed down ramp "rampNb";
	//   0 is the default ramp;
	//   other ramps that may be implemented: 1 = emergency stop, ...
	// +++ pure virtual function
	virtual smStatus Stop(int rampNb = 0) = 0;
	
	// --- completion of Go and Stop operations
	// - wait for the motor to become "Ready"
	//   wait "seconds" seconds maximal, then return the stepper motor status,
	//	 ask for the status "inquiries" times in equal time distances
	virtual smStatus Wait(double seconds, int inquiries = 10);
	// return the seconds 'not used' in "secondsLeft"
	virtual smStatus Wait(double seconds, double &secondsLeft, int inquiries = 10);
	
	void SetTimeoutAfter(float sec); // ErrTimeOut will be generated for the next move after sec
    	smStatus CheckTime(smStatus status);	// if status!=Busy return status, else return Busy | ErrTimeOut
	void OperFinished() {operInProgress = false;}

	// --- Get information
	
	// - return a string that describes software and hardware connected
        virtual const char * GetVersion();

	// - get absolute position [steps]
	virtual smStatus GetSteps(int &position);

	// - get the actual status, see smStatus codes below
	// +++ pure virtual function
	virtual smStatus GetStatus() = 0;
	
	// - get the actual status, plus a (hardware dependent) status word
	//   of max. 32 bit length
	virtual smStatus GetStatus(uint32_t &statusBits);
	
	// --- set the absolute position to "position" steps, 
	//     usually used after GoToInitiator()
	virtual smStatus SetSteps(int position = 0);	
	
	// --- Set and Get other Parameters
	// -- Avoid to use these functions in "standard" programs. Use them merely as an interface
	//    for an interactiv program in a test phase.
	// set the parameter (or perform a function) specified by string "para" to the
	// value specified by string "value"
	virtual smStatus SetParameter(const char * const /*para*/, const char * const /*value*/){return Ready;}
	
	// get the parameter specified by string "para"
	// return its value as a string at "value"
	virtual smStatus GetParameter(const char * const /*para*/, char * value){*value = (char)0; return Ready;}
	
	virtual int GetMaxSteps() {return maxSteps;}
	virtual void SetMaxSteps(int steps) {maxSteps = steps;}

	// get the error string set
	static NumberedStrings * ErrorTexts() {return errorTexts;}
	
	// smStatus codes
	static const int Busy = 1;				// OK, but motor is moving or otherwise busy -> wait before move
	static const int Ready = 0;				// OK, ready to accept the next move command
	static const int ErrNotNow = -1;		// (program-)Err, command cannot be performed at that time
	static const int ErrIllParam = -2;		// (program-)Err, illegal parameter (e. g. try to move outside of current area limits)
	static const int ErrTimeOut = -3;		// (program-)Err, did not arrive at target position (standstill) in time
	static const int ErrSend = -4;			// Err, (repeated) errors during data send to motor controller
	static const int ErrReceive = -5;		// Err, (repeated) errors during data reveive from motor controller
	static const int ErrInitiator = -6;		// Err, motor at (forbidden) +/- initiator
	static const int ErrFailure = -7;		// Err, light hardware related failure, a Reset/Init my help
	static const int ErrSevere = -8;		// Err, severe hardware related failure
	static const int ErrUnexReset = -9;		// Err, controller was unexpectedly reset (e. g. switched on)

	static const int ErrUnknown = -10;		// Err, unclassified

protected:
	int normDirection(int direction){return direction >= 0 ? 1 : -1;}
	
	int actualPosition;	// MUST be updated by GoSteps() at least
	bool actPosKnown;	// actualPosition is correct (will be correct after the motor has stopped) 
	int maxSteps;		// max. number of steps, -1 for "unknown"

private:
	static int instanceCounter;
	static NumberedStrings * errorTexts;	// set of error texts	
	
	bool operInProgress;	// a time consuming operation is in progress
	timePoint timeoutTime;  // ErrTimeOut will be generated after that time
};
#endif
