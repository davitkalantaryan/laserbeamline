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

int g_nDebugLevel = 0;

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


pitz::rpi::ComPortEqFct::ComPortEqFct(const std::string& a_ending,const char* a_comName)
	:
	EqFct("NAME = location"),
	m_anyCommand(COMMAND_TYPE::SET_ANY_ASCII_STRING, 
		reinterpret_cast<TypeCallback>(&ComPortEqFct::CallbackFunction2),
		"STRING.COMMAND executes any command provided", this),
	m_comPortName("COMPORT.NAME the name of com port", this),
	m_baudRate("BAUD_RATE property holds baud rate",this),

	m_asciiEnding(a_ending)
{
	m_strComName = a_comName ? a_comName : "";

	m_comPortName.set_ro_access();
	m_baudRate.set_ro_access();
}


pitz::rpi::ComPortEqFct::~ComPortEqFct()
{
}


#if 0
const ::common::serial::ComPort& pitz::rpi::ComPortEqFct::ComPort()const
{
	return m_serial3;
}
#endif


int pitz::rpi::ComPortEqFct::ReadComRaw(void* a_buffer, int a_nBufLen)
{
	return m_serial3.readC(a_buffer, a_nBufLen);
}


int pitz::rpi::ComPortEqFct::ReadComRaw(void* a_buffer, int a_nBufLen, int a_nTimeoutMs)
{
	return m_serial3.readC(a_buffer, a_nBufLen, a_nTimeoutMs);
}


int pitz::rpi::ComPortEqFct::WriteStringWithEnding2(char* a_string, int a_str_len)
{
	int dwWritten;

	// ending for polux controller is "\r\n"
	memcpy(a_string+ a_str_len,m_asciiEnding.c_str(),m_asciiEnding.length());

	m_mutexForSerial.lock();
	dwWritten = m_serial3.writeC(a_string, a_str_len +m_asciiEnding.length());
	m_mutexForSerial.unlock();

	return dwWritten;
}


int pitz::rpi::ComPortEqFct::WriteByteStream(const void* a_byteStream, int a_dataLen)
{
	int dwWritten;

	m_mutexForSerial.lock();
	dwWritten = m_serial3.writeC(a_byteStream, a_dataLen);
	m_mutexForSerial.unlock();

	return dwWritten;
}


int pitz::rpi::ComPortEqFct::CallbackFunction2(D_fct* a_this, COMMAND_TYPET a_command, 
	EqAdr * dcsAdr, EqData *a_fromUser, EqData * toUser, EqFct * fct, 
	void* a_pData, int a_nDataLen)
{
	int nReturn(-1);

	switch (a_command)
	{
	case COMMAND_TYPE::SET_DIRECT_ASCII:
	{
		char* pcAscii = (char*)a_pData;
		int nStrLen((int)strlen(pcAscii));
		char* pcNewCommand = (char*)alloca(nStrLen + m_asciiEnding.length() + 4);
		memcpy(pcNewCommand, pcAscii, nStrLen);
		nReturn = WriteStringWithEnding2(pcNewCommand, nStrLen);
	}
		return 0;
	case COMMAND_TYPE::GET_DIRECT_ASCII:
		return -2;
	case COMMAND_TYPE::SET_DIRECT_BYTE_STREAM:
		m_mutexForSerial.lock();
		m_serial3.writeC(a_pData, a_nDataLen);
		m_mutexForSerial.unlock();
		return 0;
	case COMMAND_TYPE::GET_DIRECT_BYTE_STREAM:
		m_mutexForSerial.lock();
		m_serial3.writeC(a_pData, a_nDataLen);
		m_mutexForSerial.unlock();
		return -2;
	case COMMAND_TYPE::SET_ANY_ASCII_STRING:
	{
		std::string strAnyCommand = a_fromUser->get_string();
		int nStrLen(strAnyCommand.length());
		char* pcNewCommand = (char*)alloca(nStrLen + m_asciiEnding.length()+4);
		memcpy(pcNewCommand, strAnyCommand.c_str(), nStrLen);
		nReturn = WriteStringWithEnding2(pcNewCommand, nStrLen);
	}
	return 0;

	case COMMAND_TYPE::SET_ANY_BYTE_STREAM:
	{
		u_char* pArray;
		int nByteStreamLen;
		a_fromUser->get_byte(&nByteStreamLen, &pArray);
		if(nByteStreamLen>0){
			m_mutexForSerial.lock();
			m_serial3.writeC(pArray, nByteStreamLen);
			m_mutexForSerial.unlock();
		}
	}
	return 0;

	default:
		break;
	}
	return nReturn;
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

	if(m_strComName!=std::string("")){
		m_comPortName.set_value(m_strComName.c_str());
	}
	else{
		m_strComName = m_comPortName.value();
	}

	__DEBUG_APP__(1, "version 4 serial_name=\"%s\"",m_comPortName.value());

	if ((nRet = m_serial3.OpenCom(m_comPortName.value())))
	{
		std::string errString = FindErrorString();
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}

	if ((nRet = m_serial3.GetCommStates(&actualDcb, &aTimeouts)))
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

#if 1
	//actualDcb.BaudRate = BOUD_RATE;
	actualDcb.BaudRate = m_baudRate.value();
	if ((nRet = m_serial3.SetupCommState(&actualDcb, &aTimeouts)))
	{
		std::string errString = FindErrorString();
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}
#endif

	s_comPorts[m_comPortName.value()] = this;
	m_proxyServer.SetMutex(&m_mutexForSerial);
	m_proxyServer.SetIoDevice(&m_serial3);
	m_threadForProxy = STDN::thread(&ComPortEqFct::ThreadForProxyFnc,this);
}


void pitz::rpi::ComPortEqFct::cancel(void)
{
	m_proxyServer.StopServerN();
	m_threadForProxy.join();
}


void pitz::rpi::ComPortEqFct::ThreadForProxyFnc(void)
{
	printf("!!!!!!!!!!!!!! version 11\n");
	m_proxyServer.SetIoDevice(&m_serial3); // not necessary
	m_proxyServer.StartServerN();
	m_proxyServer.SetIoDevice(NULL); // not necessary
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
	EqAdr * a_dcsAdr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct,
	void* a_pData, int a_nDataLen)
{
	if (m_pComPort) { 
		m_pComPort->CallbackFunction2(
			a_this,a_command,
			a_dcsAdr,a_fromUser,a_toUser, a_fct,
			a_pData, a_nDataLen);
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


/*///////////////////////////////////////////////////////////////*/
pitz::rpi::D_void::D_void(
	COMMAND_TYPET a_command, TypeCallback a_fpCallback,
	const char* a_pn, EqFct* a_par)
	:
	New_D_types<D_fct>(a_command, a_fpCallback, a_pn, a_par)
{
}


pitz::rpi::D_void::~D_void()
{
}


void pitz::rpi::D_void::write(fstream &ofile)
{
	char        buf[200];
	snprintf(buf, sizeof(buf), "%s: \n", base_name.c_str());
	flush(ofile, buf, 1);
}


/*///////////////////////////////////////////////////////////////*/
pitz::rpi::D_void_fix_string::D_void_fix_string(TypeCallback a_fpCallback,const char* a_pn, EqFct* a_par)
	:
	D_void(COMMAND_TYPE::SET_DIRECT_ASCII, a_fpCallback, a_pn, a_par)
{
}


pitz::rpi::D_void_fix_string::~D_void_fix_string()
{
}


void pitz::rpi::D_void_fix_string::SetFixedString(const std::string& a_fixString)
{
	m_fixString = a_fixString;
}


void pitz::rpi::D_void_fix_string::set(EqAdr*a_dcsAdr,EqData*a_fromUser,EqData*a_toUser,EqFct*a_fct)
{
	D_fct::set(a_dcsAdr, a_fromUser, a_toUser, a_fct);
	(a_fct->*m_fCallback)(this, m_command, a_dcsAdr, a_fromUser, a_toUser, a_fct,
		(void*)m_fixString.c_str(), (int)m_fixString.length());
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
