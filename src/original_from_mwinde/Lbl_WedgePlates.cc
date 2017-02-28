#include "Lbl_WedgePlates.h"

int	EqFctLbl_WedgePlates::wpConf_done = 0;

EqFctLbl_WedgePlates::EqFctLbl_WedgePlates() :
    EqFctLbl_XYDevice()
{
/*    if (!wpConf_done) {
	list_append();
	wpConf_done = 1;
    }
*/
};

void EqFctLbl_WedgePlates::CreateDrives() {
    X_drive = new RotatingDrive(this, theMICOS_Controller, 
    	X_motorNumber.value(), X_diode_bit.value(), X_text_str,
        &X_Nominal, &X_stepsActAbs, &X_lastValidSteps, &X_stepsActNominal, &X_mmActAbs, &X_mmActNominal,
	&X_mmLowLimit, &X_mmUpLimit,
    	&X_stepsTo, &X_stepsBy, &X_mmTo, &X_mmBy, &X_destinationReachable, &X_stepsMax, &X_stepsMaxMax, 
	&X_stepsNormPosition, X_axisFactor.value(), X_diode_dark_at0.value()!=0);
    Y_drive = new RotatingDrive(this, theMICOS_Controller, 
    	Y_motorNumber.value(), Y_diode_bit.value(), Y_text_str,
        &Y_Nominal, &Y_stepsActAbs, &Y_lastValidSteps, &Y_stepsActNominal, &Y_mmActAbs, &Y_mmActNominal,
	&Y_mmLowLimit, &Y_mmUpLimit,
    	&Y_stepsTo, &Y_stepsBy, &Y_mmTo, &Y_mmBy, &Y_destinationReachable, &Y_stepsMax, &X_stepsMaxMax,
	&Y_stepsNormPosition, Y_axisFactor.value(), Y_diode_dark_at0.value()!=0);
    //??
    errorTexts->ChangeBaseSet(X_drive->ErrorTexts());
}

void EqFctLbl_WedgePlates::initHardware() {
    int initCommand = 0;
    int autoI = autoInit.value() & 1;
    if(autoI) {
    	if(!X_drive->PositionOK()) initCommand |= cmdS_X | cmd_zeroPosition;
    	if(!Y_drive->PositionOK()) initCommand |= cmdS_Y | cmd_zeroPosition;
    }
    autoInit.set_value(autoI); // strip off all the other bits
    cerr << "EqFctLbl_WedgePlates::initHardware() initCommand = " << hex << initCommand << dec << endl;
    if(initCommand) PerformCommand(initCommand, NULL); // force center finding    
}

float RotatingDrive::Calc_mm(int steps) {
// calc the offset on the target if the motor is moved by "steps" steps
// return in dg. in the range [0.0 dg .. 360.0 dg]
    if(axisFactor < 0) steps = -steps;
    while(steps < 0) steps += stepsPerRevolution;
    steps = steps % stepsPerRevolution;  // Ok. This is in range [0, stepsPerRevolution-1]
    float dg = 360. * float(steps) / float(stepsPerRevolution);
    return dg;
}

int RotatingDrive::Calc_Steps(float mm) {
// calc the number of steps necessary to make a rotation of "mm" dg.
    int steps = (float(mm)/360.) * float(stepsMax->value());
    if(axisFactor < 0) steps = -steps;
    return steps;
}

void RotatingDrive::AdjustQuad(D_float_quad* a_float_quad) {
// mmTo or mmBy changed
    float mm_To;
    if(a_float_quad != mmTo) {
    	// mmBy did change, mmTo have to be re-calculated
	mm_To = mmActNominal->value() + mmBy->value();
	if(mm_To < 0.) mm_To += 360.;
    } else {
        // mmTo did change (or mmActNominal changed), mmBy have to be re-calculated
	mm_To = mmTo->value();
    }
    int steps = Calc_Steps(mm_To);
    mmTo->D_float::set_value(mm_To);
    float mm_by = mm_To - mmActNominal->value();
    if(mm_by > 180.) mm_by -= 360.;
    mmBy->D_float::set_value(mm_by);
    // calc steps
    stepsTo->D_int::set_value(steps);
    stepsBy->D_int::set_value(steps - stepsActNominal->value());
    CheckIfTargetCanBeReached();
}

void RotatingDrive::CheckIfTargetCanBeReached() {
    destinationOK->set_value(1); // every value can be reached
}

RotatingDrive::RotatingDrive(EqFctLbl_WedgePlates* theMirror, MICOS_Controller* theMICOS, 
	int theMotorNumber, int diodesBit, char* the_text_str,
        D_int* theNominal, D_int* theStepsActAbs, D_int* theLastValid, D_int* theStepsActNominal, 
	D_float* the_mmActAbs, D_float* the_mmActNominal,
	D_float* the_mmLowLimit, D_float* the_mmUpLimit,
    	D_int_quad* the_stepsTo, D_int_quad* the_stepsBy, D_float_quad* the_mmTo, 
	D_float_quad* the_mmBy, D_int* theDestinationOK, D_int* theStepsMax, D_int* theStepsMaxMax, D_int* thestepsNormPosition, 
	float the_axisFactor, bool the_diode_dark_at0) :
	
	Drive(theMirror, theMICOS, 
	theMotorNumber, diodesBit, the_text_str,
        theNominal, theStepsActAbs, theLastValid, theStepsActNominal, 
	the_mmActAbs, the_mmActNominal,
	the_mmLowLimit, the_mmUpLimit,
    	the_stepsTo, the_stepsBy, the_mmTo, 
	the_mmBy, theDestinationOK, theStepsMax, theStepsMaxMax, thestepsNormPosition, the_axisFactor, the_diode_dark_at0)
{
    //stepsPerRevolution = theStepsMax->value();	//?? deleted -- too dangerous
    stepsPerRevolution = 360000000;	//?? not really elegant
}	

smStatus RotatingDrive::FindNormPosition(int& pending, int this_now) {
    switch(ContinueAt()) {
        case 0:
            stepperMotor->SetParameter("setsw", "2 1"); // disable +end switch - can be flashed
            stepperMotor->SetParameter("setpitch", "2.0"); // setpitch - can be flashed
 	    stepperMotor->SetParameter("setsw", "1 0"); // enable -end switch
	    break;
	case 10:
	    stepperMotor->SetMaxSteps(stepsMax->value()*2); // allow to make more then 1 revolution
            stepperMotor->SetParameter("setsw", "2 0"); // disable -end switch
	default:
	    ;
    }
    return Drive::FindNormPosition(pending, this_now);
}

smStatus RotatingDrive::MoveTo(int  moveTo_rel_steps, bool fixedStepsBy) {
    switch(ContinueAt()) {
        case 10:
	case 0: {
	    // calc (minimal) number of steps to go into +direction
	    int stepsRelativ = moveTo_rel_steps - stepsActAbs->value();
	    while(stepsRelativ < 0) stepsRelativ += stepsPerRevolution;
	    stepsRelativ = stepsRelativ % stepsPerRevolution;  // Ok. This is in range [0, stepsPerRevolution-1]
	    
	    int stepsNow = stepsActAbs->value() + stepsNormPosition->value();
	    int moveTo_steps = stepsRelativ + stepsNow;

	    // which way (+direction / -direction) takes less steps (so probably less time too) ?
	    int turnMinus = stepsPerRevolution - stepsRelativ + 2*(mirror->hysteresis).value();
	    if(stepsRelativ <= turnMinus) { // +direction acts quicker, but is it in motor's limits ?
	        if(moveTo_steps > stepperMotor->GetMaxSteps()) {
		    moveTo_steps -= stepsPerRevolution; // not possible, go into -direction
		} 
	    } else { // -direction acts quicker, but is it in motor's limits ?
	        if(moveTo_steps - stepsPerRevolution - (mirror->hysteresis).value() >= 0) {
		    moveTo_steps -= stepsPerRevolution; // it's possible, so go into -direction
		}
	    }
	    //cerr << "RotatingDrive::MoveTo -" <<DriveText() << "- moveTo_steps=" << moveTo_steps << endl;
	    moveTo_rel_steps = moveTo_steps - stepsNormPosition->value();
	}
	default:
	    return Drive::MoveTo(moveTo_rel_steps, fixedStepsBy);
    }
}
