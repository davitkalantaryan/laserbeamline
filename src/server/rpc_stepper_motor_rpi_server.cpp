/*
 *	File: rpc_stepper_motor_rpi_server.cpp
 *
 *	Created on: 11 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include "pitz_rpi_steppermotor_eqfctlowscr3.hpp"
#include "pitz_rpi_steppermotor_eqfctthz.hpp"

const char*	object_name = "stepper_motor_rpi_server";
void interrupt_usr1_prolog(int)  {}
void	eq_init_epilog() 	{}
void eq_cancel(void){}
void post_init_prolog(void){}
void	eq_init_prolog() 	{}


EqFct* eq_create(int a_eq_code, void* a_comPortName)
{
	EqFct* pRet = NULL;

	switch (a_eq_code)
	{
	case ((int)pitz::rpi::EQ_FCT_CODES::GEN_COM_PORT):
		// "\r\n" depends on the application
		pRet = new pitz::rpi::ComPortEqFct("\r\n",(const char*)a_comPortName);
		break;
	case ((int)pitz::rpi::STP_MTR_EQ_FCT_CODES::LOW_SCR3):
		pRet = new pitz::rpi::StepperMotor::EqFctLowScr3;
		break;
	case ((int)pitz::rpi::STP_MTR_EQ_FCT_CODES::THZ_MOT):
		pRet = new pitz::rpi::StepperMotor::EqFctThz;
		break;
	default: break;
	}
	return pRet;
}

void refresh_prolog(){}
void refresh_epilog()	{}	// called after "update"
void post_init_epilog(void)	 {}
void interrupt_usr1_epilog(int)  {}
