#ifndef lbl_WedgePlates_h
#define lbl_WedgePlates_h

#include "Lbl_XYDevice.h"

class EqFctLbl_WedgePlates : public EqFctLbl_XYDevice {

public:
    EqFctLbl_WedgePlates();		// constructor
    ~EqFctLbl_WedgePlates() { ; };	// destructor

    virtual int	fct_code()	{ return CodeLbl_WedgePlates; };

    static int	wpConf_done;
    
    virtual void CreateDrives();
    virtual void initHardware();
    
private:
protected:

};

class RotatingDrive : public Drive {

public:
    RotatingDrive(EqFctLbl_WedgePlates* theMirror, MICOS_Controller* theMICOS,
    	int theMotorNumber, int diodesBit, char* the_text_str,
        D_int* theNominal, D_int* theStepsActAbs, D_int* theLastValid, D_int* theStepsActNominal, 
	D_float* the_mmActAbs, D_float* the_mmActNominal,
	D_float* the_mmLowLimit, D_float* the_mmUpLimit,
    	D_int_quad* the_stepsTo, D_int_quad* the_stepsBy, D_float_quad* the_mmTo, D_float_quad* the_mmBy,
	D_int* theDestinationOK, D_int* theStepsMax, D_int* theStepsMaxMax, D_int* thestepsNormPosition, 
	float the_axisFactor, bool the_diode_dark_at0);
    
    virtual void AdjustQuad(D_int_quad* an_int_quad) { Drive::AdjustQuad(an_int_quad);}
    virtual void AdjustQuad(D_float_quad* a_float_quad);

    virtual float Calc_mm(int steps);
    virtual int Calc_Steps(float mm);
       
    smStatus MoveTo(int  moveTo_rel_steps, bool fixedStepsBy = true);
    smStatus FindNormPosition(int& pending, int this_now);
    virtual void CheckIfTargetCanBeReached();

    virtual void UpdateLimits() {}; // always [0..360]

private:    
    int stepsPerRevolution;
};
#endif
