/*
 *	File		: pitz_rpi_steppermotor_eqfctlowscr3.hpp
 *
 *	Created on	: 24 Mar, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef __pitz_rpi_steppermotor_eqfctlowscr3_hpp__
#define __pitz_rpi_steppermotor_eqfctlowscr3_hpp__

#include "pitz_rpi_steppermotor_eqfctbase.hpp"

namespace pitz {namespace rpi{ 
	
namespace StepperMotor{

namespace CAMERA_POS { enum { YAG_0_POS = 0, YAG_DZ_MINUS20 = -1, YAG_DZ_PLUS20 = 1 }; }

class EqFctLowScr3 : public EqFctBase
{
public:
	EqFctLowScr3();
	~EqFctLowScr3();

protected:
	int  fct_code();
	void post_init(void);

	int CallbackFunctionLowScr3(
		D_fct* a_this, COMMAND_TYPET command,
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct,
		void* data, int dataLen);

protected:
	D_int						m_discretPos;
	D_void						m_doIt;	
	VALUE_TYPE					m_valueZero;
	VALUE_TYPE					m_valueMinus20;
	VALUE_TYPE					m_valuePlus20;
};


}}}


#endif // #ifndef __pitz_rpi_steppermotor_eqfctlowscr3_hpp__
