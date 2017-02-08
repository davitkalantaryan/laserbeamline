#include	<math.h>

#ifndef __NO_DOOCS_SERVER
#include	"eq_errors.h"
#include	"eq_sts_codes.h"
#include	"eq_fct_errors.h"
#include 	"eq_client.h"
#endif

#include <strings.h>

#include "StepperM_MICOS.h"
#include "MICOS_Controller.h"
#include "timeClass.h"

#ifndef __NO_DOOCS_SERVER

int	EqFctMICOS_Controller::conf_done = 0;
extern	Config* 	config;

extern ArchiveFile*	arch;
extern time_t		walltime;
extern  int		ring_buffer;

//MICOS_Controller* MICOS_controllers[maxNbOfControllers];	// array of all controllers
extern IPUnidig* unidig_lbl;	// the IP-Unidig used of all motors of the Laser Beam Line

extern MICOS_Controller* MICOS_controllers[];


EqFctMICOS_Controller::EqFctMICOS_Controller():
    EqFct ("NAME = location" )
    ,alias_("ALIAS device name",this )

    ,controllerNumber("CONTROLLER_NUMBER nb. of this MICOS_Controller", this)
    ,powerPattern("POWER_PATTERN which bit shows the power state of the controller", this)
    ,ipCarrierRS232("IPCARRIER__RS232 name of IP carrier", this)
    ,addrRS232("ADDR_RS232 VME addr IP500 Acromag", this)
    ,portRS232("PORT_RS232 port used on IP500 Acromag", this)
    ,doReset("RESET perform controller reset", this)
    ,doUpdateAllDrives("DO.UPDATE 0: do not perform updateAllDrives() during update()", this)
    
    ,caseToContinue(-1)
{
    if (!conf_done) {
	list_append();
	conf_done = 1;
    }
}

void EqFctMICOS_Controller::init()
{
//cerr << "EqFctMICOS_Controller::init()" << endl;
    set_error ( no_error );

     // create a SerialMotorController
    IP500_Acromag* theRS232 = 
        new IP500_Acromag(ipCarrierRS232.value(), addrRS232.value(),
					portRS232.value());
    theMICOS_Controller
    	= new MICOS_Controller(this, theRS232, unidig_lbl, powerPattern.value());
    if(controllerNumber.value() < maxNbOfControllers)
    	MICOS_controllers[controllerNumber.value()] = theMICOS_Controller;
	
    theMICOS_Controller->syncACall.post();	// to allow the first action
    theMICOS_Controller->syncAllHardware.post();	// to allow the first action with the hardware
    doUpdateAllDrives.set_value(1);	// set to default
}

void EqFctMICOS_Controller::update() {

//??    if(!EqFctlaserBeamLineInitDone) return; // wait for all inits being finished

    int wasOnline = g_sts_.online();
    int	error = 0;
    if(theMICOS_Controller->CheckControllerPower() != 1) {
	error = 1; // motor controler switched off or IP-Unidig not reachable
    }
    if (!error) {
	g_sts_.error (0);
	g_sts_.newerror (0);
	g_sts_.online (1);
	if((!wasOnline) | (doReset.value())) { // went to online
    	    // perform a Reset(), this includes Init() of all motors
	    doReset.set_value(0);
	    theMICOS_Controller->status = MICOS_Controller::ErrResetting;
	    caseToContinue = 0;
    	    doUpdateAllDrives.set_value(1);	// set to default
	    theMICOS_Controller->Reset(caseToContinue); 
	} else {
	    if(caseToContinue >= 0) {
	        if(theMICOS_Controller->Reset(caseToContinue) == 0) {
	            theMICOS_Controller->status = 0;
		}
	    } else {
	        theMICOS_Controller->status = 0;
	    }
	}
    } else { // offline
	g_sts_.online (0);
	theMICOS_Controller->status = MICOS_Controller::ErrPowerIsOff;
    }
}

#endif

#if 0
int status;	// ErrPowerIsOff / ready / ErrResettings

        IPUnidig* unidig;
        IP500_Acromag* itsSerialMotorController;
        EqFctMICOS_Controller* itsEqf;

        #ifndef __NO_DOOCS_SERVER
        vector<StepperM_MICOS*> listOfMotors;
        #endif

        timePoint waitUntil;	// wait until that timePoint, before performing the next step
                                // of a Reset()

        int maxTrySend;		// max nb. of tries to send a telegram
        bool tryMulti;		// repeatedly try to perform a command

        int synchronGo;		// number of pending synchronous moves
        char MICOS_Tele[maxTeleLength];	// telegram to be sent actually
        int MICOS_TeleLength;		// nb of chars to send (including header and trailer)

        char MICOS_AckData[maxTeleLength]; // acknowledgement data
        int MICOS_AckDataLength;	   // length of acknowledgement
#endif // #if 0

int MICOS_Controller::instanceCounter;
NumberedStrings * MICOS_Controller::errorTexts;

MICOS_Controller::MICOS_Controller(EqFctMICOS_Controller* anEqf, IP500_Acromag* theSerialMotorController,
	IPUnidig* theUnidig, int thePowerBit) 
        :  unidig_bit_controller(thePowerBit),unidig(theUnidig), itsSerialMotorController(theSerialMotorController),  itsEqf(anEqf)
{	
    maxTrySend = 4;
    //??tryMulti = true;
    tryMulti = false;
    itsSerialMotorController->SetEndOfInfo(0xA);
    if(instanceCounter == 0) {
        errorTexts = new NumberedStrings(NULL, 3); // baseSet will be set later, after Motor was created
	errorTexts->Append(ErrPowerIsOff, "motor power supply off");
	errorTexts->Append(ErrResetting, "controller reset ...");
	errorTexts->Append(ErrWrongProgramFlow, "internal program error");
    }
    instanceCounter++;
}

void MICOS_Controller::AppendMotor(StepperM_MICOS* aMotor) {
// append a motor to the listOfMotors
#ifndef __NO_DOOCS_SERVER
    listOfMotors.push_back(aMotor);
#endif
if(errorTexts->GetBaseSet() == NULL)
    errorTexts->ChangeBaseSet(aMotor->ErrorTexts());
}

void MICOS_Controller::InitAllMotors() {
#ifndef __NO_DOOCS_SERVER
    for(int i=0; i<listOfMotors.size(); i++) listOfMotors[i]->Init();
#endif
}

int MICOS_Controller::CheckControllerPower() {
    //return 1;	// ?? for the moment: assume it's always on

    uint32_t rb = unidig->Read();
    if(rb & unidig_bit_controller) return 1;   // power is on
    return 0;	// power is off
}

int MICOS_Controller::Reset(int& caseToContinue) {
// perform a reset of the interface, init the motors

    const float secsToWaitAfterPowerOn = 8.0;
    const float secsToWaitAfterReset = 4.0;

    int controllerPower = CheckControllerPower();
    if(controllerPower != 1) return ErrPowerIsOff;
    
    #ifndef __NO_DOOCS_SERVER
    if(listOfMotors.size() <= 0) return ErrNoMotors;
    #endif
        
    switch(caseToContinue) {
	case 0: { // controller just switched on
		caseToContinue = 1;
		waitUntil = timePoint()+timeInterval(secsToWaitAfterPowerOn);	// wait after power on
		break;
	}	
	case 1: {
		if(timePoint() < waitUntil) break;
		#ifndef __NO_DOOCS_SERVER
		syncAllHardware.wait(); // ---------- enqueue
		listOfMotors[0]->Reset();
		syncAllHardware.post(); // ---------- dequeue
		#endif
		caseToContinue = 2;
		waitUntil = timePoint()+timeInterval(secsToWaitAfterReset);	// wait after "nreset"
		break;
	}
	case 2: {
		if(timePoint() < waitUntil) break;
		#ifndef __NO_DOOCS_SERVER
		syncAllHardware.wait(); // ---------- dequeue
		#endif
		InitAllMotors();
		caseToContinue = -1;	// we are done with this command
		#ifndef __NO_DOOCS_SERVER
		syncAllHardware.post(); // ---------- dequeue
		#endif
		return 0;
	}
	default:
		#ifndef __NO_DOOCS_SERVER
		cout << "MICOS_Controller::Reset: case " << caseToContinue
		<< " - ErrWrongProgramFlow" << endl;
		#endif
		return ErrWrongProgramFlow;
    }
    return StepperMotor::Busy;
}

// make an MICOS telegram, send it (try a number of times if necessarry),
// get the acknowledgement, analise the status
int MICOS_Controller::TelegramDialog(StepperM_MICOS * theMotor, const char* const text1, const char* const text2){	
    MakeTelegramm(theMotor->itsMICOS_Addr, text1, text2);
    int stat = SendAndAckTelegram();
    return stat;
}

// send an MICOS telegram (try a number of times if necessary),
int MICOS_Controller::OnlySendTelegramm(StepperM_MICOS * theMotor, const char* const text1, const char* const text2){
    int stat;
    syncACall.wait();
    MakeTelegramm(theMotor->itsMICOS_Addr,text1,text2);
    int i; for (i=0; i<maxTrySend; i++) {
	stat=SendTelegram();
	if(stat !=0) {
		cerr << "MICOS_Controller::OnlySendTelegramm ERROR stat = " << stat
		     << ", MICOS_Add=" << theMotor->itsMICOS_Addr << ", text1=" << text1 << ", text2=" << text2 
		     << ", i=" << i << endl;
	}
	if(stat == 0 || !tryMulti) break;  // success (or no repetition of command)
    }
    syncACall.post();
    return stat;
}

// send an MICOS telegram (try a number of times if necessary),
// get the acknowledgement
int MICOS_Controller::SendAndAckTelegram(){
    int stat;
    syncACall.wait();
    int i; for (i=0; i<maxTrySend; i++) {
	    stat = SendTelegram();
	    if(stat !=0) {
		cerr << "MICOS_Controller::SendAndAckTelegram Send ERROR stat = " << stat
		     << ", MICOS_Tele=" << MICOS_Tele  
		     << ", i=" << i << endl;
	    }
	    if (stat < 0) continue;	// send error -> try once more
	    stat = GetAck();
	    if(stat !=0) {
		cerr << "MICOS_Controller::SendAndAckTelegram GetAck ERROR stat = " << stat
		     << ", MICOS_Tele=\"" << MICOS_Tele  
		     << "\", i=" << i << endl;
	    }
	    if ((stat == 0) || !tryMulti) break;		// success (or no repetition of command)
    }
//    syncACall.post();
    return stat;
}

void MICOS_Controller::Post() {
    syncACall.post();
}

void MICOS_Controller::MakeTelegramm(int MICOS_Addr, const char* const text1,const char* const text2){

    if(strlen(text2) != 0) {
	sprintf(MICOS_Tele, "%s %d %s ", text2, MICOS_Addr, text1);
    } else {
	sprintf(MICOS_Tele, "%d %s ", MICOS_Addr, text1);
    }
    MICOS_TeleLength = strlen(MICOS_Tele);
}

int MICOS_Controller::SendTelegram() {

	if(itsSerialMotorController->Write(MICOS_Tele, MICOS_TeleLength) == MICOS_TeleLength)
	{
	//cerr << "MICOS_Controller::SendTelegram():" << MICOS_Tele << endl;
		return 0;
	}
	return StepperMotor::ErrSend;
}

int MICOS_Controller::GetAck() {

    int sumLen= 0;
    
    // wait until an <CR><LF> is received, 100 ms max.
    for(int i = 0; ; i++) {
	int len = itsSerialMotorController->Read(&MICOS_AckData[sumLen], sizeof(MICOS_AckData)-sumLen);
	sumLen += len;
	if( (sumLen>=2) && (MICOS_AckData[sumLen-2] == 0xD) && (MICOS_AckData[sumLen-1]== 0xA)) break;
	timeInterval(0.005).wait();	// wait 5 mSec
	if(i>20) {
	    MICOS_AckDataLength = 0;
	    MICOS_AckData[0] = char(0);
	    return StepperMotor::ErrReceive;	// no complete answer after about 1 sec
	}
    }
    MICOS_AckDataLength = sumLen-2;	    // length (without terminating 0)
    MICOS_AckData[MICOS_AckDataLength] = char(0); // set terminating 0 anyway
    return 0;
}

int MICOS_Controller::getDoUpdateAllDrives() {
	return itsEqf->doUpdateAllDrives.value();
}

