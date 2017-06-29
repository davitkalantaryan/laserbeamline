// file Lbl_XYZ_Device.h
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

#ifndef lbl_XYZ_Device_h
#define lbl_XYZ_Device_h

#include "Lbl_XYDevice.h"

class EqFctLbl_XYZ_Device;

SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_move, cmdS_Z | cmd_move);
SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_moveToNominal, cmdS_Z | cmd_moveToNominal);
SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_stop, cmdS_Z | cmd_stop);
SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_zeroPosition, cmdS_X | cmdS_Y | cmdS_Z | cmd_zeroPosition);
SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_init, cmdS_Z | cmd_init);
SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_moveToZero, cmdS_Z | cmd_moveToZero);
SimpleButton(EqFctLbl_XYZ_Device, D_int_cmd_Z_setAsNominal, cmdS_Z | cmd_setAsNominal);

class EqFctLbl_XYZ_Device : public EqFctLbl_XYDevice {

public:
    EqFctLbl_XYZ_Device();		// constructor
    ~EqFctLbl_XYZ_Device() { ; }	// destructor

    virtual void	init ();	// started after creation of all Eq's
    virtual void CreateDrives();
    virtual void initHardware();
    virtual void online();
    virtual void updateAllDrives();
    virtual void FillHistories(int error);

    virtual int	fct_code() = 0;

    static int	xyzConf_done;

    virtual smStatus PerformSeletedCmd(int val);
    virtual smStatus TryReady();
    
    virtual smStatus SetAsNominal(int val);   	
    virtual void NotifyStatus();
    
protected:
    DoButton getOrSet_Z;

    D_int Z_motorNumber;		// Z motor number
    D_int Z_diode_bit;	// this bit in the IP-Unidig contains the state of the zero-position diode
    D_int Z_diode_dark_at0; // 1: it should be dark at pos 0 (at -initiator), 0: should be light
    D_float Z_axisFactor;   // mm (on target) from steps
    char Z_text_str[STRING_LENGTH];	
    D_string Z_text;	/* text describing what y-direction really is (e. g. "vertical" */
    D_int Z_errNb;			// Z motor status
    char Z_err_str[STRING_LENGTH];	
    D_string Z_errTxt;
    smStatus Z_status;
    D_int Z_Nominal;
    D_int Z_stepsActAbs;
    D_int Z_lastValidSteps;
    D_int Z_stepsActNominal;
    D_float Z_mmActAbs;
    D_float Z_mmActNominal;
    D_int_quad Z_stepsTo;
    D_int_quad Z_stepsBy;
    D_float_quad Z_mmTo;
    D_float_quad Z_mmBy;
    D_float Z_mmUpLimit;
    D_float Z_mmLowLimit;
    D_int Z_destinationReachable;
    D_int Z_stepsMax;
    D_int Z_stepsMaxMax;
    D_int Z_stepsNormPosition;

    D_hist Z_mmAct_hist_;

    D_int_cmd_Z_move   Z_button_move;
    D_int_cmd_Z_moveToNominal   Z_button_moveToNominal;
    D_int_cmd_Z_stop   Z_button_stop;
    D_int_cmd_Z_zeroPosition   Z_button_zeroPosition;
    D_int_cmd_Z_init   Z_button_init;
    D_int_cmd_Z_moveToZero   Z_button_moveToZero;
    D_int_cmd_Z_setAsNominal   Z_button_setAsNominal;

    Drive * Z_drive;
};

#endif
