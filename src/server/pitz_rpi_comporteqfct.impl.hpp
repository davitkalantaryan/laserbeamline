/*
 *	File		: pitz_rpi_comporteqfct.tos
 *
 *	Created on	: 24 Mar, 2017
 *	Author		: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef __pitz_rpi_comporteqfct_impl_hpp__
#define __pitz_rpi_comporteqfct_impl_hpp__

#ifndef __pitz_rpi_comporteqfct_hpp__
//#error this file should not be used directly
#include "pitz_rpi_comporteqfct.hpp"
#endif

template<typename D_type>
pitz::rpi::New_D_types<D_type>::New_D_types(
	COMMAND_TYPET a_command, TypeCallback a_fpCallback,
	const char* a_pn, EqFct* a_par)
	:
	D_type(a_pn, a_par),
	m_fCallback(a_fpCallback),
	m_command(a_command)
{
}


template<typename D_type>
pitz::rpi::New_D_types<D_type>::~New_D_types()
{
}


template<typename D_type>
void pitz::rpi::New_D_types<D_type>::set(EqAdr * a_adr, EqData *a_fromUser, EqData * a_toUser, EqFct * a_fct)
{
	D_type::set(a_adr, a_fromUser, a_toUser, a_fct);
	(a_fct->*m_fCallback)(this, m_command, a_adr,a_fromUser,a_toUser,a_fct);
}


#endif  // #ifndef __pitz_rpi_comporteqfct_impl_hpp__
