/*
 *	File		: pitz_rpi_comporteqfct.hpp
 *
 *	Created on	: 24 Mar, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef __pitz_rpi_comporteqfct_hpp__
#define __pitz_rpi_comporteqfct_hpp__

#include <eq_fct.h>
#include "pitz_rpi_tools_serial.hpp"
#include "tools_comportserver.hpp"

extern int g_nDebugLevel;

#define __DEBUG_APP_BASE__(__num,__fd,...) \
	do{if((__num)<=g_nDebugLevel){\
		fprintf((__fd),"fn:%s, ln:%d  ",__FUNCTION__,__LINE__);fprintf((__fd),__VA_ARGS__);fprintf((__fd),"\n");}}while(0)

#ifndef __DEBUG_APP__
#define __DEBUG_APP__(__num,...) __DEBUG_APP_BASE__((__num),stdout,__VA_ARGS__)
#endif  // #ifndef __DEBUG_APP__

namespace pitz{ namespace rpi{

namespace EQ_FCT_CODES{enum {
	GEN_COM_PORT=304, 
	GEN_COM_PORT_USER = 305
};}

typedef int COMMAND_TYPET;
namespace COMMAND_TYPE{enum {
	SET_DIRECT_ASCII,
	GET_DIRECT_ASCII,
	SET_DIRECT_BYTE_STREAM,
	GET_DIRECT_BYTE_STREAM,
	SET_ANY_ASCII_STRING,
	SET_ANY_BYTE_STREAM,
	COM_PORT_LAST
};}

typedef int (EqFct::*TypeCallback)(D_fct* prop, COMMAND_TYPET command,
	EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct,
	void* data, int dataLen);

template <typename D_type>
class New_D_types : public D_type
{
public:
	New_D_types(COMMAND_TYPET a_command, TypeCallback fpCallback,const char* pn, EqFct* par);
	virtual ~New_D_types();

protected:
	virtual void set(EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);

protected:
	TypeCallback	m_fCallback;
	COMMAND_TYPET	m_command;
};


class D_void : public New_D_types<D_fct>
{
public:
	D_void(COMMAND_TYPET a_command, TypeCallback fpCallback, const char* pn, EqFct* par);
	virtual ~D_void();
	virtual void write(fstream &ofile);
};


class D_void_fix_string : public D_void
{
public:
	D_void_fix_string(TypeCallback fpCallback, const char* pn, EqFct* par);
	virtual ~D_void_fix_string();

	void SetFixedString(const std::string& fixString);

protected:
	virtual void set(EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);

protected:
	std::string	m_fixString;
};


class ComPortEqFct : public EqFct
{
public:
	ComPortEqFct(const std::string& ending,const char* comName);
	virtual ~ComPortEqFct();

	int WriteStringWithEnding2(char* string, int str_len);
	int WriteByteStream(const void* byteStream, int dataLen);
	virtual int CallbackFunction2(
		D_fct* a_prop, COMMAND_TYPET command,
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fctLoc,
		void* data, int dataLen);

protected:
	virtual int  fct_code();
	virtual void init(void);
	virtual void cancel(void)override;
	void ThreadForProxyFnc(void);

protected:
	pitz::rpi::tools::Serial	m_serial2;
	New_D_types<D_string>		m_anyCommand;
	D_string					m_comPortName;
	D_int						m_baudRate;

	std::string					m_strComName;
	std::string					m_asciiEnding;
	::tools::ComServer			m_comServer;
	STDN::mutex					m_mutexForSerial;
	STDN::thread				m_threadForProxy;
};


class ComPortUserEqFct : public EqFct
{
public:
	ComPortUserEqFct();
	virtual ~ComPortUserEqFct();

protected:
	virtual int		CallbackFunctionU(D_fct* a_prop, COMMAND_TYPET command,
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct,
		void* data, int dataLen);
	virtual int		fct_code();
	virtual void	post_init(void);

	void			tryToInit(void);

protected:
	ComPortEqFct*		m_pComPort;
	D_string			m_comPortName;
};

}}


#include "pitz_rpi_comporteqfct.impl.hpp"


#endif // #ifndef __pitz_rpi_comporteqfct_hpp__

