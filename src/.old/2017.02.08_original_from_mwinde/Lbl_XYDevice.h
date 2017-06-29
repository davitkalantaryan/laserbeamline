// file Lbl_XYDevice.h
//
//
// Author:      Michael Winde <Michael.Winde@desy.de>
//              Copyright  2004 Michael Winde
//
//              This program is free software; you can redistribute it
//              and/or  modify it under  the terms of  the GNU General
//              Public  License as  published  by  the  Free  Software
//              Foundation;  either  version 2 of the License, or  (at
//              your option) any later version.
//
//

#ifndef lbl_XYDevice_h
#define lbl_XYDevice_h

#include "sema.h"
#include <eq_fct.h>
#include "ArchiveFile.h"

#include "StepperM_MICOS.h"
#include "eq_laserBeamLine.h"
#include "timeClass.h"

using namespace std;

class EqFctLbl_XYDevice;
class Drive;
class RotatingDrive;

class D_float_quad : public D_float {
public:
        D_float_quad(const char* pn, EqFctLbl_XYDevice* theEqFct): D_float(pn, (EqFct*)theEqFct){}
	void SetHalf(Drive* theHalf) {half = theHalf;}
	void set_value(float val);
private:
	Drive* half;
};

class D_int_quad : public D_int {
public:
        D_int_quad(const char* pn, EqFctLbl_XYDevice* theEqFct): D_int(pn, (EqFct*)theEqFct){}
	void SetHalf(Drive* theHalf) {half = theHalf;}
	void set_value(int val);
private:
	Drive* half;
};

class DoButton : public D_int {
public: 
        DoButton(const char* pn, EqFctLbl_XYDevice* theEqFct): D_int(pn, (EqFct*)theEqFct){}
	void SetHalf(Drive* theHalf) {half = theHalf;}
	void set_value(int val); 
private:
	Drive* half;
};

#define SimpleButton(_eq_fct_class_, _class_name_, _cmd_number_) \
class _class_name_ : public D_int { \
public: \
        _class_name_(const char* pn, _eq_fct_class_* theEqFct) \
	    :D_int(pn, (EqFct*)theEqFct), eq_fct_(theEqFct), commandNumber(_cmd_number_) {} \
	\
	void set_value(int val); \
private: \
	_eq_fct_class_* eq_fct_; \
	int commandNumber; \
}

#define D_int_cmd_(commandClass) \
void commandClass::set_value(int val) { \
	if(val) { \
		D_int::set_value(val); \
		eq_fct_->PerformCommand(commandNumber, this); \
	} \
} \

static const int cmdS_X = 0x10;	// command to be performed for the x-axis
static const int cmdS_Y = 0x20; // y-axis
static const int cmdS_Z = 0x40; // z-axis
static const int cmdS_X_fine = 0x80; // fine tuning to be performed for the x-axis
static const int cmdS_X_now = 0x100; // used in FindNormPosition() only:
static const int cmdS_Y_now = 0x200; // next step will be performed for the y-axis only
static const int cmdS_Z_now = 0x400; // .. for the z-axis only
static const int cmdS_X_fineNow = 0x800; // next step is fine tuning for the x-axis

static const int cmdS_terminates_others = 0x1000;

static const int cmdMask_baseCmds = 0xF; // base command must be in the range 1..0xf
static const int cmd_move = 1;
static const int cmd_moveToNominal = 2;
static const int cmd_stop = 3;
static const int cmd_zeroPosition = 4;
static const int cmd_setAsNominal = 5;
static const int cmd_resetController = 6;
static const int cmd_moveToZero = 7;
static const int cmd_init = 8;		// set normPosition = unknown, then perform zero position

SimpleButton(EqFctLbl_XYDevice, D_int_cmd_moveToNominal, cmdS_X | cmdS_Y | cmd_moveToNominal);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_moveBy, cmdS_X | cmdS_Y | cmd_move);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_stop, cmdS_X | cmdS_Y | cmdS_terminates_others | cmd_stop);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_resetController, cmdS_X | cmdS_Y | cmdS_terminates_others | cmd_resetController);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_setAsNominal, cmdS_X | cmdS_Y | cmd_setAsNominal);

SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_move, cmdS_X | cmd_move);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_moveToNominal, cmdS_X | cmd_moveToNominal);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_stop, cmdS_X | cmd_stop | cmdS_terminates_others);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_zeroPosition, cmdS_X | cmdS_Y | cmd_zeroPosition);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_init, cmdS_X | cmdS_Y | cmd_init);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_moveToZero, cmdS_X | cmd_moveToZero);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_X_setAsNominal, cmdS_X | cmd_setAsNominal);

SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_move, cmdS_Y | cmd_move);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_moveToNominal, cmdS_Y | cmd_moveToNominal);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_stop, cmdS_Y | cmd_stop | cmdS_terminates_others);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_zeroPosition, cmdS_X | cmdS_Y | cmd_zeroPosition);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_init, cmdS_X | cmdS_Y | cmd_init);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_moveToZero, cmdS_Y | cmd_moveToZero);
SimpleButton(EqFctLbl_XYDevice, D_int_cmd_Y_setAsNominal, cmdS_Y | cmd_setAsNominal);

class EqFctLbl_XYDevice : public EqFct {

friend class Drive;
friend class RotatingDrive;
friend class DoButton;

protected:

    u_short 	control;	// var for D_bit
    D_name		alias_;

    D_int opticalDeviceNumber;  // 0..EqFctlaserBeamLine::devicesConnected-1
    D_int controllerNumber;	// 0..maxNbOfControllers-1, controller for this motors
    D_int status;		/* status of the mover: <0 error, 0: OK - ready, 1: moving, */
			    /* 2: position unknown, 3: Target position not reachable */
    D_int motorPositionsKnown;	// 1=sure, 0=assumed, -1=unknown
    
    char errMover_str[STRING_LENGTH];	
    D_string errMover;	/* text showing the status of the motor */

    char nextDevice_str[STRING_LENGTH];	
    D_string nextDevice;	/* text showing the name of the next device in the laser beam */
    D_float nextDeviceDistance;	// mm to next device
    
    D_int minResolution;	// minimal resolution of the light barrier (diode)
    D_int hysteresis;  		// luft of the gear (in motor steps)
    D_int fineStartSteps;  	// initial stepwidth fine tuning
    D_int rawStartSteps;  	// initial stepwidth raw tuning
    D_int autoInit;		// init the device at start (should be on (1))
    D_int testSettings;  	// bits determine special test settings

    D_int_cmd_moveBy   button_moveBy;
    D_int_cmd_moveToNominal   button_moveToNominal;
    D_int_cmd_stop   button_stop;
    D_int_cmd_resetController   button_resetController;
    D_int_cmd_setAsNominal   button_setAsNominal;
    
    DoButton getOrSet_X;
    DoButton getOrSet_Y;
    D_string cmdSet;
    D_string cmdGet;
    D_string cmdAck;

    D_int X_motorNumber;	// X motor number (as addresses by the controller)
    D_int X_diode_bit;	// this bit in the IP-Unidig contains the state of the zero-position diode
    D_int X_diode_dark_at0; // 1: it should be dark at pos 0 (at -initiator), 0: should be light
    D_float X_axisFactor;   // mm (on target) from steps
    char X_text_str[STRING_LENGTH];	
    D_string X_text;	/* text describing what x-direction really is (e. g. "PITZ x" */
    D_int X_errNb;			// X motor status
    char X_err_str[STRING_LENGTH];	
    D_string X_errTxt;
    smStatus X_status;
    D_int X_Nominal;
    D_int X_stepsActAbs;
    D_int X_lastValidSteps;
    D_int X_stepsActNominal;
    D_float X_mmActAbs;
    D_float X_mmActNominal;
    D_int_quad X_stepsTo;
    D_int_quad X_stepsBy;
    D_float_quad X_mmTo;
    D_float_quad X_mmBy;
    D_float X_mmUpLimit;
    D_float X_mmLowLimit;
    D_int X_destinationReachable;
    D_int X_stepsMax;
    D_int X_stepsMaxMax;
    D_int X_stepsNormPosition;

    D_hist X_mmAct_hist_;

    D_int_cmd_X_move   X_button_move;
    D_int_cmd_X_moveToNominal   X_button_moveToNominal;
    D_int_cmd_X_stop   X_button_stop;
    D_int_cmd_X_zeroPosition   X_button_zeroPosition;
    D_int_cmd_X_init   X_button_init;
    D_int_cmd_X_moveToZero   X_button_moveToZero;
    D_int_cmd_X_setAsNominal   X_button_setAsNominal;

    D_int Y_motorNumber;		// Y motor number
    D_int Y_diode_bit;	// this bit in the IP-Unidig contains the state of the zero-position diode
    D_int Y_diode_dark_at0; // 1: it should be dark at pos 0 (at -initiator), 0: should be light
    D_float Y_axisFactor;   // mm (on target) from steps
    char Y_text_str[STRING_LENGTH];	
    D_string Y_text;	/* text describing what y-direction really is (e. g. "vertical" */
    D_int Y_errNb;			// Y motor status
    char Y_err_str[STRING_LENGTH];	
    D_string Y_errTxt;
    smStatus Y_status;
    D_int Y_Nominal;
    D_int Y_stepsActAbs;
    D_int Y_lastValidSteps;
    D_int Y_stepsActNominal;
    D_float Y_mmActAbs;
    D_float Y_mmActNominal;
    D_int_quad Y_stepsTo;
    D_int_quad Y_stepsBy;
    D_float_quad Y_mmTo;
    D_float_quad Y_mmBy;
    D_float Y_mmUpLimit;
    D_float Y_mmLowLimit;
    D_int Y_destinationReachable;
    D_int Y_stepsMax;
    D_int Y_stepsMaxMax;
    D_int Y_stepsNormPosition;

    D_hist Y_mmAct_hist_;

    D_int_cmd_Y_move   Y_button_move;
    D_int_cmd_Y_moveToNominal   Y_button_moveToNominal;
    D_int_cmd_Y_stop   Y_button_stop;
    D_int_cmd_Y_zeroPosition   Y_button_zeroPosition;
    D_int_cmd_Y_init   Y_button_init;
    D_int_cmd_Y_moveToZero   Y_button_moveToZero;
    D_int_cmd_Y_setAsNominal   Y_button_setAsNominal;

    Drive * X_drive;
    Drive * Y_drive;

    MICOS_Controller* theMICOS_Controller;
    int pendingCommand;

public:
    EqFctLbl_XYDevice();		// constructor
    ~EqFctLbl_XYDevice() { ; }	// destructor

    virtual void	update ();
    virtual void updateAllDrives();
    virtual void FillHistories(int error);
    virtual void	init ();	// started after creation of all Eq's
    virtual void CreateDrives();
    virtual void initHardware();
    virtual void online();
    
    virtual int	fct_code()	{ return CodeLbl_XY_Device; }

    static int	conf_done;

    void PerformCommand(int val, D_int * itsButton);
    virtual smStatus PerformSeletedCmd(int val);
    
    virtual smStatus TryReady();
    void commandFinished(smStatus stat);
    
    NumberedStrings* ErrorTexts() {return errorTexts;}
    
    virtual smStatus SetAsNominal(int val);   	

    virtual void NotifyStatus();
    smStatus NotifyAxisStatus(smStatus mostSignificantErr, smStatus thisErr,
		bool destinationReachable, D_int* errNb, D_string* errTxt);
    void NotifyOverallStatus(smStatus stat);
    
protected:

    static NumberedStrings* errorTexts;
    static const int errBase = -10000;		    // errors detected by this part of the server are in the range errBase..errBase-n
    static const int errIllCmd = errBase-1;	    // client error: illegal cmd number
    static const int errCommandPending = errBase-4; // client error: tried to move while the 
						    // last operation is still in progress
    static const int errDeviceOffline = errBase-5;  // device is offline

private:
    
    void ActPosKnown(bool isKnown);
    
    D_int * pendingButton;	// the button, that was pressed to force the pending command,
    				// NULL means: no button pressed
};

class Drive {

friend class EqFctLbl_XYDevice;
friend class EqFctLbl_XYZ_Device;
friend class DoButton;

public:
    Drive(EqFctLbl_XYDevice* theMirror, MICOS_Controller* theMICOS,
        int theMotorNumber, int diodesBit, const char* the_text_str,
        D_int* theNominal, D_int* theStepsActAbs, D_int* theLastValid, D_int* theStepsActNominal, 
	D_float* the_mmActAbs, D_float* the_mmActNominal,
	D_float* the_mmLowLimit, D_float* the_mmUpLimit,
    	D_int_quad* the_stepsTo, D_int_quad* the_stepsBy, D_float_quad* the_mmTo, D_float_quad* the_mmBy,
	D_int* theDestinationOK, D_int* theStepsMax, D_int* theStepsMaxMax, D_int* thestepsNormPosition, 
	int the_axisFactor, bool the_diode_dark_at0);
    
    void Init();
    virtual void AdjustQuad(D_int_quad* an_int_quad);
    virtual void AdjustQuad(D_float_quad* a_float_quad);

    virtual void CheckIfTargetCanBeReached();
    bool PositionOK();
    void update();
    
    const char* DriveText();
    
    smStatus SetAsNominal();
    virtual void UpdateLimits();
    virtual smStatus MoveTo(int  moveTo_rel_steps, bool fixedStepsBy = true);
    smStatus Stop();
    virtual smStatus FindNormPosition(int& pending, int this_now);
    
    virtual float Calc_mm(int steps);
    virtual int Calc_Steps(float mm);
       
    void ContinueAt(int contMarker) {continueAt = contMarker;}
    int ContinueAt() {return continueAt;}
    NumberedStrings* ErrorTexts() {return errorTexts;}
    
    StepperM_MICOS* stepperMotor;

protected:

    EqFctLbl_XYDevice* mirror;
    const char* _text_str;
    D_int* nominal;
    D_int* stepsActAbs;
    D_int* stepsActNominal;
    D_int* lastValidSteps;	// saved after Move(), used by FindNormPosition()

    D_float* mmActAbs;
    D_float* mmActNominal;
    D_float* mmLowLimit;
    D_float* mmUpLimit;
    D_int_quad* stepsTo;
    D_int_quad* stepsBy;
    D_float_quad* mmTo;
    D_float_quad* mmBy;
    D_int* stepsMax;
    D_int* stepsMaxMax;
    D_int* stepsNormPosition;
    int actStepWidth;
    D_int* destinationOK;
    bool diode_dark_at0;
    int axisFactor;     // axisFactor includes: mirror geometry, motor orientation,
    			// it is about 1000000 * 40 for the mirrors
    			// +/-1000000 for all other motors

    
    int diodesBitPattern;	// this bit in the IP-Unidig contains the state of the zero-position diode
    timePoint timeOutTime;	// wait for this timePoint after motor controller reset
    
    static NumberedStrings* errorTexts;
    static const int errBase = -11000;		// errors detected by this part of the server are in the range errBase..errBase-n
    static const int errBadDest = errBase-1;	// illegal value of destination
    static const int ErrWrongProgramFlow = errBase-2;	// wrong Program Flow
    static const int errDiodeDark = errBase-3;	// diode seems to see permanently DARK
    static const int errDiodeLight = errBase-4;	// diode seems to see permanently LIGHT
    static const int errMinusEndSwitch = errBase-5;	// - end switch not reached
    static const int errPlusEndSwitch = errBase-6;	// + end switch not reached
    static const int errDiodeBlind = errBase-7;	// diode does not change its state, wrong connected
    int continueAt;
    
};

#endif
