// StepperMotor.cpp

#include "StepperMotor.h"

int StepperMotor::instanceCounter;
NumberedStrings * StepperMotor::errorTexts;

StepperMotor::StepperMotor() {
	if(instanceCounter == 0) {
		errorTexts = new NumberedStrings(NULL, 11);
		errorTexts->Append(Busy, "moving or otherwise busy");
		errorTexts->Append(Ready, "OK, ready");
		errorTexts->Append(ErrNotNow, "command cannot be performed at that time");
		errorTexts->Append(ErrIllParam, "illegal parameter");
		errorTexts->Append(ErrTimeOut, "did not arrive at target position in time (timeout)");
		errorTexts->Append(ErrSend, "errors during data send to motor controller");
		errorTexts->Append(ErrReceive, "errors during data reveive from motor controller");
		errorTexts->Append(ErrInitiator, "hardware malfunction: motor at forbidden initiator");
		errorTexts->Append(ErrFailure, "hardware malfunction, a Reset my help");
		errorTexts->Append(ErrSevere, "severe hardware related failure");
		errorTexts->Append(ErrUnexReset, "controller was unexpectedly reset (e. g. switched on)");
	}
	instanceCounter++;
	operInProgress = false;
	StepperMotor::Init();
}

StepperMotor::~StepperMotor() {
	if(--instanceCounter == 0) {
		delete errorTexts;
	}
}

// - perform initialisation
smStatus StepperMotor::Init() {
	actPosKnown = false;
	actualPosition = 0;
	maxSteps = -1;
	return Ready;
}

// - go to the initiator in (positive / negative) "direction"			
//   if "direction" >= 0: go into increasing absolute position
//   else				  go into <0: decreasing absolute position
smStatus StepperMotor::GoToInitiator(int direction) {
	return GoSteps(normDirection(direction)*maxSteps);
}

// run into (positive / negative) "direction", stopped by Stop()
smStatus StepperMotor::FreeRun(int direction){
	return GoSteps(normDirection(direction)*maxSteps);
}		
	
// - return a string that describes software and hardware connected
const char * StepperMotor::GetVersion() {
	return "V1.0, 19-Sep-2000, M. Winde -- unknown motors";
}

// - get absolute position [steps]
smStatus StepperMotor::GetSteps(int &position){
	position = actualPosition;
	return Ready;
}

// - get the actual status, plus a (hardware dependent) status word
//   of max. 32 bit length
smStatus StepperMotor::GetStatus(uint32_t &statusBits) {
	statusBits = 0;
	return GetStatus();
}

void StepperMotor::SetTimeoutAfter(float sec) {
    if(operInProgress) return; // timeout was already set
    operInProgress = (sec > 0.);
    if(operInProgress) {
        timeoutTime = timePoint() + timeInterval(sec);
    }
}

smStatus StepperMotor::CheckTime(smStatus status) {
    if(status != Busy) {
        operInProgress = false;
        return status;
    }
    // Busy, check on timeOut
    if(operInProgress) {
	if(timePoint() > timeoutTime) {
	    return ErrTimeOut;
	}
    }
    return status;
}

// - wait for the motor to become "Ready"
//   wait "seconds" seconds maximal, then return the stepper motor status
//   return the seconds 'not used' in "secondsLeft"
smStatus StepperMotor::Wait(double seconds, double &secondsLeft, int inquiries) {
	
	if(inquiries <= 0) inquiries = 1;
	double waitQuant = seconds/inquiries;		// calc time quantum for next wait
	if(waitQuant <= 0.001) waitQuant = 0.001;	// assure min. wait quantum is 1 mSec
	
	smStatus status;
	timeInterval waitedSum(0.);	
	timeInterval toWait(waitQuant);
	timeInterval toWaitSum(seconds);
	while( (status = GetStatus()) == Busy) {
		if( waitedSum >= toWaitSum) {
			secondsLeft = 0.0;
			return ErrTimeOut;
		}
		toWait.wait();
		waitedSum += waitQuant;
	}
	secondsLeft = seconds - waitedSum.get();
	return status;
}	

// - wait for the motor to become "Ready"
//   wait "seconds" seconds maximal, then return the stepper motor status
smStatus StepperMotor::Wait(double seconds, int inquiries) {
	
	double secondsLeft;
	return Wait(seconds, secondsLeft, inquiries);
}	

// --- set the absolute position to "position" steps, 
//     usually used after GoToInitiator()
smStatus StepperMotor:: SetSteps(int position) {
	actualPosition = position;
	actPosKnown = true;
	return Ready;
}	
	
