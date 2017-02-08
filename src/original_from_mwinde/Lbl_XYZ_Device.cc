// file Lbl_XYZ_Device.cc

#include "Lbl_XYZ_Device.h"
#include "MICOS_Controller.h"

int	EqFctLbl_XYZ_Device::xyzConf_done = 0;

extern ArchiveFile*	arch;
extern time_t		walltime;
extern  int		ring_buffer;

EqFctLbl_XYZ_Device::EqFctLbl_XYZ_Device() :
    EqFctLbl_XYDevice()
    ,getOrSet_Z("Z.DO.CMD.GET_OR_SET perform GetParameter(1) or SetParameter(2)", this)
    
    ,Z_motorNumber("Z.MOTOR_NUMBER Z motor number (as addresses by the controller)", this)
    ,Z_diode_bit("Z.DIODE_BIT 1-bit-pattern for IP-Unidig mask", this)
    ,Z_diode_dark_at0("Z.DIODE.DARK_AT0 1: dark at zero pos, 0: light", this)
    ,Z_axisFactor("Z.AXIS_FACTOR mm (on target) from steps", this)
    ,Z_text("Z.DIRECTION_TEXT description", Z_text_str, STRING_LENGTH, this)
    ,Z_errNb("Z.ERR.NB motor status", this)
    ,Z_errTxt("Z.ERR.TXT motor status text", Z_err_str, STRING_LENGTH, this)
    ,Z_Nominal("Z.STEPS.NOMINAL steps nominal 0 minus absolut 0", this)
    ,Z_stepsActAbs("Z.STEPS.ACT.ABS actual steps relative to Z.STEPS.DIODE", this)
    ,Z_lastValidSteps("Z.STEPS.ACT.VALID steps after last valid move", this)
    ,Z_stepsActNominal("Z.STEPS.ACT.NOMINAL actual steps relativ to nominal 0", this)
    ,Z_mmActAbs("Z.MM.ACT.ABS absolute actual mm", this)
    ,Z_mmActNominal("Z.MM.ACT.NOMINAL actual position relativ to nominal 0", this)
    ,Z_stepsTo("Z.STEPS.TO target position in abs steps", this)
    ,Z_stepsBy("Z.STEPS.BY target position in relative steps", this)
    ,Z_mmTo("Z.MM.TO target position in abs mm on next device", this)
    ,Z_mmBy("Z.MM.BY target position in relative mm on next device", this)
    ,Z_mmUpLimit("Z.MM.UPLIMIT upper limit for Z.MM.TO", this)
    ,Z_mmLowLimit("Z.MM.LOWLIMIT lower limit for Z.MM.TO", this)
    ,Z_destinationReachable("Z.DEST.OK target position is reachable", this)
    ,Z_stepsMax("Z.STEPS.MAX highest position", this)
    ,Z_stepsMaxMax("Z.STEPS.MAXMAX for sure greater than highest position", this)
    ,Z_stepsNormPosition("Z.STEPS.DIODE photo diode changes its state", this)
    ,Z_mmAct_hist_("Z.MM.ACT.NOMINAL.HIST absolute actual mm", this)
    
    ,Z_button_move("Z.DOMOVEBY command move to/by", this)
    ,Z_button_moveToNominal("Z.DOMOVETONOMINAL move to nominal position", this)
    ,Z_button_stop("Z.DOSTOP stop motor", this)
    ,Z_button_zeroPosition("Z.DOZEROPOS find NormPosition - photo diode changes its state", this)
    ,Z_button_init("Z.DOINIT init- includes find NormPosition", this)
    ,Z_button_moveToZero("Z.DOMOVETOZERO move to abs 0 (NormPosition)", this)
    ,Z_button_setAsNominal("Z.DOSETNOMINAL Z nominal = Z actual", this)

{
    if (!xyzConf_done) {
	list_append();
	xyzConf_done = 1;
    }
}

void EqFctLbl_XYZ_Device::init() {
    EqFctLbl_XYDevice::init();
    Z_mmAct_hist_.book_hist (arch, name_.value(), ring_buffer );
}

void EqFctLbl_XYZ_Device::CreateDrives() {
    EqFctLbl_XYDevice::CreateDrives();
    Z_drive = new Drive(this, theMICOS_Controller, 
    	Z_motorNumber.value(), Z_diode_bit.value(), Z_text_str,
        &Z_Nominal, &Z_stepsActAbs, &Z_lastValidSteps, &Z_stepsActNominal, &Z_mmActAbs, &Z_mmActNominal,
	&Z_mmLowLimit, &Z_mmUpLimit,
    	&Z_stepsTo, &Z_stepsBy, &Z_mmTo, &Z_mmBy, &Z_destinationReachable, &Z_stepsMax, &Z_stepsMaxMax, 
	&Z_stepsNormPosition, Z_axisFactor.value(), Z_diode_dark_at0.value()!=0);
        
	getOrSet_X.SetHalf(X_drive);
        getOrSet_Y.SetHalf(Y_drive);
        getOrSet_Z.SetHalf(Z_drive);
	
        //??
        //errorTexts->ChangeBaseSet(X_drive->ErrorTexts());
}

void EqFctLbl_XYZ_Device::initHardware()
{
    cerr << timePoint().str(timePoint::date) << "going to initHardware() " << fct_name() << endl;
    X_drive->Init();
    Y_drive->Init();
    Z_drive->Init();
    int initCommand = 0;
    int autoI = autoInit.value() & 1;
    if(autoI) {
	if(!X_drive->PositionOK()) initCommand |= cmdS_X | cmd_zeroPosition;
    	if(!Y_drive->PositionOK()) initCommand |= cmdS_Y | cmd_zeroPosition;
    	if(!Z_drive->PositionOK()) initCommand |= cmdS_Z | cmd_zeroPosition;
    }
    autoInit.set_value(autoI); // strip off all the other bits
    if(initCommand) PerformCommand(initCommand, NULL); // force center finding    
    cerr << "  EqFctLbl_XYZDevice::initHardware() done >" << fct_name() << "<, initCommand=0x" << hex << initCommand << dec << endl;
}

void EqFctLbl_XYZ_Device::online() {
    EqFctLbl_XYDevice::online();
    smStatus stat = Z_drive->stepperMotor->ItsController()->GetStatus();
	Z_status = stat;
}

void EqFctLbl_XYZ_Device::FillHistories(int error) {
    TDS	history;
    history.tm = walltime;
    history.status = error;
    #define FillTheHistory(hist, val) history.data = val; hist.fill_hist(&history);
    FillTheHistory(X_mmAct_hist_, X_mmActNominal.value());
    FillTheHistory(Y_mmAct_hist_, Y_mmActNominal.value());
    FillTheHistory(Z_mmAct_hist_, Z_mmActNominal.value());
}

void EqFctLbl_XYZ_Device::updateAllDrives() {
    if(theMICOS_Controller->getDoUpdateAllDrives()) 
    {	// do NOT perform the update when an other motor command is "on the way"
    	// since that will cause problems with the controller 
        X_drive->update();
        Y_drive->update();
        Z_drive->update();
    }
}


D_int_cmd_(D_int_cmd_Z_move);
D_int_cmd_(D_int_cmd_Z_moveToNominal);
D_int_cmd_(D_int_cmd_Z_stop);
D_int_cmd_(D_int_cmd_Z_zeroPosition);
D_int_cmd_(D_int_cmd_Z_init);
D_int_cmd_(D_int_cmd_Z_moveToZero);
D_int_cmd_(D_int_cmd_Z_setAsNominal);

smStatus EqFctLbl_XYZ_Device::PerformSeletedCmd(int val) {
    smStatus stat = 0;
    
    Z_drive->ContinueAt(0);
    int baseCmd = val & cmdMask_baseCmds;
    //cerr << "PerformSeletedCmd(int val) = 0x" << hex << val  << dec << ", baseCmd= " << baseCmd << endl;
    switch(baseCmd) {
        case cmd_init:  // init all 3 motors
	    Z_drive->stepsNormPosition->set_value(-1);
    	    cerr << "PerformSeletedCmd(int val) = 0x" << hex << val  << dec << ", baseCmd= " << baseCmd << endl;
       	case cmd_zeroPosition:  // re-adjust all 3 motors
	    Z_drive->stepsNormPosition->set_value(-1); //?? just for the moment
		X_drive->stepsNormPosition->set_value(-1);
		Y_drive->stepsNormPosition->set_value(-1);
		cerr << timePoint().str() << "going to perform cmd_init for " << fct_name() << endl;
	    pendingCommand |= cmdS_Z | cmdS_Z_now;
	    break;
        case cmd_move:  // move (by) one or both motors
        case cmd_moveToNominal:  // move to nominal one or both motors
        case cmd_stop:  // stop one or both motors
        case cmd_resetController:  // reset controller of motors
        case cmd_moveToZero:  // move to abs 0 (NormPosition)
        case cmd_setAsNominal:  // set current position as the new nominal, one motor 
	    if( ((pendingCommand & cmdS_X) != 0) &&
	        ((pendingCommand & cmdS_Y) != 0) )
	       pendingCommand |= cmdS_Z;
	default: ;
    }
    stat = EqFctLbl_XYDevice::PerformSeletedCmd(val);
    return stat;
}

smStatus EqFctLbl_XYZ_Device::TryReady() {
    // perform a movement (continue)
    
    smStatus stat2 = EqFctLbl_XYDevice::TryReady();
    if(stat2 < 0) return stat2;
    
    smStatus stat = 0;
    int baseCmd = pendingCommand & cmdMask_baseCmds;
    switch(baseCmd) {
        case cmd_move:  // move (by) one or both motors
	    if(pendingCommand & cmdS_Z) Z_status = stat = Z_drive->MoveTo(Z_stepsActAbs.value() + Z_stepsBy.value());
	    break;
        case cmd_moveToNominal:  // move to nominal one or both motors
	    if(pendingCommand & cmdS_Z) Z_status = stat = Z_drive->MoveTo(Z_Nominal.value());
	    break;
        case cmd_moveToZero:  // move to abs 0 (NormPosition)
	    if(pendingCommand & cmdS_Z) Z_status = stat = Z_drive->MoveTo(0);
	    break;
        case cmd_stop:  // stop one or both motors
	    if(pendingCommand & cmdS_Z) Z_status = stat = Z_drive->Stop();
	    break;
        case cmd_init:  // init all 3 motors
        case cmd_zeroPosition:  // move one motor to NormPosition, using the state of the photo diode
	    if(pendingCommand & cmdS_Z_now) {
		Z_status = stat = Z_drive->FindNormPosition(pendingCommand, cmdS_Z_now);
	    }
	    break;
	default:; // nothing to do
    }
    if(stat == 0) stat = stat2;
    //??if(stat <= 0) commandFinished(stat);
    return stat;
}

void EqFctLbl_XYZ_Device::NotifyStatus() {
    smStatus stat = NotifyAxisStatus(0, X_status, (X_destinationReachable.value()==1), &X_errNb, &X_errTxt);
    stat = NotifyAxisStatus(stat, Y_status, (Y_destinationReachable.value()==1), &Y_errNb, &Y_errTxt);
    stat = NotifyAxisStatus(stat, Z_status, (Z_destinationReachable.value()==1), &Z_errNb, &Z_errTxt);
    NotifyOverallStatus(stat);
}

smStatus EqFctLbl_XYZ_Device::SetAsNominal(int val) {
    smStatus stat = EqFctLbl_XYDevice::SetAsNominal(val);
    smStatus stat2 = 0;
    if(val & cmdS_Y)  stat2 = Z_drive->SetAsNominal();
    if((stat == 0) || (stat2 < 0)) stat = stat2;
    return stat;
}
   
