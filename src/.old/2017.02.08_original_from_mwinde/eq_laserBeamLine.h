// file eq_laserBeamLine.h 
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

#ifndef eq_laserBeamLine_h
#define eq_laserBeamLine_h

#include "sema.h"
#include "NumberedStrings.h"
#include <eq_fct.h>
#include "ArchiveFile.h"

#define CodeLaserBeamLine 300
#define CodeMICOS_Controller 301
#define CodeLbl_XY_Device 302
#define CodeLbl_Camera CodeLbl_XY_Device
#define CodeLbl_Aperture 303
#define CodeLbl_Mirror 304
#define CodeLbl_Pinhole CodeLbl_XY_Device
#define CodeLbl_WedgePlates 305

const static int  maxDevicesConnected = 20;	// number of OPTICAL devices connected

class EqFctlaserBeamLine : public EqFct {

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

protected:

	u_short 	control;	// var for D_bit
	D_name		alias_;

    D_int status;		/* status of the lbl: <0 error, 0: OK - ready, 1: moving, */
    char errLbl_str[STRING_LENGTH];	
    D_string errLbl;	/* text showing the status of the lbl */
    D_hist status_hist_;
					
public:
    EqFctlaserBeamLine ( );		// constructor
    ~EqFctlaserBeamLine ( ) { ; }	// destructor

    void	update ();
    void	init ();	// started after creation of all Eq's

    void	online();

    int	fct_code()	{ return CodeLaserBeamLine; }
    static int	conf_done;

private:	
    void NotifyStatus(int actPositionsUnknown, int stat);
    int UnknownActPositions(int& stat);	// return nb. of unknown positions
    	// stat = "most important" status of all devices

private:

    NumberedStrings * errorTexts;
    
    D_int devicesConnected;
    
    D_string ipCarrierUnidig;	// name of the IP-carrier board that carries the IP
				// ( as specified in file  $VME_CONF/VME<hostname>conf)
    D_int addrUnidig;		// VME adresses (offset from start address of carrier board)
    D_int typeUnidig;		// TYPE_UNIDIG IP-Unidig(=0), IP-Unidig-I(=1) or IP-Unidig-D(=2)
    D_int lightDiodes;		// unidig bit pattern
    
    static const int errBase = -10000;	// errors detected by the frame part of the server are in the range errBase..errBase-n
    static const int errIllCmd = errBase-1;	// client error: illegal cmd number
    static const int errNoState = errBase-3;	// internal server error: state of rotator was not detected
    static const int errCommandPending = errBase-4;	// client error: tried to move the magnet while the 
						    // last operation is still in progress
    static const int errDeviceOffline = errBase-5;	// device is offline
};

#endif
