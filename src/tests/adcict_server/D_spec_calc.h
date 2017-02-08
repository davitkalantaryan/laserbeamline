/* 
 * File:   D_spec_calc.h
 * Author: bagrat
 *
 * Created on January 15, 2009, 5:12 PM
 */

#ifndef _D_SPEC_CALC_H
#define	_D_SPEC_CALC_H

#include	"eq_fct.h"
#include	"d_fct.h"

class D_spec_calc : public D_iiii {
// configuration is programmed by IIII data type :
// .i1_data :	calculator type
//			0 : non
//			1 : mean
//			2 : min
//			3 : max
//			4 : RMS
//			5 : line fit: gradient
//			6 : line fit: offset
// .i2_data :	start sample
// .i3_data :	number of samples
// .i4_data :	increment

    D_spectrum*   spec_;
    D_int*        startCh_;
    D_int*        endCh_;
    D_float*      mean_;
    D_float*      rms_;
    D_float*      err_;
    D_float*      onech_;

public:
	D_spec_calc ( const char* pn, D_spectrum* sp, D_int* startch, D_int* endCh, D_float* mean, D_float* rms, D_float* onech, EqFct* eq);
	float	evaluate ();		// do the calculation
};
//:DESCRIPTION	D_spec_calc
//.The D_adc_calc class implements calculations on ADC data.
//.The type and parameters are stored in an IIII structure.
//.Special functions of the D_adc_calc class are:
//!void	set_value ( IIII* );
//.	Set the filter parameter, receives a pointer to a IIII structure. The
//.	first two parameters are used only (i1_data and f1_data).
//.		1. parameter :	calculator type
//.			0 : non
//.			1 : mean
//.			2 : min
//.			3 : max
//.			4 : RMS
//.			5 : line fit: gradient
//.			6 : line fit: offset
//.	 	2. parameter :	start sample
//.	 	3. parameter :	number of samples
//.	 	4. parameter :	increment
//!float	evaluate (char* adr)
//.	Evaluates a new value by applying the calculation to the ADC data
//.	that start on "adr" and returns the float result.
//.


// ========================= CLASS EqFctSpectDiff =====================
class D_Spectrum : public D_spectrum
{
public:
    D_Spectrum(const char *pn, u_int maxl, EqFct *ef, bool store = false);
    void get (EqAdr *, EqData *, EqData *, EqFct *);
};

#endif	/* _D_SPEC_CALC_H */

