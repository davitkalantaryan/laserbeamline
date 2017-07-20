
// pitz_rpi_steppermotor_eqfctbase.cpp
// 2017 Jul 20

#include "pitz_rpi_steppermotor_eqfctbase.hpp"

pitz::rpi::StepperMotor::EqFctBase::EqFctBase()
	:
	m_motorNumber("MOTOR.NUMBER motor number in the desy chain [1-16]", this),
	m_goLeft(COMMAND_TYPE::GO_LEFT, reinterpret_cast<TypeCallback>(&EqFctBase::CallbackFunctionSM),
		"OTHER.GO.LEFT moves to the left end", this),
	m_goRight(COMMAND_TYPE::GO_RIGHT, reinterpret_cast<TypeCallback>(&EqFctBase::CallbackFunctionSM),
		"OTHER.GO.RIGHT moves to the right end", this),
	m_upperLimit("POSITION.MAXIMUM upper limit of the motor possition",this),
	m_lowerLimit("POSITION.MINIMUM lower limit of the motor possition", this),
	m_currentPos("POSITION.RDBK",this),
	m_anyString(
		COMMAND_TYPE::SET_ANY_ASCII_STRING, 
		reinterpret_cast<TypeCallback>(&EqFctBase::CallbackFunctionU),
		"OTHER.ANY.COMMAND runs any command", this),
	m_statusInt("STATUS.INT",this),
	m_statusStr("STATUS.STR", this),
	m_abortCmd(reinterpret_cast<TypeCallback>(&EqFctBase::CallbackFunctionSM),
		"ABORT.CMD stops all activites", this),
	m_velocity("VELOCITY",this)
{
	char vcExecString[8];

	m_motorNumber.set_ro_access();
	m_upperLimit.set_ro_access();
	m_lowerLimit.set_ro_access();
	m_statusInt.set_ro_access();
	m_statusStr.set_ro_access();

	vcExecString[0]=3;
	vcExecString[1]=13;
	vcExecString[2]=10;
	vcExecString[3]=3;
	vcExecString[4]=0;
	m_abortCmd.SetFixedString(vcExecString);

	// to be done
	m_velocity.set_ro_access();
}


pitz::rpi::StepperMotor::EqFctBase::~EqFctBase()
{
}


int pitz::rpi::StepperMotor::EqFctBase::CallbackFunctionSM(
	D_fct* a_this, COMMAND_TYPET a_command,
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct,
	void* a_pData, int a_nDataLen)
{
	int nWrite;
	char vcString[512];

	if (!m_pComPort) { return -3; }

	switch (a_command)
	{
	case COMMAND_TYPE::GO_LEFT:
		nWrite = snprintf(vcString, 505, "%d ncal \r\n", m_motorNumber.value());
		m_pComPort->WriteByteStream(vcString, nWrite);
		break;

	case COMMAND_TYPE::GO_RIGHT:
		nWrite = snprintf(vcString, 505, "%d nrm \r\n", m_motorNumber.value());
		m_pComPort->WriteByteStream(vcString, nWrite);
		break;

	default:
		snprintf(vcString, 511, "default!");
		return pitz::rpi::ComPortUserEqFct::CallbackFunctionU(
			a_this, a_command, a_dcsAdr, a_fromUser, a_toUser, a_fct,a_pData,a_nDataLen);
	}

	__DEBUG_APP__(0, "%s\n", vcString);

	return 0;
}
