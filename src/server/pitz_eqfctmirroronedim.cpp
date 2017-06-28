/*
 *	File: pitz_eqfctmirroronedim.cpp
 *
 *	Created on: 28 Feb 2017
 *	Created by: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements ...
 *
 */
#include "pitz_eqfctmirroronedim.hpp"
#include <eq_errors.h>
#include "MICOS_Controller.h"
#include "doocsDebug.h"
#include	"eq_fct_errors.h"

extern ArchiveFile*	arch;
extern time_t		walltime;
extern  int		ring_buffer;

int g_nDebugApp = 0;

#ifndef __DEBUG_APP__
#define __DEBUG_APP__(__log_level__,...)  do{\
    if((__log_level__)<=g_nDebugApp){\
        printf("fl:%s, ln:%d",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");}}while(0)
#endif

pitz::EqFctMirrorOneDim::EqFctMirrorOneDim()
{
    __DEBUG_APP__(1," ");
}


pitz::EqFctMirrorOneDim::~EqFctMirrorOneDim()
{
    __DEBUG_APP__(1," ");
}


int	pitz::EqFctMirrorOneDim::fct_code()
{
    __DEBUG_APP__(1," ");
    return CodeLbl_OneDimMirror;
}


smStatus pitz::EqFctMirrorOneDim::PerformSeletedCmd(int a_nVal)
{
    __DEBUG_APP__(1," ");
    smStatus stat = 0;
    int baseCmd = a_nVal & cmdMask_baseCmds;
    switch(baseCmd) {
    case cmd_zeroPosition:
        pendingCommand |= cmdS_X_fine;	// force additional fine-tuning
    default: break; // not a command special to EqFctLbl_Mirror
    }
    stat = EqFctLbl_XYDevice::PerformSeletedCmd(a_nVal);
    return stat;
}


void pitz::EqFctMirrorOneDim::init()
{
    __DEBUG_APP__(1," ");
    set_error ( no_error );

    theMICOS_Controller = NULL;

    X_mmAct_hist_.book_hist (arch, name_.value(), ring_buffer );
    //Y_mmAct_hist_.book_hist (arch, name_.value(), ring_buffer );

    if(!errorTexts) {
        errorTexts = new NumberedStrings(NULL, 3); // will be set after Drives have been created
    errorTexts->Append(errIllCmd, "illegal cmd number");
    errorTexts->Append(errCommandPending, "operation in progress");
    errorTexts->Append(errDeviceOffline, "server is offline");
    }
    ActPosKnown(motorPositionsKnown.value()==1);
    g_sts_.online (0);	// force an initHardware(); at the first update()
}


void pitz::EqFctMirrorOneDim::online()        // called by init() and RPC "set online"
{
    __DEBUG_APP__(1," ");
    int	error = 0;
    // do some online/alive test
    smStatus stat = X_drive->stepperMotor->ItsController()->GetStatus();
    //X_status = Y_status = stat;
    if (stat != 0) {
    // controller not ready (switched off or just reseting)
    error = 1;
        X_status = stat;
    }
    if (!error) {	// test successful
    g_sts_.error (0);
    g_sts_.newerror (0);
    g_sts_.online (1);
    } else {
    g_sts_.online (0);	// error handling
    }
}


void pitz::EqFctMirrorOneDim::updateAllDrives() {
    __DEBUG_APP__(1," ");
    if(theMICOS_Controller->getDoUpdateAllDrives())
    {	// do NOT perform the update when an other motor command is "on the way"
        // since that will cause problems with the controller
        X_drive->update();
        //Y_drive->update();
    }
    __DEBUG_APP__(1," ");
}


void pitz::EqFctMirrorOneDim::FillHistories(int error) {
    TDS	history;
    history.tm = walltime;
    history.status = error;
    #define FillTheHistory(hist, val) history.data = val; hist.fill_hist(&history);
    FillTheHistory(X_mmAct_hist_, X_mmActNominal.value());
    //FillTheHistory(Y_mmAct_hist_, Y_mmActNominal.value());
}


void pitz::EqFctMirrorOneDim::CreateDrives() {
    X_drive = new Drive(this, theMICOS_Controller,
        X_motorNumber.value(), X_diode_bit.value(), X_text_str,
        &X_Nominal, &X_stepsActAbs, &X_lastValidSteps, &X_stepsActNominal, &X_mmActAbs, &X_mmActNominal,
    &X_mmLowLimit, &X_mmUpLimit,
        &X_stepsTo, &X_stepsBy, &X_mmTo, &X_mmBy, &X_destinationReachable, &X_stepsMax, &X_stepsMaxMax,
    &X_stepsNormPosition, X_axisFactor.value(), X_diode_dark_at0.value()!=0);
#if 0
    Y_drive = new Drive(this, theMICOS_Controller,
        Y_motorNumber.value(), Y_diode_bit.value(), Y_text_str,
        &Y_Nominal, &Y_stepsActAbs, &Y_lastValidSteps, &Y_stepsActNominal, &Y_mmActAbs, &Y_mmActNominal,
    &Y_mmLowLimit, &Y_mmUpLimit,
        &Y_stepsTo, &Y_stepsBy, &Y_mmTo, &Y_mmBy, &Y_destinationReachable, &Y_stepsMax, &Y_stepsMaxMax,
    &Y_stepsNormPosition, Y_axisFactor.value(), Y_diode_dark_at0.value()!=0);
#endif

        getOrSet_X.SetHalf(X_drive);
        //getOrSet_Y.SetHalf(Y_drive);

        //??
        errorTexts->ChangeBaseSet(X_drive->ErrorTexts());
}


void pitz::EqFctMirrorOneDim::initHardware()
{
    //cerr << timePoint().str(timePoint::date) << "going to initHardware() " << fct_name() << endl;
    X_drive->Init();
    //Y_drive->Init();
    int initCommand = 0;
    int autoI = autoInit.value() & 1;
    if(autoI) {
        if(!X_drive->PositionOK()) initCommand |= cmdS_X | cmd_zeroPosition;
        //if(!Y_drive->PositionOK()) initCommand |= cmdS_Y | cmd_zeroPosition;
    }
    autoInit.set_value(autoI); // strip off all the other bits
    if(initCommand) PerformCommand(initCommand, NULL); // force center finding
    //cerr << "  EqFctLbl_XYDevice::initHardware() done >" << fct_name() << "<, initCommand=0x" << hex << initCommand << dec << endl;
}


smStatus pitz::EqFctMirrorOneDim::TryReady() {
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
        //if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->MoveTo(Y_stepsActAbs.value() + Y_stepsBy.value());
        break;
        case cmd_moveToNominal:  // move to nominal one or both motors
        if(pendingCommand & cmdS_X) X_status = stat = X_drive->MoveTo(X_Nominal.value());
        //if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->MoveTo(Y_Nominal.value());
        break;
        case cmd_moveToZero:  // move to abs 0 (NormPosition)
        if(pendingCommand & cmdS_X) X_status = stat = X_drive->MoveTo(0);
        //if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->MoveTo(0);
        break;
        case cmd_stop:  // stop one or both motors
        if(pendingCommand & cmdS_X) X_status = stat = X_drive->Stop();
        //if(pendingCommand & cmdS_Y) Y_status = stat2 = Y_drive->Stop();
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
        /*if(pendingCommand & cmdS_Y_now) {
        Y_status = stat2 = Y_drive->FindNormPosition(pendingCommand, cmdS_Y_now);
        }*/
        break;
    default:
        stat = errIllCmd;	// illegal command number
    }
    if((stat == 0) || (stat2 < 0)) stat = stat2;
    //??if(stat <= 0) commandFinished(stat);
    return stat;
}


void pitz::EqFctMirrorOneDim::NotifyStatus() {
    smStatus stat = NotifyAxisStatus(0, X_status, (X_destinationReachable.value()==1), &X_errNb, &X_errTxt);
    //stat = NotifyAxisStatus(stat, Y_status, (Y_destinationReachable.value()==1), &Y_errNb, &Y_errTxt);
    NotifyOverallStatus(stat);
}
