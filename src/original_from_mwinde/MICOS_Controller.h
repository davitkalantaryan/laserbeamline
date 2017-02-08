// file MICOS_Controller.h
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

#ifndef MICOS_Controller_h
#define MICOS_Controller_h

#ifndef __NO_DOOCS_SERVER
#include "sema.h"
#include <eq_fct.h>
#include "ArchiveFile.h"
#endif

#include "IPUnidig.h"
#include "IP500_Acromag.h"
#include "StepperM_MICOS.h"
#include "timeClass.h"
#include "NumberedStrings.h"

#include "StepperM_MICOS.h"

using namespace std;

#ifndef __NO_DOOCS_SERVER

#define CodeMICOS_Controller 301

class MICOS_Controller;

const static int maxNbOfControllers = 4;

class EqFctMICOS_Controller : public EqFct {

	friend class MICOS_Controller;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// theory of operation:
// 5. update() checks whether a command is pending. If yes, it checks the state of the magnet mover
//	  and sets status and errorTxt accordingly. If a command finished, the button that forced the command
//	  will be unpressed by the server.

protected:

	u_short 	control;	// var for D_bit
	D_name		alias_;

	D_int controllerNumber;		// 0..maxNbOfControllers-1, identifies the controller to the motors
	D_int powerPattern;			// 
	D_string ipCarrierRS232;	// name of the IP-carrier board that carries the IP
					// ( as specified in file  $VME_CONF/VME<hostname>conf)
	D_int addrRS232;		// VME adresses (offset from start address of carrier board)
	D_int portRS232;		// port number used on the Acromag IP500 module (RS232)
					// 0=A, 1=B, 2=C, 3=D	
	D_int doReset;	// =1 perform a reset of the controller
	D_int doUpdateAllDrives;	// 1:default, 0: do not perform EqFctLbl_XYDevice::updateAllDrives() during update()
	
public:
	EqFctMICOS_Controller();		// constructor
        ~EqFctMICOS_Controller() { ; }	// destructor

	void	update ();
	void	init ();	// started after creation of all Eq's
        int	fct_code()	{ return CodeMICOS_Controller; }
	
	static int	conf_done;
		
private:
	MICOS_Controller* theMICOS_Controller;
	int caseToContinue;	// >=0: Reset() in progress, after the controller was powered
	
};
#endif

class MICOS_Controller
{
	#ifndef __NO_DOOCS_SERVER
	friend class EqFctMICOS_Controller;
	#endif
	
	friend class StepperM_MICOS;

public:
	// creator
	MICOS_Controller(EqFctMICOS_Controller* anEqf, IP500_Acromag * theSerialMotorController,
		IPUnidig* theUnidig, int thePowerBit);
		
	int CheckControllerPower();	// return power state
	int Reset(int& caseToContinue); // (soft-)switch controller power on,
					// reset the controller,
					// reset all motors connected to the controller
	void AppendMotor(StepperM_MICOS* aMotor);	// append a motor to the listOfMotors
	void InitAllMotors();		// Init() all motors in listOfMotors
	
	int GetStatus() { return status;}
	NumberedStrings * ErrorTexts() {return errorTexts;}
	
        const char* GetVersion() {return itsSerialMotorController->GetVersion();}
	int getDoUpdateAllDrives();
	
	const int unidig_bit_controller;// 1-bit pattern: the bit==1 shows which bit of the 
					// ip-unidig is used to show the power state of this controller

	#ifndef __NO_DOOCS_SERVER
	sema syncAllHardware;	// sync all hardware calls
	sema syncACall;	// sync Telegram--Ack
	#endif

	static const int ErrPowerIsOff = -28;	//  power supply off
	static const int ErrResetting = -29;	//  controller is resetting
	static const int ErrNoMotors = -30;	//  no motors appended to the controller
	static const int ErrWrongProgramFlow = -33;	//  internal program error, call an expert

private:
	
	// prepare a telegram at MICOS_Tele, do not send it
	void MakeTelegramm(int MICOS_Addr, const char* const text1, const char* const text2);
	
	// send the telegram at MICOS_Tele
	int SendTelegram();
	
	int OnlySendTelegramm(StepperM_MICOS * theMotor, const char* const text1, const char* const text2);
	// send an MICOS telegram (try a number of times if necessarry),

	int SendAndAckTelegram();

	// make an MICOS telegram send it (try a number of times if necessarry),
	// get the acknowledgement	
	int TelegramDialog(StepperM_MICOS * theMotor, const char* const text1, const char* const text2);
	void Post();	// mandatory to call after the ack-buffer was read


	int GetAck();


    private:
        static const int maxTeleLength = 200;
        static int instanceCounter;
        static NumberedStrings * errorTexts;	// set of error texts

    private:
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
	

};

#endif
