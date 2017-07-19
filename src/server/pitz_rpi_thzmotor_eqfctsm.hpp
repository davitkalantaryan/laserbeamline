/*
 *	File		: pitz_rpi_thzmotor_eqfctsm.hpp
 *
 *	Created on	: 18 Jul, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef __pitz_rpi_thzmotor_eqfctsm_hpp__
#define __pitz_rpi_thzmotor_eqfctsm_hpp__

#include "pitz_rpi_comporteqfct.hpp"

#ifndef VALUE_TYPE
#define VALUE_TYPE	D_float
#endif

namespace pitz {namespace rpi{ 
	
namespace COMMAND_TYPE{enum {YAG_CAMERA_SET=ANY_COMMAND+1,GO_LEFT,GO_RIGHT};}

namespace ThzMotor{

namespace CAMERA_POS{enum {YAG_0_POS=0,YAG_DZ_MINUS20=-1,YAG_DZ_PLUS20=1};}

class EqFctThzMot : public pitz::rpi::ComPortUserEqFct
{
public:
	EqFctThzMot();
	~EqFctThzMot();

protected:
	int  fct_code();
	void post_init(void);

	int CallbackFunctionSM(D_fct* a_this, COMMAND_TYPET command, 
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);

protected:
	D_int						m_discretPos;
	D_void						m_doIt;
	D_int						m_motorNumber;
	VALUE_TYPE					m_valueZero;
	VALUE_TYPE					m_valueMinus20;
	VALUE_TYPE					m_valuePlus20;
	D_void						m_goLeft;
	D_void						m_goRight;
	New_D_types<D_string>		m_anyString;
};


}}}


#endif // #ifndef __pitz_rpi_steppermotor_eqfctsm_hpp__
