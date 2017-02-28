#include	<math.h>
#include	"eq_errors.h"
#include	"eq_sts_codes.h"
#include	"eq_fct_errors.h"
#include 	"eq_client.h"

#include "eq_laserBeamLine.h"
#include "Lbl_XYDevice.h"
#include "MICOS_Controller.h"
#include "timeClass.h"

#include 	"MICOS_Controller.h"

#define DEBUG
#define D_TryReady 0x1
#define D_FindNorm 0x2
#define D_MoveTo 0x4
#include "doocsDebug.h"

extern MICOS_Controller* MICOS_controllers[maxNbOfControllers];	// array of all controllers
extern bool actPosOfDeviceIsKnown[maxDevicesConnected];		// array[optical devices]
extern int statusOfDevice[maxDevicesConnected];			// array[optical devices]
extern IPUnidig* unidig_lbl;						// the IP-Unidig modul
extern bool EqFctlaserBeamLineInitDone;

int	EqFctLbl_XYDevice::conf_done = 0;

extern ArchiveFile*	arch;
extern time_t		walltime;
extern  int		ring_buffer;

static const char* cmdAsText[] =	{
	"cmd_move", "cmd_moveToNominal", "cmd_stop", "cmd_zeroPosition",
	"cmd_setAsNominal", "cmd_resetController", "cmd_moveToZero", "cmd_init" };

EqFctLbl_XYDevice::EqFctLbl_XYDevice():
    EqFct ("NAME = location" )
    ,alias_("ALIAS device name",this )

    ,opticalDeviceNumber("OPTDEVICE nb. of this device in the chain of the LBL", this)
    ,controllerNumber("CONTROLLER_NUMBER nb. of allocated MICOS_Controller", this)
    
    // status of the mover: <0 error, 0: OK - ready, 1: moving
    // 2: position unknown, 3: Target position not reachable
    ,status("DEVICE.ERR.NB device status", this)
    ,motorPositionsKnown("POSITIONS.KNOWN 1=sure, 0=assumed, -1=unknown", this)
    
    // text showing the status of the optical device, color should change:
    // green: ready (to accept next go command), yellow: moving, red: error
    ,errMover("DEVICE.ERR.TXT device error", errMover_str, STRING_LENGTH, this)
    
    // the "next device" is the one, that is hit next by the laser beam
    ,nextDevice("NEXTDEVICE.TEXT name of next device", nextDevice_str, STRING_LENGTH, this)
    ,nextDeviceDistance("NEXTDEVICE.DISTANCE mm to next device", this)
    
    ,minResolution("MINRESOLUTION.DIODE minimal resolution of the light barrier", this)
    ,hysteresis("HYSTERESYS luft of the gear (in motor steps)", this)
    ,fineStartSteps("FINE.FIRSTSTEP initial motor steps fine tuning", this)
    ,rawStartSteps("RAW.FIRSTSTEP initial motor steps midle position finding", this)
    
    ,autoInit("AUTOINIT init the device at start (normally off)", this)
    ,testSettings("TESTSETTINGS bit 0:not used 1:FindNorm 2:MoveTo", this)
    
    ,button_moveBy("DOMOVEBY command move both directions by", this)
    ,button_moveToNominal("DOMOVETONOMINAL command both directions to nominal position", this)
    ,button_stop("DOSTOP command emergency stop", this)
    ,button_resetController("DORESET command reset controller of motors", this)
    ,button_setAsNominal("DOSETNOMINAL nominal = actual", this)
    
    ,getOrSet_X("X.DO.CMD.GET_OR_SET perform GetParameter(1) or SetParameter(2)", this)
    ,getOrSet_Y("Y.DO.CMD.GET_OR_SET perform GetParameter(1) or SetParameter(2)", this)
    ,cmdSet("CMD.SET command performed when GET_OR_SET = 2", this)
    ,cmdGet("CMD.GET command performed when GET_OR_SET = 1", this)
    ,cmdAck("CMD.ACK Acknowledge returned after GET_OR_SET = 1", this)
    
    ,X_motorNumber("X.MOTOR_NUMBER X motor number (as addresses by the controller)", this)
    ,X_diode_bit("X.DIODE_BIT 1-bit-pattern for IP-Unidig mask", this)
    ,X_diode_dark_at0("X.DIODE.DARK_AT0 1: dark at zero pos, 0: light", this)
    ,X_axisFactor("X.AXIS_FACTOR mm (on target) from steps", this)
    ,X_text("X.DIRECTION_TEXT description", X_text_str, STRING_LENGTH, this)
    ,X_errNb("X.ERR.NB motor status", this)
    ,X_errTxt("X.ERR.TXT motor status text", X_err_str, STRING_LENGTH, this)
    ,X_Nominal("X.STEPS.NOMINAL steps nominal 0 minus absolut 0", this)
    ,X_stepsActAbs("X.STEPS.ACT.ABS actual steps relative to X.STEPS.DIODE", this)
    ,X_lastValidSteps("X.STEPS.ACT.VALID steps after last valid move", this)
    ,X_stepsActNominal("X.STEPS.ACT.NOMINAL actual steps relativ to nominal 0", this)
    ,X_mmActAbs("X.MM.ACT.ABS absolute actual mm", this)
    ,X_mmActNominal("X.MM.ACT.NOMINAL actual position relativ to nominal 0", this)
    ,X_stepsTo("X.STEPS.TO target position in abs steps", this)
    ,X_stepsBy("X.STEPS.BY target position in relative steps", this)
    ,X_mmTo("X.MM.TO target position in abs mm on next device", this)
    ,X_mmBy("X.MM.BY target position in relative mm on next device", this)
    ,X_mmUpLimit("X.MM.UPLIMIT upper limit for X.MM.TO", this)
    ,X_mmLowLimit("X.MM.LOWLIMIT lower limit for X.MM.TO", this)
    ,X_destinationReachable("X.DEST.OK target position is reachable", this)
    ,X_stepsMax("X.STEPS.MAX highest position", this)
    ,X_stepsMaxMax("X.STEPS.MAXMAX for sure greater than highest position", this)
    ,X_stepsNormPosition("X.STEPS.DIODE photo diode changes its state", this)
    ,X_mmAct_hist_("X.MM.ACT.NOMINAL.HIST absolute actual mm", this)
    
    ,X_button_move("X.DOMOVEBY command move to/by", this)
    ,X_button_moveToNominal("X.DOMOVETONOMINAL move to nominal position", this)
    ,X_button_stop("X.DOSTOP stop motor", this)
    ,X_button_zeroPosition("X.DOZEROPOS find NormPosition - photo diode changes its state", this)
    ,X_button_init("X.DOINIT init- includes find NormPosition", this)
    ,X_button_moveToZero("X.DOMOVETOZERO move to abs 0 (NormPosition)", this)
    ,X_button_setAsNominal("X.DOSETNOMINAL X nominal = X actual", this)
    
    ,Y_motorNumber("Y.MOTOR_NUMBER Y motor number (as addresses by the controller)", this)
    ,Y_diode_bit("Y.DIODE_BIT 1-bit-pattern for IP-Unidig mask", this)
    ,Y_diode_dark_at0("Y.DIODE.DARK_AT0 1: dark at zero pos, 0: light", this)
    ,Y_axisFactor("Y.AXIS_FACTOR mm (on target) from steps", this)
    ,Y_text("Y.DIRECTION_TEXT description", Y_text_str, STRING_LENGTH, this)
    ,Y_errNb("Y.ERR.NB motor status", this)
    ,Y_errTxt("Y.ERR.TXT motor status text", Y_err_str, STRING_LENGTH, this)
    ,Y_Nominal("Y.STEPS.NOMINAL steps nominal 0 minus absolut 0", this)
    ,Y_stepsActAbs("Y.STEPS.ACT.ABS actual steps relative to Y.STEPS.DIODE", this)
    ,Y_lastValidSteps("Y.STEPS.ACT.VALID steps after last valid move", this)
    ,Y_stepsActNominal("Y.STEPS.ACT.NOMINAL actual steps relativ to nominal 0", this)
    ,Y_mmActAbs("Y.MM.ACT.ABS absolute actual mm", this)
    ,Y_mmActNominal("Y.MM.ACT.NOMINAL actual position relativ to nominal 0", this)
    ,Y_stepsTo("Y.STEPS.TO target position in abs steps", this)
    ,Y_stepsBy("Y.STEPS.BY target position in relative steps", this)
    ,Y_mmTo("Y.MM.TO target position in abs mm on next device", this)
    ,Y_mmBy("Y.MM.BY target position in relative mm on next device", this)
    ,Y_mmUpLimit("Y.MM.UPLIMIT upper limit for Y.MM.TO", this)
    ,Y_mmLowLimit("Y.MM.LOWLIMIT lower limit for Y.MM.TO", this)
    ,Y_destinationReachable("Y.DEST.OK target position is reachable", this)
    ,Y_stepsMax("Y.STEPS.MAX highest position", this)
    ,Y_stepsMaxMax("Y.STEPS.MAXMAX for sure greater than highest position", this)
    ,Y_stepsNormPosition("Y.STEPS.DIODE photo diode changes its state", this)
    ,Y_mmAct_hist_("Y.MM.ACT.NOMINAL.HIST absolute actual mm", this)

    ,Y_button_move("Y.DOMOVEBY command move to/by", this)
    ,Y_button_moveToNominal("Y.DOMOVETONOMINAL move to nominal position", this)
    ,Y_button_stop("Y.DOSTOP stop motor", this)
    ,Y_button_zeroPosition("Y.DOZEROPOS find NormPosition - photo diode changes its state", this)
    ,Y_button_init("Y.DOINIT init- includes find NormPosition", this)
    ,Y_button_moveToZero("Y.DOMOVETOZERO move to abs 0 (NormPosition)", this)
    ,Y_button_setAsNominal("Y.DOSETNOMINAL Y nominal = Y actual", this)
{
    if (!conf_done) {
	list_append();
	conf_done = 1;
    }
}


NumberedStrings* EqFctLbl_XYDevice::errorTexts(NULL);

void EqFctLbl_XYDevice::init()
{
    set_error ( no_error );

    theMICOS_Controller = NULL;

    X_mmAct_hist_.book_hist (arch, name_.value(), ring_buffer );
    Y_mmAct_hist_.book_hist (arch, name_.value(), ring_buffer );
    
    if(!errorTexts) {
        errorTexts = new NumberedStrings(NULL, 3); // will be set after Drives have been created
	errorTexts->Append(errIllCmd, "illegal cmd number");
	errorTexts->Append(errCommandPending, "operation in progress");
	errorTexts->Append(errDeviceOffline, "server is offline");
    }
    ActPosKnown(motorPositionsKnown.value()==1);
    g_sts_.online (0);	// force an initHardware(); at the first update()
}

void EqFctLbl_XYDevice::CreateDrives() {
    X_drive = new Drive(this, theMICOS_Controller, 
    	X_motorNumber.value(), X_diode_bit.value(), X_text_str,
        &X_Nominal, &X_stepsActAbs, &X_lastValidSteps, &X_stepsActNominal, &X_mmActAbs, &X_mmActNominal,
	&X_mmLowLimit, &X_mmUpLimit,
    	&X_stepsTo, &X_stepsBy, &X_mmTo, &X_mmBy, &X_destinationReachable, &X_stepsMax, &X_stepsMaxMax, 
	&X_stepsNormPosition, X_axisFactor.value(), X_diode_dark_at0.value()!=0);
    Y_drive = new Drive(this, theMICOS_Controller, 
    	Y_motorNumber.value(), Y_diode_bit.value(), Y_text_str,
        &Y_Nominal, &Y_stepsActAbs, &Y_lastValidSteps, &Y_stepsActNominal, &Y_mmActAbs, &Y_mmActNominal,
	&Y_mmLowLimit, &Y_mmUpLimit,
    	&Y_stepsTo, &Y_stepsBy, &Y_mmTo, &Y_mmBy, &Y_destinationReachable, &Y_stepsMax, &Y_stepsMaxMax,
	&Y_stepsNormPosition, Y_axisFactor.value(), Y_diode_dark_at0.value()!=0);
	
        getOrSet_X.SetHalf(X_drive);
        getOrSet_Y.SetHalf(Y_drive);
	
        //??
        errorTexts->ChangeBaseSet(X_drive->ErrorTexts());
}

void EqFctLbl_XYDevice::initHardware()
{
    cerr << timePoint().str(timePoint::date) << "going to initHardware() " << fct_name() << endl;
    X_drive->Init();
    Y_drive->Init();
    int initCommand = 0;
    int autoI = autoInit.value() & 1;
    if(autoI) {
    	if(!X_drive->PositionOK()) initCommand |= cmdS_X | cmd_zeroPosition;
    	if(!Y_drive->PositionOK()) initCommand |= cmdS_Y | cmd_zeroPosition;
    }
    autoInit.set_value(autoI); // strip off all the other bits
    if(initCommand) PerformCommand(initCommand, NULL); // force center finding
    cerr << "  EqFctLbl_XYDevice::initHardware() done >" << fct_name() << "<, initCommand=0x" << hex << initCommand << dec << endl;
}

void Drive::Init() {
	stepperMotor->Init();
}

NumberedStrings* Drive::errorTexts(NULL);
Drive::Drive(EqFctLbl_XYDevice* theMirror, MICOS_Controller* theMICOS, 
        int theMotorNumber, int diodesBit, const char* the_text_str,
        D_int* theNominal, D_int* theStepsActAbs, D_int* theLastValid, D_int* theStepsActNominal, 
	D_float* the_mmActAbs, D_float* the_mmActNominal,
	D_float* the_mmLowLimit, D_float* the_mmUpLimit,
    	D_int_quad* the_stepsTo, D_int_quad* the_stepsBy, D_float_quad* the_mmTo, 
	D_float_quad* the_mmBy, D_int* theDestinationOK, D_int* theStepsMax, D_int* theStepsMaxMax, D_int* thestepsNormPosition, 
	int the_axisFactor, bool the_diode_dark_at0) :
    mirror(theMirror), _text_str(the_text_str),
    nominal(theNominal), stepsActAbs(theStepsActAbs), stepsActNominal(theStepsActNominal),lastValidSteps(theLastValid),
    mmActAbs(the_mmActAbs), mmActNominal(the_mmActNominal),
    mmLowLimit(the_mmLowLimit), mmUpLimit(the_mmUpLimit),
    stepsTo(the_stepsTo), stepsBy(the_stepsBy), mmTo(the_mmTo), mmBy(the_mmBy),
    stepsMax(theStepsMax), stepsMaxMax(theStepsMaxMax),stepsNormPosition(thestepsNormPosition),destinationOK(theDestinationOK),
    diode_dark_at0(the_diode_dark_at0),axisFactor(the_axisFactor),diodesBitPattern(diodesBit)
{
    stepsTo->SetHalf(this);
    stepsBy->SetHalf(this);
    mmTo->SetHalf(this);
    mmBy->SetHalf(this);
    stepperMotor = new StepperM_MICOS(theMICOS, theMotorNumber);
    stepperMotor->SetPositionKnown(theMirror->motorPositionsKnown.value()==1,
    				   stepsNormPosition->value()+stepsActAbs->value());
    theMICOS->AppendMotor(stepperMotor);

    if(!errorTexts) {
	errorTexts = new NumberedStrings(stepperMotor->ErrorTexts());
	errorTexts->Append(errBadDest, "illegal value of destination");
	errorTexts->Append(errDiodeDark, "diode seems to see permanently DARK");
	errorTexts->Append(errDiodeLight, "diode seems to see permanently LIGHT");
	errorTexts->Append(errDiodeBlind, "diode seems to be blind (wrong connected)");
	errorTexts->Append(errMinusEndSwitch, "- end switch not reached, call an expert");
	errorTexts->Append(errPlusEndSwitch, "+ end switch not reached, call an expert");
	errorTexts->Append(ErrWrongProgramFlow, "internal error - Drive:ErrWrongProgramFlow");
 	errorTexts->Append(stepperMotor->ErrCabel, "cable test failed");
 	errorTexts->Append(stepperMotor->ErrInternal, "internal error of the MICOS controler firmware");
   }
}

void D_float_quad::set_value(float val) {
    D_float::set_value(val);
    half->AdjustQuad(this);
}

void D_int_quad::set_value(int val) {
    D_int::set_value(val);
    half->AdjustQuad(this);
}

void Drive::AdjustQuad(D_int_quad* an_int_quad) {
// stepsTo or stepsBy changed
    if(an_int_quad != stepsTo) {
    	// stepsBy did change, stepsTo have to be re-calculated
	stepsTo->D_int::set_value(stepsActNominal->value() + stepsBy->value());
    } else {
        // stepsTo did change, stepsBy have to be re-calculated
	stepsBy->D_int::set_value(stepsTo->value() - stepsActNominal->value());
    }
    // calc mm on target
    mmTo->D_float::set_value(Calc_mm(stepsTo->value()));
    mmBy->D_float::set_value(mmTo->value() - mmActNominal->value());
    CheckIfTargetCanBeReached();
}

const char* Drive::DriveText() {
    return _text_str;
}

bool Drive::PositionOK() {
    smStatus stat;
    int position;
    stat = stepperMotor->GetSteps(position);
    if(stat != 0) return false;		// can't read
    int lastValidMotorSteps = stepsActAbs->value() + stepsNormPosition->value();
    bool isOK = lastValidMotorSteps == position;
    cerr << DriveText() << "::PositionOK() awaited:" << lastValidMotorSteps << ", real:" << position << "  ";
    cerr << endl;
    return isOK; // allow upto 10 steps difference
}

float Drive::Calc_mm(int steps) {
// calc the offset on the target if the motor is moved by "steps" steps
    float mm = float(steps) / axisFactor;
    return mm;
}

int Drive::Calc_Steps(float mm) {
// calc the number of steps necessary to make an offset of "mm" mm on the target
    int steps = mm * axisFactor;   
    // axisFactor includes: mirror geometry, motor orientation,
    // it is about 1000000 / 40 for the mirrors
    // +/-1000000 for all other motors
    return steps;
}

void Drive::CheckIfTargetCanBeReached() {
    int motorSteps = stepsTo->value() + nominal->value() + stepsNormPosition->value();
    bool canBeReached = motorSteps >= 0;
    canBeReached &= motorSteps <= stepperMotor->GetMaxSteps();
    /* cerr << "Drive::CheckIfTargetCanBeReached() stepsTo = " << stepsTo->value()
    << ", nominal = " << nominal->value()
    << ", stepsNormPosition = " << stepsNormPosition->value()
    << ", maxSteps = " << stepperMotor->GetMaxSteps() 
    << ", canBeReached: " << (motorSteps >= 0) 
    << ", canBeReached: " << canBeReached << endl; */
    destinationOK->set_value(canBeReached ? 1:0);
}

void Drive::AdjustQuad(D_float_quad* a_float_quad) {
// mmTo or mmBy changed
    float mm_To;
    if(a_float_quad != mmTo) {
    	// mmBy did change, mmTo have to be re-calculated
	mm_To = mmActNominal->value() + mmBy->value();
        mmTo->D_float::set_value(mm_To);
    } else {
        // mmTo did change (or mmActNominal changed), mmBy have to be re-calculated
	mm_To = mmTo->value();
        mmBy->D_float::set_value(mm_To - mmActNominal->value());
    }
    // calc steps
    stepsTo->D_int::set_value( Calc_Steps(mmTo->value()) );
    stepsBy->D_int::set_value(stepsTo->value() - stepsActNominal->value());
    CheckIfTargetCanBeReached();
}

void Drive::update() {
    int motorSteps;
    smStatus stat = stepperMotor->GetSteps(motorSteps);
    //if(stat) 
    	//cerr << "Drive::update() name=<" << _text_str << ">, MICOS_Addr=" << stepperMotor->itsMICOS_Addr << ", stat = " << stat << endl;
    if(stat >= 0) {
	int steps = motorSteps - stepsNormPosition->value();
	stepsActAbs->set_value(steps);
	stepsActNominal->set_value(steps - nominal->value());
	mmActAbs->set_value(Calc_mm(steps));
	mmActNominal->set_value(Calc_mm(stepsActNominal->value()));
    }
}

smStatus Drive::SetAsNominal() {  // set current position as the new nominal
    nominal->set_value(stepsActAbs->value());
    stepsActNominal->set_value(0);
    mmActAbs->set_value(Calc_mm(stepsActAbs->value()));
    mmActNominal->set_value(0.);
    AdjustQuad(mmBy); // keep mmBy constant, recalc mmTo, stepsBy, stepsTo
    UpdateLimits();
    return 0;
}

void Drive::UpdateLimits() {
    int nominalPlusNormal = nominal->value() + stepsNormPosition->value();
    float  limit1 = Calc_mm(-nominalPlusNormal);
    float limit2 = Calc_mm(stepsMax->value() - nominalPlusNormal);
    if(limit1 <= limit2) {
	mmLowLimit->set_value( limit1);
	mmUpLimit->set_value( limit2);
    } else {
 	mmLowLimit->set_value( limit2);
	mmUpLimit->set_value( limit1);
   }
}

smStatus Drive::MoveTo(int  moveTo_rel_steps, bool fixedStepsBy) {
    //cerr << "Drive::MoveTo -" <<DriveText() << "- moveTo_rel_steps=" << moveTo_rel_steps << endl;
    smStatus stat;
    int position;
    switch(ContinueAt()) {
        case 10:
	case 0: {
	    int moveTo_steps = moveTo_rel_steps + stepsNormPosition->value();
	    if(moveTo_steps < 0) return StepperMotor::ErrIllParam;
	    //cerr << "Drive::MoveTo -" <<DriveText() << "- moveTo_steps=" << moveTo_steps << endl;
	    int stepsNow = stepsActAbs->value() + stepsNormPosition->value();
	    if(moveTo_steps >= stepsNow) {
		ContinueAt(12);
		if(moveTo_steps != stepsNow) {
		    stat = stepperMotor->GoToStep(moveTo_steps);
		    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
		}
		goto case2;
	    }
	    // last step must be always into +direction, since this one is into -direction, 
	    // we first perform a move to about 10 um below the final target
	    ContinueAt(11);
	    actStepWidth = min(moveTo_steps, (mirror->hysteresis).value()); // never move below 0
	    stat = stepperMotor->GoToStep(moveTo_steps - actStepWidth);
	    actStepWidth = moveTo_steps;
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	}
	case 11:
	    stat = stepperMotor->GetStatus();
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	    stat = stepperMotor->GoToStep(actStepWidth);
	    ContinueAt(12);
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	case 12:
	case2:
	    stat = stepperMotor->GetStatus();
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	    stepperMotor->GetSteps(position);
	    //cerr << "Drive::MoveTo -" <<DriveText() << "- new position=" << position << endl;
	    update();
	    lastValidSteps->set_value(stepsActAbs->value());
	    if(fixedStepsBy) {
	        AdjustQuad(stepsBy); // hold stepsBy fixed, adjust the others
	    } else {
	        AdjustQuad(stepsTo); // hold stepsTo fixed, adjust the others
	    }
	    ContinueAt(13);
	    return stat;
	case 13:	// 
	    return 0; // we already are ready (but the other axis is still busy)
	default:
	    return ErrWrongProgramFlow;
    }
}

smStatus Drive::Stop() {
    switch(ContinueAt()) {
        case 0:
	    ContinueAt(1);
	    return stepperMotor->Stop();
	case 1:
	    return  stepperMotor->GetStatus();
	default:
	    return ErrWrongProgramFlow;
    }
}

/** find the NormPostion (the position of the limit of the light barrier in the middle of the 
    move-area) for a Lbl_XYDevice, a Lbl_XYZ_Device or a Lbl_Mirror.
    It sequentialises the movement of the 2 (3) axis, when necessary. 
*/
smStatus Drive::FindNormPosition(int& pending, int this_now) {
    smStatus stat;
    int position;
    int lMinus, lPlus;
    //cerr << "Drive::FindNormPosition cont at " << ContinueAt() << ", pending = 0x" << hex << pending 
    //<< ", this_now = 0x" << this_now << dec << endl;
    switch(ContinueAt()) {
        case 0:
	    // check if the motor is cabled
	    stat = stepperMotor->CheckCable();
	    if(stat!=StepperMotor::Ready) return stat;	// error - cable problem

	    
	    /* stat = stepperMotor->SetParameter("-1000.0 1000.0 setnlimit", ""); // reset the -/+ limits of the motor
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    cerr << DriveText() << "-1000.0 1000.0 setnlimit performed" << endl;
	    char text[100];
	    stat = stepperMotor->GetParameter("getnlimit", text);
	    cerr << "getnlimit <" << text << ">" << endl,
	    */
	    /* nreset seems to be problematic -- blocks the bus??
	    stepperMotor->SetParameter("nreset", ""); // perform a reset, this also resets the -/+ limits of the motor
	    timeOutTime = timePoint() + timeInterval(2.5);	// wait > 2.5 sec after reset
	    ContinueAt(1);
	    
	case 1:
	    if(timePoint() < timeOutTime) return StepperMotor::Busy; 	// keep waiting
	    cerr << DriveText() << "   Drive::FindNormPosition() -1-" << endl;
	    */
	    // go to -initiator, do this by a motor command "-?? nm" which is just a bit greater then the
	    // moving range of the stage. This (instead of "ncal") avoids moving 1000 mm in the case that the movement
	    // will be blocked by accident (by a short cable, the lead wall, etc.)
	    
	    
	    /*stat = stepperMotor->GoToStep(-stepsMaxMax->value());
	    cerr << DriveText() << "GoToStep(-stepsMaxMax->value() performed" << endl;
	    ContinueAt(2);*/
	    
	    stat = stepperMotor->GoToInitiator(-1);
	    //cerr << DriveText() << "GoToInitiator(-1)  (ncal) performed" << endl;
	    ContinueAt(3);
	    return stat ? stat : StepperMotor::Busy;
	    // no break
	    
	case 2:
	//?? case 2 not used at all
	    stat = stepperMotor->GetStatus();
	    cerr << DriveText() << " stat = " << stat << endl;
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	    
	    cerr << DriveText() << " - arrived at -limit?" << endl;
    	    stat = stepperMotor->GetLimitSwitches(lMinus, lPlus);
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    cerr << "lMinus=" << lMinus << ", lPlus=" << lPlus << endl;
	    if( !((lMinus == 1) && (lPlus == 0)) ) return errMinusEndSwitch;

	    cerr << DriveText() << " - arrived at -limit" << endl;
	    stat = stepperMotor->GoToInitiator(-1);
	    ContinueAt(3);
	    return stat ? stat : StepperMotor::Busy;
	    // no break
	    
	case 3:	{
	    stat = stepperMotor->GetStatus();
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	    //cerr << DriveText() << " -ncal done" << endl;
	    stat = stepperMotor->GetSteps(position);
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    if(position < 0) {
		cerr << DriveText() << " --> -limit switch not reached by ncal, motor position = " << position << endl;
	    	return errMinusEndSwitch;
	    }
	    bool hereIsLight = (unidig_lbl->Read() & diodesBitPattern) == 0;
	    //cerr << DriveText() << "- hereIsLight = " << hereIsLight << endl;
	    if(diode_dark_at0) { // darkness awaited
	        if(hereIsLight) { // diode sees light - should be dark
	            cerr << DriveText() << " --> diode at limit switch- sees light, should be dark --> errDiodeLight" << endl;
		    return errDiodeLight;
		}
	    } else { // light awaited
	        if(!hereIsLight) { // diode sees dark
	            cerr << DriveText() << " --> diode at limit switch- sees no light, but should --> errDiodeDark" << endl;
		    return errDiodeDark;
		}
	    }
	    
	    // situation at -limit is OK, now go to +limit
	    stat = stepperMotor->GoToInitiator(1);
	    //cerr << DriveText() << "GoToInitiator(+1) performed" << endl;
	    //stat = stepperMotor->GoToStep(stepsMaxMax->value());
	    ContinueAt(4);
	    return stat ? stat : StepperMotor::Busy;
	    // no break
	}
	case 4:	{
	    stat = stepperMotor->GetStatus();
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	    
	    //cerr << DriveText() << " +limit reached" << endl;
	    stat = stepperMotor->GetSteps(position);
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    if( (position > stepsMaxMax->value()) || (position <=0) ) {
		cerr << DriveText() << " --> +limit switch not reached by nrm, motor position = " << position << endl;
	    	return errPlusEndSwitch;	// ---- ?? ----
	    }
    	    /*stat = stepperMotor->GetLimitSwitches(lMinus, lPlus);
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    cerr << "lMinus=" << lMinus << ", lPlus=" << lPlus << endl;
	    if( !((lMinus == 0) && (lPlus == 1)) ) return errPlusEndSwitch;
	    
	    // ?? exact range meassurement seems not to be needed
	    stat = stepperMotor->GetSteps(position);
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    */
	    stepperMotor->SetMaxSteps(position);
	    stepsMax->set_value(position);
	    
	    bool hereIsLight = (unidig_lbl->Read() & diodesBitPattern) == 0;
	    //cerr << DriveText() << "+ hereIsLight = " << hereIsLight << endl;
	    if(diode_dark_at0) { // dark_at0 -- means light awaited here
	        if(!hereIsLight) { // but diode sees dark
	            cerr << DriveText() << " --> diode at limit switch+ sees no light, but should --> errDiodeDark" << endl;
		    return errDiodeDark;
		}
	    } else { // darkness awaited here
	        if(hereIsLight) { // diode sees light - should be dark
	            cerr << DriveText() << " --> diode at limit switch+ sees light, should be dark -->> errDiodeLight" << endl;
		    return errDiodeLight;
		}
	    }
	    
	    pending &= ~this_now; // clear this_now to message: "this axis ready at +limit"
	    //cerr << DriveText() << " this axis ready at +limit" << endl;
	    ContinueAt(6);
	    // no break
	}
	//case 5:	    
	    if((pending & (cmdS_X_now | cmdS_Y_now | cmdS_Z_now)) != 0) 
	    	// do not continue with this axis until both (or all 3, resp.) axises are at +limit
		return StepperMotor::Busy;
		
	    pending |= cmdS_X_now; // continue with one axis after the other
	    //cerr << DriveText() << " all axises at +limit, start to sway left and right" << endl;
	    //ContinueAt(6);
	    return StepperMotor::Busy;
	     
	case 6:	 {   
	    int stepsMaxNow = stepsMax->value();
	    int stepNow;
	    cerr << DriveText() << " start to sway, pending = 0x" << hex << pending << dec 
	    	 << ", stepsMaxNow = " << stepsMaxNow << endl;
	    if(stepsNormPosition->value() <= 0) { // NormPosition unknown
		stepNow = stepsMaxNow / 2;
	        actStepWidth = -stepsMaxNow / 4;
		cerr << DriveText() << " - NormPosition UNKNOWN";
	    } else { // NormPosition known
	    	/*actStepWidth = -mirror->rawStartSteps.value();	
	    	// big enough to be on the save side, in case the middle position is roughly known
	    	// alternatives: 65536,131072, 262144, 524288, 1048576
	    	*/
		actStepWidth = -mirror->minResolution.value() * 16;	
		stepNow = stepsNormPosition->value();
		stepNow += actStepWidth/2;
		cerr << DriveText() << " - NormPosition known";
	    }	    
	    cerr << ", actStepWidth = " << actStepWidth << ", GoToStep(" << stepNow << ")" << endl;
	    stat = stepperMotor->GoToStep(stepNow);
	    ContinueAt(7);
	    // no break
	}    
	case 7: {
	goActStepsWidth:
	    stat = stepperMotor->GetStatus();
	    //cerr << " stat=" << stat;
	    if(stat!=StepperMotor::Ready) return stat;	// Busy or error
	    //cerr << endl;
	    stat = stepperMotor->GetSteps(position);
	    if(stat!=StepperMotor::Ready) return stat;	// error
	    cerr << "position = " << position;
	    bool hereIsLight = (unidig_lbl->Read() & diodesBitPattern) == 0;
	    if(hereIsLight != diode_dark_at0) { // -- too close to -limit
	    	// go into +direction
		cerr << ", too close to -limit ";
		if(actStepWidth < 0) { 
		    // last step was into -direction, illumination changed
		    actStepWidth = -actStepWidth;	// change direction of move, next step will be into +direction
		    //actStepWidth = actStepWidth >> 3;	// reduce the step width by a factor of 8
		    actStepWidth = actStepWidth >> 2;	// reduce the step width by a factor of 4
		    if(actStepWidth < mirror->minResolution.value()) {     // minimal stepwidth reached, we found the point
			actStepWidth = mirror->minResolution.value(); // make still 1 move to reach a position at the other side of the light barrier
		    } 
		} else {
			// last step was into +direction,  illumination did not change,
			// make one more step into +direction
		}
	    } else { // -- too far away from -limit
		cerr << ", too far away from to -limit ";
		    if(actStepWidth > 0) { // last step was into +direction,  illumination changed

			if(actStepWidth <= mirror->minResolution.value()) { 
		            // minimal stepwidth reached, we found the point
			    //                            ==================
			    stat = stepperMotor->GetSteps(position);
			    cerr << endl << "Drive::FindNormPosition()" << DriveText() 
				 << " -+- found at steps = " << position << endl;
			    stepsNormPosition->set_value(position);
			    stepsActAbs->set_value(0);
			    update();
	    		    AdjustQuad(stepsTo);
			    UpdateLimits();
			    if(pending & cmdS_X_now) {
			    	pending |= cmdS_Y_now;
			    } else if(pending & cmdS_Y_now) {
			    	if(pending & cmdS_Z) pending |= cmdS_Z_now;
			    }
			    pending &= ~this_now; // ready with this axis
			    ContinueAt(10);	// this' axis thread will contininue with MoveTo than
			    if(pending & (cmdS_Y_now | cmdS_Z_now)) {
			    	// still to find the light barrier on an other axis
				// this will ContinueAt(6), as set above in case 4
			    } else {
			    	// all 2 (or 3) axises have found their respective middle positions
				// now go to move all axises to their respective last valid positions
		 		pending |= cmdS_X_now | cmdS_Y_now;          // move x-axis, y-axis
		 		if(pending & cmdS_Z) pending |= cmdS_Z_now;  // move z-axis too (if it exists)
			    }
			    return StepperMotor::Busy;
			  
			} 
			actStepWidth = -actStepWidth;   // change the direction, go back into -direction
		    	//actStepWidth = actStepWidth >> 3;	// reduce the step width by a factor of 8
		    } else { // last step was into -direction,  illumination did not changed, make an other step into -direction
			    stat = stepperMotor->GetSteps(position);
	    		    if(stat!=StepperMotor::Ready) return stat;	// error
			    if(position <= 0) return errDiodeBlind;	// reached -limit
			    if(position < -actStepWidth) actStepWidth = -position; // to avoid moving behind -0
		    }
	    }
	    cerr << "GoSteps(" << actStepWidth << ")" << endl;
	    stat = stepperMotor->GoSteps(actStepWidth);
	    goto goActStepsWidth;
	}    
	// both / all 3 axises are now at optimal middle positions (at the light-barrier-limit)
	// go to lastValidSteps
	case 10:
	case 11:
	case 12:
	    return MoveTo(lastValidSteps->value(), false);
	case 13: // we already are ready (but may be, there is still something to do  with the other axis)
	    pending &= ~this_now; // ready with this axis
	    return 0; // nothing to do with the other axis - finished
	    
	default:
	    return ErrWrongProgramFlow;
    }
}

void EqFctLbl_XYDevice::update() {

    if(!EqFctlaserBeamLineInitDone) return; // wait for all inits being finished
    
    // get a pointer to an already existing MICOS_Controller
    if(theMICOS_Controller == NULL) {
	if(controllerNumber.value() < maxNbOfControllers) theMICOS_Controller = MICOS_controllers[controllerNumber.value()];
	if(theMICOS_Controller == NULL) {
    	    cerr << "-- no MICOS_controller " << controllerNumber.value() << endl;
	    set_error ( sts_offline );
	    g_sts_.online (0);
	    return;
	}
        CreateDrives();
    }

    int error;
    int wasOnline = g_sts_.online();
    online();
    int isOnline = g_sts_.online();
    theMICOS_Controller->syncAllHardware.wait();
    if( isOnline && (!wasOnline)) { // went to online
	// forced by: -1. Reset() of the controller or 
	// -2. controller was just switched on or
	// -3. first update()-call after server start.
	initHardware();
	set_error( (int)went_online );
    } else if(wasOnline && !isOnline) { // went to offline
	commandFinished(2);
    }
    if ( isOnline ) {
	error = sts_ok;	// ... until we find different
	if(pendingCommand) {	// a command is pending
	    smStatus stat = TryReady();
    	    if(stat <= 0) commandFinished(stat);
	    if(pendingCommand) {	// command is still pending
		    error = sts_ok2;
	    }
	}
	updateAllDrives();
    } else { //	device is offline
	set_error( (int)went_offline );
	error = sts_offline;
    }
    NotifyStatus();

    theMICOS_Controller->syncAllHardware.post();
    FillHistories(error);
}

void EqFctLbl_XYDevice::updateAllDrives() {
    if(theMICOS_Controller->getDoUpdateAllDrives()) 
    {	// do NOT perform the update when an other motor command is "on the way"
    	// since that will cause problems with the controller 
    	X_drive->update();
    	Y_drive->update();
    }
}

void EqFctLbl_XYDevice::FillHistories(int error) {
    TDS	history;
    history.tm = walltime;
    history.status = error;
    #define FillTheHistory(hist, val) history.data = val; hist.fill_hist(&history);
    FillTheHistory(X_mmAct_hist_, X_mmActNominal.value());
    FillTheHistory(Y_mmAct_hist_, Y_mmActNominal.value());
}

smStatus EqFctLbl_XYDevice::SetAsNominal(int val) {
    smStatus stat = 0;
    smStatus stat2 = 0;
    if(val & cmdS_X) stat = X_drive->SetAsNominal();
    if(val & cmdS_Y)  stat2 = Y_drive->SetAsNominal();
    if((stat == 0) || (stat2 < 0)) stat = stat2;
    return stat;
}

void EqFctLbl_XYDevice::NotifyStatus() {	
    smStatus stat = NotifyAxisStatus(0, X_status, (X_destinationReachable.value()==1), &X_errNb, &X_errTxt);
    stat = NotifyAxisStatus(stat, Y_status, (Y_destinationReachable.value()==1), &Y_errNb, &Y_errTxt);
    NotifyOverallStatus(stat);
}

smStatus EqFctLbl_XYDevice::NotifyAxisStatus(smStatus mostSignificantErr, smStatus thisErr,
				bool destinationReachable, D_int* errNb, D_string* errTxt) {
				
    if((thisErr==0) && (!destinationReachable)) thisErr = 3;
    errNb->set_value(thisErr);
    switch(thisErr) {
	case 0:	errTxt->copy_value("ready");
		break;
	case 1:	errTxt->copy_value("busy");
		break;
	case 2:	errTxt->copy_value("position unknown");
		break;
	case 3:	errTxt->copy_value("destination not reachable");
		break;
	default: {
		char * theErrText = errorTexts->Get(thisErr);
		if (theErrText) {
			errTxt->copy_value(theErrText);
		} else {
			char buf[STRING_LENGTH];			
			sprintf(buf, "Unclassified error %d", thisErr);
			errTxt->copy_value(buf);
		}
	}
    }
    // find out the "more significant" of thisErr and mostSignificantErr
    // to present this as the device status
    smStatus stat = mostSignificantErr;
    if(stat == 0) {
    	stat = thisErr;
    } else {
    	stat = min(thisErr, mostSignificantErr);
    }
    //smStatus stat = min(thisErr, mostSignificantErr);	// errors ?
    //if(stat==0) stat = max(thisErr, mostSignificantErr);	// if(no errors) not ready ?
    return stat;
}

void EqFctLbl_XYDevice::NotifyOverallStatus(smStatus stat) {    
    status.set_value(stat);
    int optDeviceNum = opticalDeviceNumber.value();
    statusOfDevice[optDeviceNum-1] = stat;
    // find an error text allocated to the status
    switch(stat) {
	case 0:	errMover.copy_value("Device ready.");
		ActPosKnown(true);
		break;
	case 1: {			
		char buf[STRING_LENGTH];
		if(pendingCommand == cmd_resetController) {
		    sprintf(buf, "Initialising.");
		} else {
		    ActPosKnown(false);
		    sprintf(buf, "Device moving.");
		}
		errMover.copy_value(buf);
		break;
		}
	case 2:	ActPosKnown(false);
		errMover.copy_value("Device at rest. Position unknown.");
		break;
	case 3:	errMover.copy_value("Device at rest. Destination position not reachable.");
		break;
	default: {
		char * theErrText = errorTexts->Get(stat);
		if (theErrText) {
			errMover.copy_value(theErrText);
		} else {
		    /* theErrText = ( (thelaserBeamLine->GetMICOS_Controller())->ErrorTexts() )->Get(stat);
		    if (theErrText) {
			errMover.copy_value(theErrText);
		    } else { */
			char buf[STRING_LENGTH];			
			sprintf(buf, "Unclassified error %d", stat);
			errMover.copy_value(buf);
		    /* } */
		}
	}
    }
}

void EqFctLbl_XYDevice::online()        // called by init() and RPC "set online"
{
    int	error = 0;
    // do some online/alive test
    smStatus stat = X_drive->stepperMotor->ItsController()->GetStatus();
    //X_status = Y_status = stat;
    if (stat != 0) {
	// controller not ready (switched off or just reseting)
	error = 1;
    	X_status = Y_status = stat;
    }
    if (!error) {	// test successful
	g_sts_.error (0);
	g_sts_.newerror (0);
	g_sts_.online (1);
    } else {
	g_sts_.online (0);	// error handling
    }
};

void EqFctLbl_XYDevice::ActPosKnown(bool isKnown) {
    int optDeviceNum = opticalDeviceNumber.value();
    actPosOfDeviceIsKnown[optDeviceNum-1] = isKnown;
    motorPositionsKnown.set_value(isKnown ? 1 : -1);
}

D_int_cmd_(D_int_cmd_moveBy);
D_int_cmd_(D_int_cmd_moveToNominal);
D_int_cmd_(D_int_cmd_stop);
D_int_cmd_(D_int_cmd_resetController);
D_int_cmd_(D_int_cmd_setAsNominal);

D_int_cmd_(D_int_cmd_X_move);
D_int_cmd_(D_int_cmd_X_moveToNominal);
D_int_cmd_(D_int_cmd_X_stop);
D_int_cmd_(D_int_cmd_X_zeroPosition);
D_int_cmd_(D_int_cmd_X_init);
D_int_cmd_(D_int_cmd_X_moveToZero);
D_int_cmd_(D_int_cmd_X_setAsNominal);

D_int_cmd_(D_int_cmd_Y_move);
D_int_cmd_(D_int_cmd_Y_moveToNominal);
D_int_cmd_(D_int_cmd_Y_stop);
D_int_cmd_(D_int_cmd_Y_zeroPosition);
D_int_cmd_(D_int_cmd_Y_init);
D_int_cmd_(D_int_cmd_Y_moveToZero);
D_int_cmd_(D_int_cmd_Y_setAsNominal);

//-----------------------------------------------------------------------------------

void EqFctLbl_XYDevice::PerformCommand(int val, D_int * itsButton) {
    // perform a movement (start)

    smStatus stat = 0;
    if(pendingCommand != 0) {
	// a previous command is in progress ...
        if(val & cmdS_terminates_others) {
	    // ... but this (new) command terminates the previous
	    commandFinished(2);
        } else {
	    // this command is not allowed to terminate the previous
	    if(itsButton) itsButton->D_int::set_value(0);  // unpress botton
	    return;
	}
    }
    pendingCommand = val;	// set the new pending command
    pendingButton = itsButton;
    stat = PerformSeletedCmd(val);
    if(stat <= 0) commandFinished(stat);
}

smStatus EqFctLbl_XYDevice::PerformSeletedCmd(int val) {
    smStatus stat = 0;
    X_drive->ContinueAt(0);
    Y_drive->ContinueAt(0);
    int baseCmd = val & cmdMask_baseCmds;
    switch(baseCmd) {
        case cmd_init:  // init both motors (always both!)
		X_drive->stepsNormPosition->set_value(-1);
		Y_drive->stepsNormPosition->set_value(-1);
		cerr << timePoint().str() << "going to perform cmd_init for " << fct_name() << endl;
        case cmd_zeroPosition:  // re-adjust both motors (always both!)
		pendingCommand |= cmdS_X | cmdS_Y | cmdS_X_now | cmdS_Y_now;
        case cmd_move:  // move (by) one or both motors
        case cmd_moveToNominal:  // move to nominal one or both motors
        case cmd_stop:  // stop one or both motors
        case cmd_resetController:  // reset controller of motors
        case cmd_moveToZero:  // move to abs 0 (NormPosition)
	    stat = 1; // busy
	    status.set_value(stat);
	    ActPosKnown(false);
	    break;
        case cmd_setAsNominal:  { // set current position as the new nominal, one motor 
	    SetAsNominal(val);
	    break;
	}
	default:
	    stat = errIllCmd;	// illegal command number
    }
    return stat;
}

smStatus EqFctLbl_XYDevice::TryReady() {
    // perform a movement (continue)
    smStatus stat = 0;
    smStatus stat2 = 0;
    IFDEBUG(D_TryReady) 
	cerr << "EqFctLbl_XYDevice::TryReady() comm= 0x" << hex << pendingCommand << dec << endl;
    ENDDEBUG
    int baseCmd = pendingCommand & cmdMask_baseCmds;
    switch(baseCmd) {
        case cmd_move:  // move (by) one or both motors
	    if(pendingCommand & cmdS_X) X_status = stat = X_drive->MoveTo(X_stepsActAbs.value() + X_stepsBy.value());
	    if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->MoveTo(Y_stepsActAbs.value() + Y_stepsBy.value());
	    break;
        case cmd_moveToNominal:  // move to nominal one or both motors
	    if(pendingCommand & cmdS_X) X_status = stat = X_drive->MoveTo(X_Nominal.value());
	    if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->MoveTo(Y_Nominal.value());
	    break;
        case cmd_moveToZero:  // move to abs 0 (NormPosition)
	    if(pendingCommand & cmdS_X) X_status = stat = X_drive->MoveTo(0);
	    if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->MoveTo(0);
	    break;
        case cmd_stop:  // stop one or both motors
	    if(pendingCommand & cmdS_X) X_status = stat = X_drive->Stop();
	    if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->Stop();
	    break;
        case cmd_resetController:  { // reset controller of motors
	    int caseToContinue = X_drive->ContinueAt();
	    stat = X_drive->stepperMotor->ItsController()->Reset(caseToContinue);
	    X_drive->ContinueAt(caseToContinue);
	    break;
	    }
        case cmd_init:  
        case cmd_zeroPosition:  // move one motor to NormPosition, using the state of the photo diode
	    if(pendingCommand & cmdS_X_now) {
		X_status = stat = X_drive->FindNormPosition(pendingCommand, cmdS_X_now);
	    }
	    if(pendingCommand & cmdS_Y_now) {
		Y_status = stat2 = Y_drive->FindNormPosition(pendingCommand, cmdS_Y_now);
	    }
	    break;
	default:
	    stat = errIllCmd;	// illegal command number
    }
    if((stat == 0) || (stat2 < 0)) stat = stat2;
    //??if(stat <= 0) commandFinished(stat);
    return stat;
}

void EqFctLbl_XYDevice::commandFinished(smStatus stat) {
    if(pendingButton != NULL) {
	pendingButton->D_int::set_value(0);	// unpress botton
	pendingButton = NULL;
	IFDEBUG(D_TryReady) 
	    cerr << "EqFctLbl_XYDevice::commandFinished() comm= 0x" << hex << pendingCommand << dec;
	    switch(stat) {
		case 0:
		    cerr << " done." << endl;
		    break;
		case 1:
		    cerr << " terminated by user." << endl;
		    break;
		default:
		    cerr << " terminated due to error " << stat << endl;
		    break;
	    }
	ENDDEBUG
	if(stat < 0) {
    	    int baseCmd = pendingCommand & cmdMask_baseCmds;
	    cerr << "Command 0x" << hex << pendingCommand << dec 
		<< " terminated due to error " << stat <<endl;
	    char buf[STRING_LENGTH];			
	    char * theErrText = errorTexts->Get(stat);
	    if (!theErrText) {
		sprintf(buf, "Unclassified error %d", stat);
		theErrText = buf;
	    }
	    cerr << "Command " << cmdAsText[baseCmd-1] << " (0x" << hex << pendingCommand << dec << ") for device " << fct_name()
		<< " terminated due to error " << stat <<  " \"" << theErrText << "\"" << endl;
		
	}
    }
    pendingCommand = 0;
    //X_drive->ContinueAt(-1); // forces ErrWrongProgramFlow in each of the functions ?? needed ??
    //Y_drive->ContinueAt(-1);
    NotifyStatus();
}

void DoButton::set_value(int val) {
    D_int::set_value(val);
    switch(val) {
    	case 1:
	    char ack[64];
	    //cerr << "DoButton::set_value 1(get) ack=";
	    half->stepperMotor->GetParameter(half->mirror->cmdGet.value(), ack);
	    half->mirror->cmdAck.copy_value(ack);
	    //cerr << ack << endl;
	    break;
	case 2: {
	    char parVal[64];
	    char emptyString(char(0));
	    strcpy(parVal, half->mirror->cmdSet.value());
	    char* par = strrchr(parVal, int(' '));
	    //cerr << "par=" << int(par) << endl;
	    if(par !=0) {
	    	*par = char(0);
	    	par++;
	    } else {
	    	par = &emptyString;
	    }
	    //cerr << "DoButton::set_value 2(set) val=" << parVal << ", par=" << par << endl;
	    half->stepperMotor->SetParameter(par, parVal);
	    }
	default:;
    }
}

