
#define DEBUG 1

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#endif

#include "StepperM_MICOS.h"

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "MICOS_Controller.h"

int StepperM_MICOS::instanceCounter;
NumberedStrings * StepperM_MICOS::errorTexts;

// creators
StepperM_MICOS::StepperM_MICOS(MICOS_Controller * theSMController, int theMICOS_Addr):
		StepperMotorWithProfile()
{
    itsController = theSMController;
    itsMICOS_Addr = theMICOS_Addr;
    iniVelocity = moveVelocity = 1e06;	// just an estimate for timeout, improved by Init()
    strcpy(versionText, "V2.0, 08-Feb-2017, M. Winde -- Micos Schrittmotor (MICOS)");
    strcat(versionText, itsController->GetVersion());
    if(instanceCounter == 0) {
            errorTexts = new NumberedStrings(StepperMotor::ErrorTexts());
    }
    instanceCounter++;    
}

StepperM_MICOS::~StepperM_MICOS() {
    if(--instanceCounter == 0) {
	    delete errorTexts;
    }
}

smStatus StepperM_MICOS::Reset(int, bool) {
    smStatus status = itsController->OnlySendTelegramm(this, "nreset", "");
    return status;
}

smStatus StepperM_MICOS::Init() {
    smStatus stat = 0;
    int vel;
    stat = GetVelocity(vel);
    moveVelocity = vel;
    iniVelocity = vel;	// good enough for the only reason to define a rough timeout
    maxSteps = 1000000 * 1000;	// should be better read from motor 
    return stat;
}

void StepperM_MICOS::SetPositionKnown(bool theActPosKnown, int /*theActualPosition*/) {
    actPosKnown = theActPosKnown;
}

smStatus StepperM_MICOS::GetSteps(int &position) {
    smStatus error = itsController -> TelegramDialog(this, "np", "");
    if(error == 0) {
	position = StepsFrom_mm();
	actPosKnown = true;
    }
    itsController->Post();
    return error;
}

int StepperM_MICOS::StepsFrom_mm() {
    // mm are awaited as a char string with a decimal point and
    // digits after the decimal point
    itsController->MICOS_AckData[itsController->MICOS_AckDataLength-7-1] = char(0); // dot, 6 digits, space
    int steps_6 = atoi(itsController->MICOS_AckData);	// number before decimal point
    int steps = atoi(itsController->MICOS_AckData + itsController->MICOS_AckDataLength -7); // behind decimal point
    if(itsController->MICOS_AckData[0] == '-') {
        steps = -steps;
    }
    return steps_6 * 1000000 + steps;
}

char* StepperM_MICOS::nmFromSteps(int steps) {
    //???sprintf(nm_string, "%d", steps);
    float fsteps = steps;
    sprintf(nm_string, "%f", fsteps/1000000.);
    return nm_string;
}

smStatus StepperM_MICOS::GoToStep(int position) {
    /*if((position > maxSteps) || (position < 0)) {
    	//cerr << "StepperM_MICOS::GoToStep to " << position << " ErrIllParam maxSteps=" << maxSteps << endl;
	return ErrIllParam;
    }
    */
    float timeOT = (maxSteps/moveVelocity)*2. +3.;
    //float timeOT = (30.)*2.;
    //cerr << "StepperM_MICOS::GoToStep to " << position << " timeOT = " << timeOT;
    //SetTimeoutAfter(timeOT);
    SetTimeoutAfter(-1.);
    char* nmSteps;
    nmSteps = nmFromSteps(position);
    //cerr << ", nmSteps=" << nmSteps << endl;
    smStatus status = itsController->OnlySendTelegramm(this, "nm", nmFromSteps(position));
    return status;
}

smStatus StepperM_MICOS::GoSteps(int steps) {

    if(steps == 0) return 0;	// nothing to step

    smStatus status(0);
    int actPosition;
    status = GetSteps(actPosition);
    if(status == 0) 
    	status = GoToStep(actPosition + steps);
    return status;
}

smStatus StepperM_MICOS::GoToInitiator(int paras) {
    smStatus status;
    float timeOT;
    if(maxSteps > 0) {
        timeOT = (maxSteps/iniVelocity)*2. +30.;
    } else {
        timeOT = 300.; // 300 sec.
    }
    SetTimeoutAfter(timeOT);
    if (paras > 0 ){
	status = itsController -> OnlySendTelegramm(this, "nrm", "");
    } else if (paras < 0){
	status = itsController -> OnlySendTelegramm(this, "ncal", "");
    } else return ErrIllParam;
    actPosKnown = true;
    return status;
}

smStatus StepperM_MICOS::FreeRun(int paras) {
    SetTimeoutAfter(-1.); // no timeout
    actPosKnown = false;
    return GoToInitiator(paras);  // seems to be the best aproximation
}

smStatus StepperM_MICOS::Stop(int /*rampNb*/) {
    actPosKnown = false;
    smStatus status = itsController -> OnlySendTelegramm(this, "nabort", "");
    OperFinished();
    return status;
}

const char * StepperM_MICOS::GetVersion(){
    return versionText;
}

smStatus StepperM_MICOS::SetSteps(int position) {
//?? do not use this from outside: GoToStep(step) awaits 0 < 'step' < maxSteps
    actPosKnown = true;
    return itsController->OnlySendTelegramm(this, "setnpos", nmFromSteps(position));
}

void StepperM_MICOS::SetMaxSteps(int steps) {
    StepperMotor::SetMaxSteps(steps);
    char limitString[30];
    sprintf(limitString, "0.0 %f", steps*0.000001);
    smStatus status = itsController->OnlySendTelegramm(this, "setnlimit", limitString);
}

// - get the actual status
smStatus StepperM_MICOS::GetStatus()	{
	uint32_t statusBits;
	return GetStatus(statusBits);
}

// - get the actual status + 32 bit status word
smStatus StepperM_MICOS::GetStatus(uint32_t &statusBits)	{

    statusBits=0;
/*    itsController -> MakeTelegramm(itsMICOS_Addr, "nstatus", "");
    smStatus status = itsController -> SendAndAckTelegram();	*/
    smStatus status = itsController -> TelegramDialog(this, "nstatus", "");
    if(status == 0) { // no transfer error
        status = atoi(itsController->MICOS_AckData);
    }
    itsController->Post();
    if (status != 0) {
	return StepperMotor::CheckTime(status);
    }
    OperFinished();
    statusBits = GetError();
    if (statusBits == 0) {
	return Ready;
    }
    if (statusBits == 2 || statusBits == 3 || statusBits == 4) return ErrInternal;
    if (statusBits == 1001) return ErrIllParam;	// "
    if (statusBits == 1002) return ErrIllParam;
    if (statusBits == 1003) return ErrFailure;
    if (statusBits == 1004) return ErrIllParam;
    if (statusBits == 1015) return ErrIllParam;
    if (statusBits == 2000) return ErrUnknown;
    // unknown error code, send by the motor - should never occure
    itsController->Post();
    return ErrSevere;
}

// - wait for the motor to become "Ready"
//   then close the brakes
smStatus StepperM_MICOS::Wait(double seconds, double &secondsLeft, int inquiries) {
    smStatus status = StepperMotor::Wait(seconds, secondsLeft, inquiries);
    return status;
}

smStatus StepperM_MICOS::Wait(double seconds, int inquiries) {
    double secondsLeft;
    return Wait(seconds, secondsLeft, inquiries);
}

smStatus StepperM_MICOS::SetParameter(const char * const para, const char * const value) {
	smStatus status = itsController->OnlySendTelegramm(this, para, value);
	//cerr << "StepperM_MICOS::SetParameter " << para << "==" << value << endl;
	return status;
}

smStatus StepperM_MICOS::GetParameter(const char * const para, char * value) {
	smStatus status = itsController->TelegramDialog(this, para, "");
	strcpy(value, itsController->MICOS_AckData);
    	itsController->Post();
	return status;
}

smStatus StepperM_MICOS::SetIPar(const char* command, int ipar, const char* additionalText) {
    char chIPar[50];
    if(additionalText) {
        sprintf(chIPar, "%d%s", ipar, additionalText);
    } else {
    	sprintf(chIPar, "%d", ipar);
    }
    return itsController->OnlySendTelegramm(this, command, chIPar);
}

smStatus StepperM_MICOS::GetIPar(const char* command, int& ipar){
    smStatus status = itsController->TelegramDialog(this, command, "");
    ipar = atoi(itsController->MICOS_AckData);
    itsController->Post();
    return status;
}

smStatus StepperM_MICOS::SetFPar(const char* command, float fpar, const char* additionalText) {
    char chIPar[50];
    if(additionalText) {
        sprintf(chIPar, "%f%s", fpar, additionalText);
    } else {
        sprintf(chIPar, "%f", fpar);
    }
    return itsController->OnlySendTelegramm(this, command, chIPar);
}

smStatus StepperM_MICOS::SetMoveCurrent(int iMove){
    return SetIPar("setumotgrad", iMove);
}

smStatus StepperM_MICOS::GetMoveCurrent(int& iMove){
    return GetIPar("getumotgrad", iMove);
}

smStatus StepperM_MICOS::SetStopCurrent(int iStop){
    return SetIPar("setumotmin", iStop);
}

smStatus StepperM_MICOS::GetStopCurrent(int& iStop){
    return GetIPar("getumotmin", iStop);
}

smStatus StepperM_MICOS::SetCalVelocity(int steps_per_sec_to, int steps_per_sec_back){
    iniVelocity = float(steps_per_sec_to);
    smStatus s1 = SetFPar("setncalvel", steps_per_sec_to/1000000., " 1");
    if(s1 < 0) return s1;
    return SetFPar("setncalvel", steps_per_sec_back/1000000., " 2");
}

smStatus StepperM_MICOS::SetVelocity(int steps_per_sec){
    moveVelocity = float(steps_per_sec);
    return SetFPar("snv", steps_per_sec/1000000.);
}

smStatus StepperM_MICOS::GetVelocity(int& steps_per_sec){
    smStatus error = itsController -> TelegramDialog(this, "gnv", "");
    //cerr << "error=" << error << ", velocity=<" << itsController->MICOS_AckData << ">" << endl;
    steps_per_sec = StepsFrom_mm();
     itsController->Post();
   return error;
}

smStatus StepperM_MICOS::SetAcceleration(int steps_per_sec2){
    return SetFPar("sna", steps_per_sec2/1000000.);
}

smStatus StepperM_MICOS::GetAcceleration(int& steps_per_sec2){
    smStatus error = itsController -> TelegramDialog(this, "gna", "");
    cerr << "error=" << error << ", acc=<" << itsController->MICOS_AckData << ">" << endl;
    itsController->MICOS_AckData[itsController->MICOS_AckDataLength-4-1] = char(0); // dot, 4 digits, space
    int steps_6 = atoi(itsController->MICOS_AckData);	// number before decimal point
    int steps = atoi(itsController->MICOS_AckData + itsController->MICOS_AckDataLength -4); // behind decimal point
    if(itsController->MICOS_AckData[0] == '-') steps = -steps;
    steps_per_sec2 = steps_6 * 1000000 + steps;
    itsController->Post();
    return error;
}

smStatus StepperM_MICOS::GetError() {
    int error = itsController -> TelegramDialog(this, "gne", "");
    if(error == 0) { // there was an acknowledgement to "gne"
	error = atoi(itsController->MICOS_AckData);	// always > 0
	if(error != 0) cerr << "StepperM_MICOS::GetError() = " << error << endl;
    }
    itsController->Post();
    return error;
}

smStatus StepperM_MICOS::GetLimitSwitches(int& lMinus, int& lPlus) {
    smStatus stat;
    char value[10];
    stat = GetParameter("getswst", value);
    if(stat != 0) return stat;
    sscanf(value, "%d%d", &lMinus, &lPlus);
    return 0;
}
    
smStatus StepperM_MICOS::CheckCable() {
    // if no cable is plugged both limit switches are == 1
    // check agains this
    smStatus stat;
    int lMinus, lPlus;
    stat = GetLimitSwitches(lMinus, lPlus);
    cerr << "StepperM_MICOS::CheckCable() stat = " << stat << endl;
    if(stat != 0) return stat;
    cerr << "StepperM_MICOS::CheckCable() lMinus = " << lMinus << ", lPlus = " << lPlus << endl;
    if((lMinus == 0) || (lPlus == 0)) return 0;
    return ErrCabel;
}
