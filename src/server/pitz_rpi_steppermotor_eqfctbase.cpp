
// pitz_rpi_steppermotor_eqfctbase.cpp
// 2017 Jul 20

#include "pitz_rpi_steppermotor_eqfctbase.hpp"
#include <math.h>

#define IS_NOT_INITED(...) \
	((!m_com) || (m_nControllerAddress<1) || (m_nControllerAddress>16))
#define FLOAT_ERR_VAL	-100000.

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
	m_velocity("VELOCITY",this),
	m_isBusy("IS.BUSY",this)
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

	m_isBusy.set_ro_access();

	// to be done
	m_velocity.set_ro_access();
}


pitz::rpi::StepperMotor::EqFctBase::~EqFctBase()
{
}


void pitz::rpi::StepperMotor::EqFctBase::post_init(void)
{
	double fValue;
	int nLen, nMotorNumber = m_motorNumber.value();
	char vcBuffer[1024];

	ComPortUserEqFct::post_init();
	printf("!!!!!!!!!!!pitz::rpi::StepperMotor::EqFctBase::post_init(void)\n");
	if(!m_pComPort){return;}

	nLen=snprintf(vcBuffer,1023,"%d nidentify\r\n",nMotorNumber);
	printf("!!!!!!!!!!!! identify_string=\"%s\"\n", vcBuffer);
	m_pComPort->WriteByteStream(vcBuffer, nLen);
	nLen=m_pComPort->ReadComRaw(vcBuffer, 1023,10004);
	if(nLen<=0){
		printf("!!!!!!!!!!!!!!!! Identify failled!\n");
		m_statusStr.set_value("Identify failled");
		m_statusInt.set_value(-1);
		//return;
	}
	else{
		vcBuffer[nLen] = 0;
		printf("!!!!!!!!!!!!!!!!!!!!!identify_answer=\"%s\"\n", vcBuffer);
		m_statusStr.set_value(vcBuffer);
		m_statusInt.set_value(0);
	}

	nLen = snprintf(vcBuffer, 1023, "%d reset\r\n", nMotorNumber);
	m_pComPort->WriteByteStream(vcBuffer, nLen);

	nLen = snprintf(vcBuffer, 1023, "%d ncal\r\n", nMotorNumber);
	m_pComPort->WriteByteStream(vcBuffer, nLen);

	m_currentPos.SetComPortAndAddress(m_pComPort->comPtr(), nMotorNumber);
	m_isBusy.SetComPortAndAddress(m_pComPort->comPtr(), nMotorNumber);

	fValue = m_currentPos.GetPosFromDeviceRaw();
	while((abs(fValue - 0.)>0.001) && (fValue !=-1.)){
		Sleep(1);
		fValue = m_currentPos.GetPosFromDeviceRaw();
	}
	Sleep(5);
	m_currentPos.ApplyCurPos();
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


/*/////////////////////////////////*/

pitz::rpi::ControllerRaw::ControllerRaw()
{
	m_com = NULL;
	m_nControllerAddress = -1;
}

void pitz::rpi::ControllerRaw::SetComPortAndAddress(common::serial::ComPort* a_com, int a_address)
{
	m_com = a_com;
	m_nControllerAddress = a_address;
}


/*///////////////////////////////////////////////////*/

pitz::rpi::D_curPos::D_curPos(const char* a_propName, EqFct* a_loc)
	:
	VALUE_TYPE(a_propName,a_loc)
{
}

void pitz::rpi::D_curPos::set(EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	int nRet;
	double fValue;
	char vcBuffer[128];

	if (IS_NOT_INITED()) {
		// send error to user
		return;
	}

	fValue=(double)a_fromUser->GET_FNC();

	nRet = snprintf(vcBuffer, 128, "%lf %d nm\r\n", fValue,m_nControllerAddress);
	nRet=m_com->writeC(vcBuffer, nRet);
	if(nRet<0){return;}
}


double pitz::rpi::D_curPos::GetPosFromDeviceRaw()
{
	int nRet;
	char vcBuffer[128];

	if (IS_NOT_INITED()) {
		// send error to user
		return FLOAT_ERR_VAL;
	}

	nRet = snprintf(vcBuffer, 128, "%d np\r\n", m_nControllerAddress);
	m_com->writeC(vcBuffer, nRet);
	nRet = m_com->Read2(vcBuffer, 16, 100, 10);
	if (nRet<1) {
		// send error to user
		return FLOAT_ERR_VAL;
	}
	vcBuffer[nRet] = 0;
	return atof(vcBuffer);
}


void pitz::rpi::D_curPos::get(EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	double fValue;

	fValue = GetPosFromDeviceRaw();
	if(fValue== FLOAT_ERR_VAL){
		// send error to user
		return;
	}

	a_toUser->set((RAW_VALUE_TYPE)fValue);
	VALUE_TYPE::set_value((RAW_VALUE_TYPE)fValue);
}


void pitz::rpi::D_curPos::ApplyCurPos()
{
	char vcBuffer[128];
	double fValue = (double)value();
	int nRet = snprintf(vcBuffer, 128, "%lf %d nm\r\n", fValue, m_nControllerAddress);
	m_com->writeC(vcBuffer, nRet);
}


/* ///////////////////////////////////////////// */
pitz::rpi::D_isBusy::D_isBusy(const char* a_propName, EqFct* a_loc)
	:
	D_int(a_propName,a_loc)
{
}


int pitz::rpi::D_isBusy::GetIsBusyRaw()
{
	int nRet;
	char vcBuffer[128];

	//printf("m_com=%p, m_nControllerAddress=%d\n",m_com,m_nControllerAddress);
	if (IS_NOT_INITED()) {return -1;}
	nRet = snprintf(vcBuffer, 128, "%d nst\r\n", m_nControllerAddress);
	m_com->writeC(vcBuffer, nRet);
	nRet = m_com->Read2(vcBuffer, 16, 100, 10);
	if (nRet<1) {return -2;}
	vcBuffer[nRet] = 0;
	return atoi(vcBuffer);
}


void pitz::rpi::D_isBusy::get(EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	int fValue;

	fValue = GetIsBusyRaw();
	if (fValue < 0) {
		// send error to user
		D_int::get(a_dcsAdr, a_fromUser, a_toUser, a_fct);
		return;
	}

	a_toUser->set(fValue);
	D_int::set_value(fValue);
}
