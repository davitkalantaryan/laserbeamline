/*
 *	File		: pitz_rpi_comporteqfct.cpp
 *
 *	Created on	: 24 Mar, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */

#include "pitz_rpi_comporteqfct.hpp"
#include <map>

int g_nDebugApp = 1;

enum class SerialParity {
	None,
	Odd,
	Even,
	Mark,
	Space,
};

enum class SerialStopBits {
	One,
	OnePointFive,
	Two,
};


static std::map<std::string, pitz::rpi::ComPortEqFct*>	s_comPorts;

static std::string FindErrorString(void);
static PCWSTR StringFromSerialParity(SerialParity Parity);
static PCWSTR StringFromSerialStopBits(SerialStopBits StopBits);
static PCWSTR StringFromDtrControl(DWORD DtrControl);
static PCWSTR StringFromRtsControl(DWORD RtsControl);


pitz::rpi::ComPortEqFct::ComPortEqFct()
	:
	EqFct("NAME = location"),
	m_anyCommand(COMMAND_TYPE::ANY_COMMAND, 
		reinterpret_cast<TypeCallback>(&ComPortEqFct::CallbackFunction),
		"STRING.COMMAND executes any command provided", this),
	m_comPortName("COMPORT.NAME the name of com port", this),
	m_baudRate("BAUD_RATE property holds baud rate",this)
{
	m_comPortName.set_ro_access();
	m_baudRate.set_ro_access();
}

pitz::rpi::ComPortEqFct::~ComPortEqFct()
{
}

typedef const char* ConstCharPtrType;

int pitz::rpi::ComPortEqFct::WriteStringWithEnding(char* a_string, int a_str_len)
{
	a_string[a_str_len] = 10;
	a_string[a_str_len + 1] = 13;
	a_string[a_str_len + 2] = 13;
	a_string[a_str_len + 3] = 0;
	a_string[a_str_len + 4] = 0;
	DWORD dwWritten = m_serial.Write(a_string, a_str_len + 3);
	return (int)dwWritten;
}

int pitz::rpi::ComPortEqFct::CallbackFunction(D_fct* a_this, COMMAND_TYPET a_command, 
	EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct)
{
	switch (a_command)
	{
	case COMMAND_TYPE::ANY_COMMAND:
	{
#if 0
		ConstCharPtrType* cpcCommandPtr = (ConstCharPtrType*)a_command_arg;
		const char* cpcCommand = *cpcCommandPtr;
		int nStrLen = (int)strlen(cpcCommand);
		char* pcNewCommand = (char*)alloca(nStrLen + 5);
		memcpy(pcNewCommand, cpcCommand, nStrLen);
		WriteStringWithEnding(pcNewCommand, nStrLen);
#endif
	}
	break;
	default:
		break;
	}
	return 0;
}



int pitz::rpi::ComPortEqFct::fct_code()
{
	return (int)pitz::rpi::EQ_FCT_CODES::GEN_COM_PORT;
}


void pitz::rpi::ComPortEqFct::init(void)
{

	printf("--------------------- ComPortEqFct::init\n");

	DCB actualDcb;
	COMMTIMEOUTS aTimeouts;
	int nRet;

	__DEBUG_APP__(1, "version 4 serial_name=\"%s\"",m_comPortName.value());

	//if ((nRet = m_serial.OpenSerial(SERIAL_DEVICE_NAME)))
	if ((nRet = m_serial.OpenSerial(m_comPortName.value())))
	{
		std::string errString = FindErrorString();
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}

	if ((nRet = m_serial.GetCommStates(&actualDcb, &aTimeouts)))
	{
		std::string errString = FindErrorString();
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}

	__DEBUG_APP__(1,
		"                    baud = %d\n"
		"                  parity = %s\n"
		"               data bits = %d\n"
		"               stop bits = %s\n"
		"   XON/XOFF flow control = %s\n"
		" output DSR flow control = %s\n"
		" output CTS flow control = %s\n"
		"             DTR control = %s\n"
		"             RTS control = %s\n"
		" DSR circuit sensitivity = %s\n",
		actualDcb.BaudRate,
		StringFromSerialParity(SerialParity(actualDcb.Parity)),
		actualDcb.ByteSize,
		StringFromSerialStopBits(SerialStopBits(actualDcb.StopBits)),
		(actualDcb.fInX && actualDcb.fOutX) ? L"on" : L"off",
		actualDcb.fOutxDsrFlow ? L"on" : L"off",
		actualDcb.fOutxCtsFlow ? L"on" : L"off",
		StringFromDtrControl(actualDcb.fDtrControl),
		StringFromRtsControl(actualDcb.fRtsControl),
		actualDcb.fDsrSensitivity ? L"on" : L"off");

	//actualDcb.BaudRate = BOUD_RATE;
	actualDcb.BaudRate = m_baudRate.value();
	if ((nRet = m_serial.SetupCommState(&actualDcb, &aTimeouts)))
	{
		std::string errString = FindErrorString();
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}

	s_comPorts[m_comPortName.value()] = this;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////*/

pitz::rpi::ComPortUserEqFct::ComPortUserEqFct()
	:
	EqFct("NAME = location"),
	m_pComPort(NULL),
	m_comPortName("COMPORT.NAME the name of com port", this)
{
}


pitz::rpi::ComPortUserEqFct::~ComPortUserEqFct()
{
}


int pitz::rpi::ComPortUserEqFct::CallbackFunctionU(D_fct* a_this, COMMAND_TYPET a_command,
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	if (m_pComPort) { 
		m_pComPort->CallbackFunction(a_this,a_command,a_dcsAdr,a_fromUser,a_toUser,a_fct);
		return 0; 
	}
	return -2;
}


int pitz::rpi::ComPortUserEqFct::fct_code()
{
	return (int)EQ_FCT_CODES::GEN_COM_PORT_USER;
}


void pitz::rpi::ComPortUserEqFct::tryToInit(void)
{
	if(m_pComPort){return;}
	auto it = s_comPorts.find(m_comPortName.value());

	if (it == s_comPorts.end())
	{
		fprintf(stderr,"!!!!!!!!!!!!!!!! Does not exist\n");
		return;
	}
	m_pComPort = it->second;
}


void pitz::rpi::ComPortUserEqFct::post_init(void)
{
	printf("+++++++++++++++++++++++++ ComPortUser::post_init\n");
	tryToInit();

	if(!m_pComPort){
		std::string aLocName(m_comPortName.value(),6);
		std::transform(aLocName.begin(),aLocName.end(), aLocName.begin(),::toupper);
		add_location(EQ_FCT_CODES::GEN_COM_PORT, aLocName.c_str(), (void*)m_comPortName.value());
	}

	auto it = s_comPorts.find(m_comPortName.value());

	if (it == s_comPorts.end())
	{
		fprintf(stderr, "!!!!!!!!!!!!!!!!Problem\n");
		return;
	}
	m_pComPort = it->second;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////*/

static std::string FindErrorString(void)
{
	DWORD dwError = GetLastError();

	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		0, // Default language
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	);

	std::string ssReturn = (LPSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return ssReturn;
}



static PCWSTR StringFromSerialParity(SerialParity Parity)
{
	switch (Parity) {
	case SerialParity::None: return L"none";
	case SerialParity::Odd: return L"odd";
	case SerialParity::Even: return L"even";
	case SerialParity::Mark: return L"mark";
	case SerialParity::Space: return L"space";
	default: return L"[invalid parity]";
	}
}

static PCWSTR StringFromSerialStopBits(SerialStopBits StopBits)
{
	switch (StopBits) {
	case SerialStopBits::One: return L"1";
	case SerialStopBits::OnePointFive: return L"1.5";
	case SerialStopBits::Two: return L"2";
	default: return L"[invalid serial stop bits]";
	}
}

static PCWSTR StringFromDtrControl(DWORD DtrControl)
{
	switch (DtrControl) {
	case DTR_CONTROL_ENABLE: return L"on";
	case DTR_CONTROL_DISABLE: return L"off";
	case DTR_CONTROL_HANDSHAKE: return L"handshake";
	default: return L"[invalid DtrControl value]";
	}
}


static PCWSTR StringFromRtsControl(DWORD RtsControl)
{
	switch (RtsControl) {
	case RTS_CONTROL_ENABLE: return L"on";
	case RTS_CONTROL_DISABLE: return L"off";
	case RTS_CONTROL_HANDSHAKE: return L"handshake";
	case RTS_CONTROL_TOGGLE: return L"toggle";
	default: return L"[invalid RtsControl value]";
	}
}
