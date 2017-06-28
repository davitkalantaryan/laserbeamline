/*
 *	File		: pitz_steppermotor_rpi_eqfctsm.cpp
 *
 *	Created on	: 11 Mar, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements ...
 *
 *
 */

#include "pitz_rpi_steppermotor_eqfctsm.hpp"
#include <string>

pitz::rpi::StepperMotor::EqFctSM::EqFctSM()
	:
	m_stepsToMoveNm(COMMAND_TYPE::DOUBLE_CMD1, reinterpret_cast<TypeCallback>(&EqFctSM::CallbackFunctionSM),
			"MOVE.WITH.NM executes any command provided", this),
	m_stepsToMoveNrR(COMMAND_TYPE::DOUBLE_CMD2, reinterpret_cast<TypeCallback>(&EqFctSM::CallbackFunctionSM),
		"MOVE.WITH.NR.R executes any command provided", this),
	m_stepsToMoveNrL(COMMAND_TYPE::DOUBLE_CMD3, reinterpret_cast<TypeCallback>(&EqFctSM::CallbackFunctionSM),
		"MOVE.WITH.NR.L executes any command provided", this),
	m_stepsToMoveNcal(COMMAND_TYPE::VOID_CMD1, reinterpret_cast<TypeCallback>(&EqFctSM::CallbackFunctionSM),
		"GO.LEFT.END executes any command provided", this),
	m_motorNumber("MOTOR.NUMBER number of motor Number of motor in dasy chain, starts from 1",this)
{
	m_motorNumber.set_ro_access();
}

pitz::rpi::StepperMotor::EqFctSM::~EqFctSM()
{
}

int pitz::rpi::StepperMotor::EqFctSM::fct_code()
{
	return (int)pitz::rpi::EQ_FCT_CODES::SM_COM_USER;
}


void pitz::rpi::StepperMotor::EqFctSM::post_init(void)
{
	pitz::rpi::ComPortUserEqFct::post_init();

	// To do
}


int pitz::rpi::StepperMotor::EqFctSM::CallbackFunctionSM(D_fct* a_this, COMMAND_TYPET a_command, 
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	// To do
	//__DEBUG_APP__(0, "mov=%f, &Data=%p ",(float)m_stepsToMoveNm.value(),a_command_arg);

	if (!m_pComPort) { return -3; }

	switch (a_command)
	{
	case COMMAND_TYPE::DOUBLE_CMD1:
	{
		double lfValue;
		int nWritten;
		int nWrRet;
		char vcString[512];

#if D_TYPE_IN_USE==double
		lfValue = a_fromUser->get_double();
#elif D_TYPE_IN_USE==float
		lfValue = (double)a_fromUser->get_float();
#else
#error not handled type
#endif

		nWritten = snprintf(vcString,505,"%lf %d nm",lfValue,m_motorNumber.value());
		nWrRet=m_pComPort->WriteStringWithEnding(vcString,nWritten);		
		
		__DEBUG_APP__(0,"nWritten=%d, nWrRet=%d, val=%lf, set=\"%s\"",nWritten,nWrRet,lfValue,vcString);
		
	}
	break;

	case COMMAND_TYPE::DOUBLE_CMD2:
	{
		char vcString[512];
		const TYPE_IN_USE lfValue = m_stepsToMoveNrR.value();
		int nWritten = snprintf(vcString, 505, "%lf %d nm", (double)lfValue, m_motorNumber.value());
		int nWrRet = m_pComPort->WriteStringWithEnding(vcString, nWritten);

		__DEBUG_APP__(0, "nWritten=%d, nWrRet=%d, val=%lf, set=\"%s\"",
			nWritten, nWrRet, lfValue, vcString);

	}
	break;

	case COMMAND_TYPE::DOUBLE_CMD3:
	{
		char vcString[512];
		const TYPE_IN_USE lfValue = m_stepsToMoveNrR.value();
		int nWritten = snprintf(vcString, 505, "%lf %d nm", -((double)lfValue), m_motorNumber.value());
		int nWrRet = m_pComPort->WriteStringWithEnding(vcString, nWritten);

		__DEBUG_APP__(0, "nWritten=%d, nWrRet=%d, val=%lf, set=\"%s\"",
			nWritten, nWrRet, lfValue, vcString);

	}
	break;

	case COMMAND_TYPE::VOID_CMD1:
	{
		char vcString[512];
		int nWritten = snprintf(vcString, 505, "%d ncal", m_motorNumber.value());
		int nWrRet = m_pComPort->WriteStringWithEnding(vcString, nWritten);

		__DEBUG_APP__(0, "nWritten=%d, nWrRet=%d, set=\"%s\"",
			nWritten, nWrRet, vcString);

	}
	break;

	default:
		return pitz::rpi::ComPortUser::CallbackFunction(a_this, a_command, a_command_arg, a_arg_len);
	}

	return 0;
}



/*///////////////////////////////////////////////////////////////*/
pitz::rpi::StepperMotor::D_void::D_void(
	COMMAND_TYPET a_command, TypeCallback a_fpCallback, 
	const char* a_pn, EqFct* a_par)
	:
	New_D_types<D_fct>(a_command, a_fpCallback,a_pn,a_par)
{
}


void pitz::rpi::StepperMotor::D_void::write(fstream &ofile)
{
	char        buf[200];
	snprintf(buf, sizeof(buf), "%s: \n", base_name.c_str());
	flush(ofile, buf, 1);
}
