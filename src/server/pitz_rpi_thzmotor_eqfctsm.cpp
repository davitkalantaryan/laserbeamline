/*
 *	File		: pitz_rpi_thzmotor_eqfctsm.cpp
 *
 *	Created on	: 18 Jul, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements ...
 *
 *
 */

#include "pitz_rpi_thzmotor_eqfctsm.hpp"
#include <string>

pitz::rpi::ThzMotor::EqFctThzMot::EqFctThzMot()
	:
	m_discretPos("DISCRET.POS (0)->(0), (1)->(20), (-1)->(-20)", this),
	m_doIt(COMMAND_TYPE::YAG_CAMERA_SET,reinterpret_cast<TypeCallback>(&EqFctThzMot::CallbackFunctionSM),
		"DO.SET.POS sets the position from DISCRET.POS property", this),
	m_motorNumber("MOTOR.NUMBER motor number in the desy chain [1-16]", this),
	m_valueZero("EXPERT.VALUE.ZERO",this),
	m_valueMinus20("EXPERT.VALUE.MINUS20", this),
	m_valuePlus20("EXPERT.VALUE.PLUS20", this),
	m_goLeft(COMMAND_TYPE::GO_LEFT, reinterpret_cast<TypeCallback>(&EqFctThzMot::CallbackFunctionSM),
		"OTHER.GO.LEFT moves to the left end", this),
	m_goRight(COMMAND_TYPE::GO_RIGHT, reinterpret_cast<TypeCallback>(&EqFctThzMot::CallbackFunctionSM),
		"OTHER.GO.RIGHT moves to the right end", this),
	m_anyString(COMMAND_TYPE::ANY_COMMAND, reinterpret_cast<TypeCallback>(&EqFctThzMot::CallbackFunctionU),
		"OTHER.ANY.COMMAND runs any command", this)
{
	m_motorNumber.set_ro_access();
}

pitz::rpi::ThzMotor::EqFctThzMot::~EqFctThzMot()
{
}

int pitz::rpi::ThzMotor::EqFctThzMot::fct_code()
{
	return (int)pitz::rpi::EQ_FCT_CODES::THZ_MOT_COM_USER;
}


void pitz::rpi::ThzMotor::EqFctThzMot::post_init(void)
{
	pitz::rpi::ComPortUserEqFct::post_init();

	// To do
}


int pitz::rpi::ThzMotor::EqFctThzMot::CallbackFunctionSM(D_fct* a_this, COMMAND_TYPET a_command,
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	int nWrite, nWrRet;
	char vcString[512];

	if (!m_pComPort) { return -3; }

	switch (a_command)
	{
	case COMMAND_TYPE::YAG_CAMERA_SET:
	{
		double lfValue;
		int nValue = m_discretPos.value();

		switch (nValue)
		{
		case CAMERA_POS::YAG_DZ_MINUS20:
			lfValue = (double)m_valueMinus20.value();
			nWrite = snprintf(vcString, 505, "%lf %d nm", lfValue, m_motorNumber.value());
			nWrRet = m_pComPort->WriteStringWithEnding(vcString, nWrite);
			break;
		case CAMERA_POS::YAG_0_POS:
			lfValue = (double)m_valueZero.value();
			nWrite = snprintf(vcString, 505, "%lf %d nm", lfValue, m_motorNumber.value());
			nWrRet = m_pComPort->WriteStringWithEnding(vcString, nWrite);
			break;
		case CAMERA_POS::YAG_DZ_PLUS20:
			lfValue = (double)m_valuePlus20.value();
			nWrite = snprintf(vcString, 505, "%lf %d nm", lfValue, m_motorNumber.value());
			nWrRet = m_pComPort->WriteStringWithEnding(vcString, nWrite);
			break;
		default:
			snprintf(vcString,511,"error!");
			break;
		}
	} // case COMMAND_TYPE::YAG_CAMERA_SET:
	break;

	case COMMAND_TYPE::GO_LEFT:
		nWrite = snprintf(vcString, 505, "%d ncal \r\n",m_motorNumber.value());
		m_pComPort->WriteByteStream(vcString, nWrite);
		break;

	case COMMAND_TYPE::GO_RIGHT:
		nWrite = snprintf(vcString, 505, "%d nrm \r\n", m_motorNumber.value());
		m_pComPort->WriteByteStream(vcString, nWrite);
		break;

	default:
		snprintf(vcString, 511, "default!");
		return pitz::rpi::ComPortUserEqFct::CallbackFunctionU(
			a_this, a_command, a_dcsAdr, a_fromUser, a_toUser, a_fct);
	}

	__DEBUG_APP__(0, "%s\n", vcString);

	return 0;
}
