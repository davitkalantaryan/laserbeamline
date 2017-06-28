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

#include "pitz_rpi_steppermotor_eqfctsm.hpp"

const char*	object_name = "stepper_motor_rpi_server";
void interrupt_usr1_prolog(int)  {}
void	eq_init_epilog() 	{}
void eq_cancel(void){}
void post_init_prolog(void){}
void	eq_init_prolog() 	{}


EqFct* eq_create(int eq_code, void*)
{
	EqFct* pRet = NULL;
	// enum class EQ_FCT_CODES{GEN_COM_PORT=304, GEN_COM_PORT_USER = 305, SM_COM_USER = 306};

	switch (eq_code)
	{
	case ((int)pitz::rpi::EQ_FCT_CODES::GEN_COM_PORT):
		pRet = new pitz::rpi::ComPortEqFct;
		break;
	case ((int)pitz::rpi::EQ_FCT_CODES::SM_COM_USER):
		pRet = new pitz::rpi::StepperMotor::EqFctSM;
		break;
	default: break;
	}
	return pRet;
}

void refresh_prolog(){}
void refresh_epilog()	{}	// called after "update"
void post_init_epilog(void)	 {}
void interrupt_usr1_epilog(int)  {}
