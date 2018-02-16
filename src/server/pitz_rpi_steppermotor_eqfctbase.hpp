
// pitz_rpi_steppermottor_eqfctbase.hpp
// 2017 Jul 19

#ifndef __pitz_rpi_steppermottor_eqfctbase_hpp__
#define __pitz_rpi_steppermottor_eqfctbase_hpp__

#include "pitz_rpi_comporteqfct.hpp"

#ifndef VALUE_TYPE
#define VALUE_TYPE	D_float
#endif
#ifndef RAW_VALUE_TYPE
#define RAW_VALUE_TYPE float
#endif
#ifndef GET_FNC
#define GET_FNC get_float
#endif

namespace pitz{ namespace rpi{

class ControllerRaw
{
public:
	ControllerRaw();
	virtual ~ControllerRaw() {}
	void SetComPortAndAddress(common::serial::ComPort* a_com, int address);

protected:
	common::serial::ComPort* m_com;
	int m_nControllerAddress;
};

class D_curPos : public VALUE_TYPE, public ControllerRaw
{
public:
	D_curPos(const char* propName, EqFct* loc);

	void set(EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct)override;
	void get(EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct)override;
	void ApplyCurPos();
	double GetPosFromDeviceRaw();
};


class D_isBusy : public D_int, public ControllerRaw
{
public:
	D_isBusy(const char* propName, EqFct* loc);

	void get(EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct)override;
	int  GetIsBusyRaw();
};

namespace STP_MTR_EQ_FCT_CODES {
	enum {
		LOW_SCR3 = EQ_FCT_CODES::GEN_COM_PORT_USER+1,
		THZ_MOT
	};
}

namespace COMMAND_TYPE { enum { ACTUALIZE_POSITION=COM_PORT_LAST+1,GO_LEFT,GO_RIGHT,BASE_LAST}; }

namespace StepperMotor{

class EqFctBase : public ComPortUserEqFct
{
public:
	EqFctBase();
	virtual ~EqFctBase();

	virtual void post_init(void) override;
	virtual int CallbackFunctionSM(D_fct* a_this, COMMAND_TYPET command,
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct,
		void* aata, int dataLen);

protected:
	D_int						m_motorNumber;
	D_void						m_goLeft;
	D_void						m_goRight;
	VALUE_TYPE					m_upperLimit;
	VALUE_TYPE					m_lowerLimit;
	D_curPos					m_currentPos;
	New_D_types<D_string>		m_anyString;
	D_int						m_statusInt;
	D_string					m_statusStr;
	D_void_fix_string			m_abortCmd;
	VALUE_TYPE					m_velocity;
	D_isBusy					m_isBusy;

};

} // namespace StepperMotor{

}}


#endif  // #ifndef __pitz_rpi_steppermottor_eqfctbase_hpp__
