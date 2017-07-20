/*
 *	File		: pitz_rpi_steppermotor_eqfctthz.cpp
 *
 *	Created on	: 18 Jul, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements ...
 *
 *
 */

#include "pitz_rpi_steppermotor_eqfctthz.hpp"
#include <string>

pitz::rpi::StepperMotor::EqFctThz::EqFctThz()
	:
	m_moveAbsolute("POSITION.SP moves  the motor to the absolute position",this),
	m_setTo(
		COMMAND_TYPE::ACTUALIZE_POSITION,
		reinterpret_cast<TypeCallback>(&EqFctThz::CallbackFunctionTHz),
		"POSITION.CMD sets the position ", this),
	m_moveRelative("DISPLACEMENT.SP moves  the motor relative to the current pos", this),
	m_changeBy(
		COMMAND_TYPE::ACTUALIZE_POSITION, 
		reinterpret_cast<TypeCallback>(&EqFctThz::CallbackFunctionTHz),
		"DISPLACEMENT.CMD changes the position relative to the current position", this),
	m_velocitySP("VELOCITY.SP",this),
	m_velocityRDBK("VELOCITY.RDBK",this),
	m_velocityCmd(
		reinterpret_cast<TypeCallback>(&EqFctThz::CallbackFunctionTHz),
		"VELOCITY.CMD",this),
	m_homeCmd(
		reinterpret_cast<TypeCallback>(&EqFctThz::CallbackFunctionTHz),
		"HOME.CMD", this)
{
	m_currentPos.set_ro_access();
	m_velocityRDBK.set_ro_access();
	m_velocityCmd.set_ro_access(); // to be done
}

pitz::rpi::StepperMotor::EqFctThz::~EqFctThz()
{
}

int pitz::rpi::StepperMotor::EqFctThz::fct_code()
{
	return pitz::rpi::STP_MTR_EQ_FCT_CODES::THZ_MOT;
}


void pitz::rpi::StepperMotor::EqFctThz::post_init(void)
{
	pitz::rpi::ComPortUserEqFct::post_init();

	// To do
#if 1
	int nMotorNum = m_motorNumber.value();

	if((nMotorNum>0) && (nMotorNum<17)){
		int nStrLen;
		char vcBuffer[256];
		
		nStrLen = snprintf(vcBuffer, 255, "0 %d nm", nMotorNum);
		vcBuffer[nStrLen] = 0;
		m_homeCmd.SetFixedString(vcBuffer);
	}
#endif
}


int pitz::rpi::StepperMotor::EqFctThz::CallbackFunctionTHz(
	D_fct* a_this, COMMAND_TYPET a_command,
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct,
	void* a_pData, int a_nDataLen)
{
	int nWrite;
	char vcString[512];

	if (!m_pComPort) { return -3; }
	vcString[0] = 0;

	switch (a_command)
	{
	case COMMAND_TYPE::ACTUALIZE_POSITION:
	{
		double lfMax = (double)m_upperLimit.value();
		double lfMin = (double)m_lowerLimit.value();
		double lfValue = (double)m_moveAbsolute.value();
		if(lfValue>lfMax){lfValue= lfMax;}
		else if(lfValue<lfMin){lfValue= lfMin;}
		nWrite = (int)snprintf(vcString,505,"%lf %d nm",lfValue,m_motorNumber.value());
		m_pComPort->WriteStringWithEnding2(vcString, nWrite);
		m_currentPos.set_value((RAW_VALUE_TYPE)lfValue);
	}
		break;
	
	case COMMAND_TYPE::CHANGE_BY:
	{
		double lfMax = (double)m_upperLimit.value();
		double lfMin = (double)m_lowerLimit.value();
		double lfCur=(double)m_currentPos.value();
		double lfValue = (double)m_moveRelative.value();
		if(abs(lfValue)>(lfMax- lfMin)){/*setError*/return -3;}
		if ((lfValue+lfCur)>lfMax){lfValue=(lfMax-lfCur); }
		else if((lfValue+lfCur)<lfMin){lfValue=(lfMin-lfCur); }
		nWrite = (int)snprintf(vcString, 505, "%lf %d nr", lfValue, m_motorNumber.value());
		m_pComPort->WriteStringWithEnding2(vcString, nWrite);
		m_currentPos.set_value((RAW_VALUE_TYPE)(lfValue + lfCur));
	}
		break;

	default:
		snprintf(vcString, 511, "default!");
		return EqFctBase::CallbackFunctionSM(
			a_this, a_command, 
			a_dcsAdr, a_fromUser, a_toUser, a_fct,
			a_pData, a_nDataLen);
	}

	__DEBUG_APP__(0, "str=\"%s\"\n", vcString);

	return 0;
}

