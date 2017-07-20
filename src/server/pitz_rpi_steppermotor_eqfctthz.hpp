/*
 *	File		: pitz_rpi_steppermotor_eqfctthz.hpp
 *
 *	Created on	: 18 Jul, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef __pitz_rpi_steppermotor_eqfctthz_hpp__
#define __pitz_rpi_steppermotor_eqfctthz_hpp__

#include "pitz_rpi_steppermotor_eqfctbase.hpp"

namespace pitz {namespace rpi{ 

namespace COMMAND_TYPE { enum {CHANGE_BY=BASE_LAST+1}; }
	
namespace StepperMotor {

class EqFctThz : public EqFctBase
{
public:
	EqFctThz();
	~EqFctThz();

protected:
	int  fct_code();
	void post_init(void);

	int CallbackFunctionTHz(
		D_fct* a_this, COMMAND_TYPET command, 
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct,
		void* data, int dataLen);

protected:
	// members should be here
	VALUE_TYPE			m_moveAbsolute;
	D_void				m_setTo;
	VALUE_TYPE			m_moveRelative;
	D_void				m_changeBy;
	VALUE_TYPE			m_velocitySP;
	VALUE_TYPE			m_velocityRDBK;
	D_void_fix_string	m_velocityCmd;
	D_void_fix_string	m_homeCmd;
};


}}}


#endif // #ifndef __pitz_rpi_steppermotor_eqfctthz_hpp__
