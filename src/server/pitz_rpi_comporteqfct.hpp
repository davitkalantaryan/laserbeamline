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

extern int g_nDebugApp;

#define __DEBUG_APP_BASE__(__num,__fd,...) \
	do{if((__num)<=g_nDebugApp){\
		fprintf((__fd),"fn:%s, ln:%d  ",__FUNCTION__,__LINE__);fprintf((__fd),__VA_ARGS__);fprintf((__fd),"\n");}}while(0)

#ifndef __DEBUG_APP__
#define __DEBUG_APP__(__num,...) __DEBUG_APP_BASE__((__num),stdout,__VA_ARGS__)
#endif  // #ifndef __DEBUG_APP__

namespace pitz{ namespace rpi{

//enum class EQ_FCT_CODES{GEN_COM_PORT=304, GEN_COM_PORT_USER = 305, SM_COM_USER = 306};
namespace EQ_FCT_CODES{enum {GEN_COM_PORT=304, GEN_COM_PORT_USER = 305, SM_COM_USER = 306};}

typedef int COMMAND_TYPET;
namespace COMMAND_TYPE{enum {ANY_COMMAND};}

typedef int (EqFct::*TypeCallback)(D_fct* prop, COMMAND_TYPET command,
	EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);

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

class ComPortEqFct : public EqFct
{
public:
	ComPortEqFct();
	virtual ~ComPortEqFct();

	int WriteStringWithEnding(char* string, int str_len);
	virtual int CallbackFunction(
		D_fct* a_this, COMMAND_TYPET command,
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);

protected:
	virtual int  fct_code();
	virtual void init(void);

protected:
	pitz::rpi::tools::Serial	m_serial;
	New_D_types<D_string>		m_anyCommand;
	D_string					m_comPortName;
	D_int						m_baudRate;
};


class ComPortUserEqFct : public EqFct
{
public:
	ComPortUserEqFct();
	virtual ~ComPortUserEqFct();

protected:
	virtual int		CallbackFunctionU(D_fct* a_this, COMMAND_TYPET command,
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);
	virtual int		fct_code();
	virtual void	post_init(void);

	void			tryToInit(void);

protected:
	ComPortEqFct*	m_pComPort;
	D_string		m_comPortName;
};

}}


#include "pitz_rpi_comporteqfct.tos"


#endif // #ifndef __pitz_rpi_comporteqfct_hpp__

