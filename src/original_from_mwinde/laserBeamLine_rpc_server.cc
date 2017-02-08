// file laserBeamLine_rpc_server.cc 
//

#pragma implementation

#include	"eq_laserBeamLine.h"
#include "Lbl_XYDevice.h"
#include "Lbl_Aperture.h"
#include "Lbl_Mirror.h"
#include "Lbl_WedgePlates.h"

#include	"eq_errors.h"
#include	"eq_sts_codes.h"
#include	"eq_fct_errors.h"
#include 	"eq_client.h"
#include <iostream>

#include 	"MICOS_Controller.h"

//int current;	// dummy for libVME

MICOS_Controller* MICOS_controllers[maxNbOfControllers];	// array of all controllers
bool actPosOfDeviceIsKnown[maxDevicesConnected];		// array[optical devices]
int statusOfDevice[maxDevicesConnected];			// array[optical devices]
IPUnidig* unidig_lbl;						// the IP-Unidig modul 
bool EqFctlaserBeamLineInitDone = false;


const char*	object_name = "laserBeamLine_svr"; // name of this object (used in error messages)
EqFct*		laserBeamLine_fct;
int		EqFctlaserBeamLine::conf_done = 0;
ArchiveFile*	arch = 0;
int  arch_mode = 0;

//??extern	std::vector<EqFct*> *eq_list;
extern	int	chan_count;
extern	Config* config;
extern	int	fct_code;

extern  int		ring_buffer;

extern	EqFctSvr*	server_eq;

time_t			walltime;

#define laserBeamLine_STORE 	"laserBeamLine.hist"
#define laserBeamLine_DEV 	"laserBeamLine_channels"

EqFctlaserBeamLine::EqFctlaserBeamLine ( ) :
        EqFct ("NAME = location" )
        , alias_("ALIAS device name",this)



    // text showing the status of the rotator, color should change:
    // green: ready (to accept next go command), yellow: moving, red: error


    ,status("STATUS.LBL overall status of laserBeamLine", this)


    ,errLbl("STATUS.ERROR lbl error text", errLbl_str, STRING_LENGTH, this)
    ,status_hist_("STATUS.LBL.HIST overall status of laserBeamLine", this)
    ,devicesConnected("DEVICES.CONNECTED nb. of optical devices connected ", this)
    ,ipCarrierUnidig("IPCARRIER__UNIDIG name of IP carrier", this)
    ,addrUnidig("ADDR_UNIDIG VME addr IP-Unidig", this)
    ,typeUnidig("TYPE_UNIDIG IP-Unidig(=0), IP-Unidig-I(=1) or IP-Unidig-D(=2)", this)


    //
    ,lightDiodes("LIGHTDIODES unidig bit pattern", this)

{
    if (!conf_done) {
        list_append();
        conf_done = 1;
    }
}


void	eq_init_prolog ()	// called before init of all EqFct's
{
    EqFctlaserBeamLineInitDone = false;
	if (!arch) arch = new ArchiveFile( laserBeamLine_STORE, laserBeamLine_DEV );
	if (!arch)  {
		printf("Can't create Archiver. Program terminated !!\n");
		exit(1);
	}
}

void	EqFctlaserBeamLine::init ( )
{
//cerr << "EqFctlaserBeamLine::init()" << endl;

    for(int i=0; i< maxDevicesConnected; i++) actPosOfDeviceIsKnown[i] = false;
    unidig_lbl = new IPUnidig(ipCarrierUnidig.value(), addrUnidig.value(), typeUnidig.value());
    //
    // by init set noerror
    // be aware that the error status from the .conf file will 
    // be overwritten. This may not by usefull for every server!
    //
    set_error ( no_error );

    //
    // book the histories
    //
    status_hist_.book_hist (arch, name_.value(), ring_buffer );

    //
    // do an online/alive test
    //

    online();

    if(g_sts_.online()) {
	int stat;
	int actPositionsUnknown = UnknownActPositions(stat);
	NotifyStatus(actPositionsUnknown, stat);
    }
}

void	eq_init_epilog ()	// called at end of init of all EqFct's
{
    EqFctlaserBeamLineInitDone = true;
}

/*
void	eq_call_back (int i)
{
	EqFct* eqn;

	switch (i) {
	case 1:		// "{"  ==> new EqFct
		switch (fct_code) {
		case CodeFct:
			eqn =  new EqFct ("DUMMY");
			break;
		case CodeSvr:
			eqn =  new EqFctSvr ();
			break;
		case CodeLaserBeamLine:
			eqn =  new EqFctlaserBeamLine ();
			break;
		case CodeMICOS_Controller:
			eqn =  new EqFctMICOS_Controller ();
			break;
		case CodeLbl_XY_Device:
			eqn =  new EqFctLbl_XYDevice ();
			break;
		case CodeLbl_Aperture:
			eqn =  new EqFctLbl_Aperture ();
			break;
		case CodeLbl_Mirror:
			eqn =  new EqFctLbl_Mirror ();
			break;
		case CodeLbl_WedgePlates:
			eqn =  new EqFctLbl_WedgePlates ();
			break;
		default:
			eqn =  new EqFctlaserBeamLine ();
			break;
		}
		laserBeamLine_fct = eqn;
		eq_list->push_back(eqn);

		chan_count++;
		break;
	case 3:		// "}"  ==> read settings
		list_append(laserBeamLine_fct);
		break;
	default:
		break;
	}
};
*/

EqFct * 
eq_create (int eq_code, void *) 
{ 
        EqFct    *eqn; 
 
        switch (eq_code) { 
 
		case CodeFct:
			eqn =  new EqFct ("DUMMY");
			break;
		case CodeSvr:
			eqn =  new EqFctSvr ();
			break;
		case CodeLaserBeamLine:
			eqn =  new EqFctlaserBeamLine ();
			break;
		case CodeMICOS_Controller:
			eqn =  new EqFctMICOS_Controller ();
			break;
		case CodeLbl_XY_Device:
			eqn =  new EqFctLbl_XYDevice ();
			break;
		case CodeLbl_Aperture:
			eqn =  new EqFctLbl_Aperture ();
			break;
		case CodeLbl_Mirror:
			eqn =  new EqFctLbl_Mirror ();
			break;
		case CodeLbl_WedgePlates:
			eqn =  new EqFctLbl_WedgePlates ();
			break;
 
        default: 
             eqn = (EqFct *) 0; 
             break; 
        } 
        return eqn; 
} 

//-----------------------------------------------------------------------------------

void EqFctlaserBeamLine::NotifyStatus(int actPositionsUnknown, int stat) {

    status.set_value(stat);
    switch(stat) {
	case 0:	
	    switch(actPositionsUnknown) {
	    case 0:
	        errLbl.copy_value("OK. Actual positions of all optical devices known.");
	        break;
	    case 1:			
		errLbl.copy_value("OK. Actual position of one optical device unknown.");
		break;
	    default: {
		char buf[STRING_LENGTH];
		sprintf(buf, "OK. Actual position of %d optical devices unknown.", actPositionsUnknown);
		errLbl.copy_value(buf);
		break;
		}
	    }
	    break;
	default: {
	    errLbl.copy_value("ERRORs at one or more optical device(s).");
	    break;
	    }
    }
}

void	EqFctlaserBeamLine::update ( )
{
//?? cerr << "EqFctlaserBeamLine::update()" << endl;
	TDS	history;
	int error;
	
	history.tm = walltime;

	online();
	int isOnline = g_sts_.online();
    	if ( isOnline ) {
	    error = sts_ok;	// ... until we find different
	} else { //	device is offline
	    set_error( (int)went_offline );
	    error = sts_offline;
   	}
	history.status = error;
	int stat;
	int actPositionsUnknown = UnknownActPositions(stat);
	NotifyStatus(actPositionsUnknown, stat);
	
	#define FillTheHistory(hist, val) history.data = val; hist.fill_hist(&history);
	FillTheHistory(status_hist_, status.value());
	uint32_t  lightDiodesVal = unidig_lbl->Read();
	lightDiodes.set_value(lightDiodesVal);

}

int EqFctlaserBeamLine::UnknownActPositions(int& stat) {
    int actPositionsUnknown = 0;
    //??actPositionsUnknownPattern = 0;
    //??int oneBit = 0x1;
    stat = 0;
    for(int i=0; i<devicesConnected.value(); i++) {
        if(!actPosOfDeviceIsKnown[i]) {
	    //??actPositionsUnknownPattern |= oneBit;
	    actPositionsUnknown++;
	}
	//??oneBit << 1;
	int thisErr = statusOfDevice[i];
        stat = min(thisErr, stat);	// errorNb or 0
        if(stat==0) stat = max(thisErr, stat);	// if(no errors) not ready ?
    }
    return actPositionsUnknown;
}

void EqFctlaserBeamLine::online()        // called by init() and RPC "set online"
{
   int	error = 0;

	//
	// do some online/alive test
	//

    //?? the laser beamline - is always online ??
    //??	- is online, if all of its components are online
	//
	// test successful
	//
    //???
    // error always is 0, why checking is done here?
	if (!error) {
	    g_sts_.error (0);
	    g_sts_.newerror (0);
	    g_sts_.online (1);
	}
	else {
		g_sts_.online (0);
		// error handling
	}
}


void refresh_prolog ()		// called before "update"
{
	walltime = ::time(0);
}

void refresh_epilog ()		// called after "update"
{
}

void post_init_prolog() {}
void eq_cancel() {}
void post_init_epilog() {}

void interrupt_usr1_prolog (int)
{
}

void interrupt_usr1_epilog (int)
{
}

void interrupt_usr2_prolog ()
{
}

void interrupt_usr2_epilog ()
{
}

