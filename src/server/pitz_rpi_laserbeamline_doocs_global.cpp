// 
// file:			pitz_rpi_laserbeamline_doocs_global.cpp 
// created on:		2019 Feb 12 
// created by:		D. Kalantaryan 
// 

#include <eq_fct.h>

const char* object_name = "laserbeamline_server";

void eq_init_prolog() {}
void eq_cancel(){}
void refresh_prolog() {}
void refresh_epilog() {}
void post_init_epilog(void) {}
void interrupt_usr1_epilog(int) {}
void interrupt_usr1_prolog(int) {}
void eq_init_epilog() {}
void post_init_prolog() {}

EqFct* eq_create(int a_eq_code, void*)
{
	return NULL;
}
