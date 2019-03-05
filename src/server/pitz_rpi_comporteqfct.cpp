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
#include <common/base.hpp>

#define POLUX_ASCII_ENDING		"\r\n"
#define POLUX_ASCII_ENDING_LEN	2

#ifdef _WIN32
#define WaitForSignal()	SleepEx(INFINITE,TRUE)
#else
#define WaitForSignal() sigsuspend(nullptr)
#endif

int g_nDebugLevel = 0;


namespace __private{ namespace pitz{ namespace rpi{

class PrivateComPortEqFct : public ::pitz::rpi::ComPortEqFct
{
public:
	static void ReadClbkPrivate(void* clbkData, int error, const char* data, int dataLen);
	static void WriteClbkPrivate(void* clbkData, int error, const char* data, int dataLen);
};

}}}  // namespace __private{ namespace pitz{ namespace rpi{


static std::map<std::string, pitz::rpi::ComPortEqFct*>	s_comPorts;


pitz::rpi::ComPortEqFct::ComPortEqFct(const char* a_comName)
	:
	EqFct("NAME = location"),
	m_serial(this,&__private::pitz::rpi::PrivateComPortEqFct::ReadClbkPrivate,&__private::pitz::rpi::PrivateComPortEqFct::WriteClbkPrivate),
	m_anyCommand(COMMAND_TYPE::SET_ANY_ASCII_STRING, 
		reinterpret_cast<TypeCallback>(&ComPortEqFct::CallbackFunction2),
		"STRING.COMMAND executes any command provided", this),
	m_comPortName("COMPORT.NAME the name of com port", this),
	m_baudRate("BAUD_RATE property holds baud rate",this)
{
	m_pClientSocket = nullptr;
	m_proxyRuns = 0;
	m_pcStrComNameRaw = a_comName ? strdup(a_comName) : nullptr;

	m_comPortName.set_ro_access();
	m_baudRate.set_ro_access();
}


pitz::rpi::ComPortEqFct::~ComPortEqFct()
{
	free(m_pcStrComNameRaw);
}


int pitz::rpi::ComPortEqFct::ReadComRaw(void* a_buffer, int a_nBufLen)
{
	return m_serial.readC(a_buffer, a_nBufLen);
}


int pitz::rpi::ComPortEqFct::WriteStringWithEnding2(char* a_string, int a_str_len)
{
	int dwWritten;

	// ending for polux controller is "\r\n"
	memcpy(a_string+ a_str_len,POLUX_ASCII_ENDING, POLUX_ASCII_ENDING_LEN);

	m_mutexForSerial.lock();
	dwWritten = m_serial.writeC(a_string, a_str_len+POLUX_ASCII_ENDING_LEN);
	m_mutexForSerial.unlock();

	return dwWritten;
}


int pitz::rpi::ComPortEqFct::WriteByteStream(const void* a_byteStream, int a_dataLen)
{
	int dwWritten;

	m_mutexForSerial.lock();
	dwWritten = m_serial.writeC(a_byteStream, a_dataLen);
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
		char* pcNewCommand = (char*)alloca(nStrLen + POLUX_ASCII_ENDING_LEN + 4);
		memcpy(pcNewCommand, pcAscii, nStrLen);
		nReturn = WriteStringWithEnding2(pcNewCommand, nStrLen);
	}
		return 0;
	case COMMAND_TYPE::GET_DIRECT_ASCII:
		return -2;
	case COMMAND_TYPE::SET_DIRECT_BYTE_STREAM:
		m_mutexForSerial.lock();
		m_serial.writeC(a_pData, a_nDataLen);
		m_mutexForSerial.unlock();
		return 0;
	case COMMAND_TYPE::GET_DIRECT_BYTE_STREAM:
		m_mutexForSerial.lock();
		m_serial.writeC(a_pData, a_nDataLen);
		m_mutexForSerial.unlock();
		return -2;
	case COMMAND_TYPE::SET_ANY_ASCII_STRING:
	{
		std::string strAnyCommand = a_fromUser->get_string();
		int nStrLen(strAnyCommand.length());
		char* pcNewCommand = (char*)alloca(nStrLen + POLUX_ASCII_ENDING_LEN +4);
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
			m_serial.writeC(pArray, nByteStreamLen);
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

	if(m_pcStrComNameRaw){
		m_comPortName.set_value(m_pcStrComNameRaw);
		free(m_pcStrComNameRaw);
		m_pcStrComNameRaw = nullptr;
	}

	__DEBUG_APP__(1, "version 4 serial_name=\"%s\"",m_comPortName.value());

	if ((nRet = m_serial.openC(m_comPortName.value())))
	{
		std::string errString;
		nRet=::common::MakeErrorReport(nullptr,nullptr,&errString);
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}

	if ((nRet = m_serial.GetCommStates(&actualDcb, &aTimeouts)))
	{
		std::string errString;
		nRet = ::common::MakeErrorReport(nullptr, nullptr, &errString);
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during getting state !\n");
		return;
	}

	::common::io::serial::MakeStatisticForCom(&m_serial);

	//actualDcb.BaudRate = BOUD_RATE;
	actualDcb.BaudRate = m_baudRate.value();
	if ((nRet = m_serial.SetupCommState(&actualDcb, &aTimeouts)))
	{
		std::string errString;
		nRet = ::common::MakeErrorReport(nullptr, nullptr, &errString);
		set_error(nRet, errString, nRet);
		__DEBUG_APP_BASE__(0, stderr, "error during openning !\n");
		return;
	}

	s_comPorts[m_comPortName.value()] = this;
	m_proxyRuns = 1;
	m_threadForProxy = STDN::thread(&ComPortEqFct::ThreadForProxyFnc,this);
}


void pitz::rpi::ComPortEqFct::cancel(void)
{
	m_proxyRuns = 0;
	m_proxy.Stop();
	m_threadForProxy.join();
}


void pitz::rpi::ComPortEqFct::ThreadForProxyFnc(void)
{
	common::io::async::Base* vDevices[2];
	common::io::async::IoContext proxyContext = common::io::async::GetIoContext();

	vDevices[0] = &m_serial;

	while(m_proxyRuns){
		WaitForSignal();
		if(m_pClientSocket){
			vDevices[1] = m_pClientSocket;
			m_proxy.SetDevices(vDevices);
			m_proxy.Start();
		}
	}
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
	//flush(ofile, buf, 1);
	ofile.write(buf, strlen(buf));
	ofile.flush();
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

namespace __private{ namespace pitz{ namespace rpi{

void PrivateComPortEqFct::ReadClbkPrivate(void* clbkData, int error, const char* data, int dataLen)
{
}


void PrivateComPortEqFct::WriteClbkPrivate(void* clbkData, int error, const char* data, int dataLen)
{
}

}}}  // namespace __private{ namespace pitz{ namespace rpi{
