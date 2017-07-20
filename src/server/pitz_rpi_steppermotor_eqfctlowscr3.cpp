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

#include "pitz_rpi_steppermotor_eqfctlowscr3.hpp"
#include <string>

pitz::rpi::StepperMotor::EqFctLowScr3::EqFctLowScr3()
	:
	m_discretPos("DISCRET.POS (0)->(0), (1)->(20), (-1)->(-20)", this),
	m_doIt(
		COMMAND_TYPE::ACTUALIZE_POSITION,reinterpret_cast<TypeCallback>(&EqFctLowScr3::CallbackFunctionLowScr3),
		"DO.SET.POS sets the position from DISCRET.POS property", this),
	m_valueZero("EXPERT.VALUE.ZERO",this),
	m_valueMinus20("EXPERT.VALUE.MINUS20", this),
	m_valuePlus20("EXPERT.VALUE.PLUS20", this)
{
}

pitz::rpi::StepperMotor::EqFctLowScr3::~EqFctLowScr3()
{
}

int pitz::rpi::StepperMotor::EqFctLowScr3::fct_code()
{
	return (int)pitz::rpi::STP_MTR_EQ_FCT_CODES::LOW_SCR3;
}


void pitz::rpi::StepperMotor::EqFctLowScr3::post_init(void)
{
	pitz::rpi::ComPortUserEqFct::post_init();

	// To do
}


int pitz::rpi::StepperMotor::EqFctLowScr3::CallbackFunctionLowScr3(
	D_fct* a_prop, COMMAND_TYPET a_command,
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct,
	void* a_pData, int a_nDataLen)
{
	int nWrite, nWrRet;
	char vcString[512];

	if (!m_pComPort) { return -3; }
	vcString[0] = 0;

	switch (a_command)
	{
	case COMMAND_TYPE::ACTUALIZE_POSITION:
	{
		double lfValue;
		int nValue = m_discretPos.value();

		switch (nValue)
		{
		case CAMERA_POS::YAG_DZ_MINUS20:
			lfValue = (double)m_valueMinus20.value();
			nWrite = snprintf(vcString, 505, "%lf %d nm", lfValue, m_motorNumber.value());
			nWrRet = m_pComPort->WriteStringWithEnding2(vcString, nWrite);
			break;
		case CAMERA_POS::YAG_0_POS:
			lfValue = (double)m_valueZero.value();
			nWrite = snprintf(vcString, 505, "%lf %d nm", lfValue, m_motorNumber.value());
			nWrRet = m_pComPort->WriteStringWithEnding2(vcString, nWrite);
			break;
		case CAMERA_POS::YAG_DZ_PLUS20:
			lfValue = (double)m_valuePlus20.value();
			nWrite = snprintf(vcString, 505, "%lf %d nm", lfValue, m_motorNumber.value());
			nWrRet = m_pComPort->WriteStringWithEnding2(vcString, nWrite);
			break;
		default:
			snprintf(vcString, 511, "error!");
			break;
		}
	} // case COMMAND_TYPE::YAG_CAMERA_SET:
	break;

	default:
		snprintf(vcString, 511, "default!");
		return EqFctBase::CallbackFunctionSM(
			a_prop, a_command, a_dcsAdr, a_fromUser, a_toUser, a_fct,
			a_pData,a_nDataLen);
	}

	__DEBUG_APP__(0, "%s\n", vcString);

	return 0;
}
