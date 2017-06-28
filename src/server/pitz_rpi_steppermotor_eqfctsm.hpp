/*
 *	File		: pitz_rpi_steppermotor_eqfctsm.hpp
 *
 *	Created on	: 24 Mar, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef __pitz_rpi_steppermotor_eqfctsm_hpp__
#define __pitz_rpi_steppermotor_eqfctsm_hpp__

#include "pitz_rpi_comporteqfct.hpp"

#define D_TYPE_IN_USE	D_double
//#define TYPE_IN_USE2	double

namespace pitz {namespace rpi{ 
	
namespace COMMAND_TYPE{enum {YAG_CAMERA_POS=ANY_COMMAND+1};}

namespace StepperMotor{

namespace CAMERA_POS{enum {YAG_0_POS,YAG_DZ_PLUS20,YAG_DZ_MINUS20};}

class D_void : New_D_types<D_fct>
{
public:
	D_void(COMMAND_TYPET a_command, TypeCallback fpCallback, const char* pn, EqFct* par);
	virtual void write(fstream &ofile);
};

class EqFctSM : public pitz::rpi::ComPortUserEqFct
{
public:
	EqFctSM();
	~EqFctSM();

protected:
	int  fct_code();
	void post_init(void);

	int CallbackFunctionSM(D_fct* a_this, COMMAND_TYPET command, 
		EqAdr * dcsAdr, EqData *fromUser, EqData * toUser, EqFct * fct);

protected:
	D_int						m_discretPos;
	D_void						m_doIt;
	D_int						m_motorNumber;
};


}}}


#endif // #ifndef __pitz_rpi_steppermotor_eqfctsm_hpp__
