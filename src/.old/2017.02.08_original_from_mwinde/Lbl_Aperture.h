#ifndef lbl_Aperture_h
#define lbl_Aperture_h

#define NUMBER_OF_SOMETHING 15

#include "Lbl_XYZ_Device.h"

static const int cmd_changeAper = 8;	// additional base command


class EqFctLbl_Aperture;

SimpleButton(EqFctLbl_Aperture, D_int_cmd_changeAper, cmdS_X | cmdS_Y | cmd_changeAper);

class D_int_next : public D_int {
public:
        D_int_next(const char* pn, EqFctLbl_Aperture* theEqFct);
	void set_value(int val);
private:
	EqFctLbl_Aperture* itsEqFct;
};

class EqFctLbl_Aperture : public EqFctLbl_XYZ_Device {

friend class D_int_next;

public:
    EqFctLbl_Aperture();		// constructor
    ~EqFctLbl_Aperture() { ; }	// destructor

    virtual void	update ();
    virtual void	init ();	// started after creation of all Eq's

    virtual int	fct_code()	{ return CodeLbl_Aperture; }

    static int	aperConf_done;
    
    virtual smStatus PerformSeletedCmd(int val);
    //virtual smStatus TryReady();
    
    void SaveActAper();
    virtual smStatus SetAsNominal(int val);   	

private:
    // nominal values of apertures
    D_int X_Aper_0 ,X_Aper_1 ,X_Aper_2 ,X_Aper_3 ,X_Aper_4;
    D_int X_Aper_5 ,X_Aper_6 ,X_Aper_7 ,X_Aper_8 ,X_Aper_9;
    D_int X_Aper_10 ,X_Aper_11 ,X_Aper_12 ,X_Aper_13 ,X_Aper_14;

    D_int Y_Aper_0 ,Y_Aper_1 ,Y_Aper_2 ,Y_Aper_3 ,Y_Aper_4;
    D_int Y_Aper_5 ,Y_Aper_6 ,Y_Aper_7 ,Y_Aper_8 ,Y_Aper_9;
    D_int Y_Aper_10 ,Y_Aper_11 ,Y_Aper_12 ,Y_Aper_13 ,Y_Aper_14;
    
    D_int Z_Aper_0 ,Z_Aper_1 ,Z_Aper_2 ,Z_Aper_3 ,Z_Aper_4;
    D_int Z_Aper_5 ,Z_Aper_6 ,Z_Aper_7 ,Z_Aper_8 ,Z_Aper_9;
    D_int Z_Aper_10 ,Z_Aper_11 ,Z_Aper_12 ,Z_Aper_13 ,Z_Aper_14;
    
    // diameters of apertures (or form describing text)
    D_string diamAper_0 ,diamAper_1 ,diamAper_2 ,diamAper_3 ,diamAper_4;
    D_string diamAper_5 ,diamAper_6 ,diamAper_7 ,diamAper_8 ,diamAper_9;
    D_string diamAper_10 ,diamAper_11 ,diamAper_12 ,diamAper_13 ,diamAper_14;
    
    // diameters of apertures (0: no hole, <0: grid or otherwise special form)
    D_float diamAper_0_f, diamAper_1_f, diamAper_2_f, diamAper_3_f, diamAper_4_f;
    D_float diamAper_5_f, diamAper_6_f, diamAper_7_f, diamAper_8_f, diamAper_9_f;
    D_float diamAper_10_f, diamAper_11_f, diamAper_12_f, diamAper_13_f, diamAper_14_f;
    
    char diamAperThis_str[STRING_LENGTH];
    D_string diamAper;	// diameter of actually selected aperture [mm] (or form descibing text)
    D_float diamAper_f;	// diameter of actually selected aperture [mm] (float, 0: no hole, -1: grid)
    D_int actAper;	// nb. of actually selected aperture [1..15]
    
    char diamAperNext_str[STRING_LENGTH];
    D_string diamNextAper;  // diameter of preselected aperture [mm]  (or form descibing text)
    D_int_next nextAper;   // nb. of preselected (foreseen to be moved to) aperture
    
    static const int maxApers;
    D_int* X_actAperP[NUMBER_OF_SOMETHING];
    D_int* Y_actAperP[NUMBER_OF_SOMETHING];
    D_int* Z_actAperP[NUMBER_OF_SOMETHING];
    char diamAper_str[NUMBER_OF_SOMETHING][STRING_LENGTH];
    D_float* aperDiam_p[NUMBER_OF_SOMETHING];
    D_int aperSetNb;


protected:
    D_int_cmd_changeAper   button_changeAper;

};

#endif
