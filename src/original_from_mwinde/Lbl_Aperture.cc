#include "Lbl_Aperture.h"

int	EqFctLbl_Aperture::aperConf_done = 0;
const int  EqFctLbl_Aperture::maxApers = NUMBER_OF_SOMETHING;

EqFctLbl_Aperture::EqFctLbl_Aperture() :
    EqFctLbl_XYZ_Device()
    ,X_Aper_0("X.APER_0 nominal", this)    
    ,X_Aper_1("X.APER_1 nominal", this)
    ,X_Aper_2("X.APER_2 nominal", this)
    ,X_Aper_3("X.APER_3 nominal", this)
    ,X_Aper_4("X.APER_4 nominal", this)
    ,X_Aper_5("X.APER_5 nominal", this)
    ,X_Aper_6("X.APER_6 nominal", this)
    ,X_Aper_7("X.APER_7 nominal", this)
    ,X_Aper_8("X.APER_8 nominal", this)
    ,X_Aper_9("X.APER_9 nominal", this)
    ,X_Aper_10("X.APER_10 nominal", this)
    ,X_Aper_11("X.APER_11 nominal", this)
    ,X_Aper_12("X.APER_12 nominal", this)
    ,X_Aper_13("X.APER_13 nominal", this)
    ,X_Aper_14("X.APER_14 nominal", this)
    
    ,Y_Aper_0("Y.APER_0 nominal", this)    
    ,Y_Aper_1("Y.APER_1 nominal", this)
    ,Y_Aper_2("Y.APER_2 nominal", this)
    ,Y_Aper_3("Y.APER_3 nominal", this)
    ,Y_Aper_4("Y.APER_4 nominal", this)
    ,Y_Aper_5("Y.APER_5 nominal", this)
    ,Y_Aper_6("Y.APER_6 nominal", this)
    ,Y_Aper_7("Y.APER_7 nominal", this)
    ,Y_Aper_8("Y.APER_8 nominal", this)
    ,Y_Aper_9("Y.APER_9 nominal", this)
    ,Y_Aper_10("Y.APER_10 nominal", this)
    ,Y_Aper_11("Y.APER_11 nominal", this)
    ,Y_Aper_12("Y.APER_12 nominal", this)
    ,Y_Aper_13("Y.APER_13 nominal", this)
    ,Y_Aper_14("Y.APER_14 nominal", this)

    ,Z_Aper_0("Z.APER_0 nominal", this)    
    ,Z_Aper_1("Z.APER_1 nominal", this)
    ,Z_Aper_2("Z.APER_2 nominal", this)
    ,Z_Aper_3("Z.APER_3 nominal", this)
    ,Z_Aper_4("Z.APER_4 nominal", this)
    ,Z_Aper_5("Z.APER_5 nominal", this)
    ,Z_Aper_6("Z.APER_6 nominal", this)
    ,Z_Aper_7("Z.APER_7 nominal", this)
    ,Z_Aper_8("Z.APER_8 nominal", this)
    ,Z_Aper_9("Z.APER_9 nominal", this)
    ,Z_Aper_10("Z.APER_10 nominal", this)
    ,Z_Aper_11("Z.APER_11 nominal", this)
    ,Z_Aper_12("Z.APER_12 nominal", this)
    ,Z_Aper_13("Z.APER_13 nominal", this)
    ,Z_Aper_14("Z.APER_14 nominal", this)

    ,diamAper_0("DIAM.APER_0 diameter", &diamAper_str[0][0], STRING_LENGTH, this)    
    ,diamAper_1("DIAM.APER_1 diameter", &diamAper_str[1][0], STRING_LENGTH, this)
    ,diamAper_2("DIAM.APER_2 diameter", &diamAper_str[2][0], STRING_LENGTH, this)
    ,diamAper_3("DIAM.APER_3 diameter", &diamAper_str[3][0], STRING_LENGTH, this)
    ,diamAper_4("DIAM.APER_4 diameter", &diamAper_str[4][0], STRING_LENGTH, this)
    ,diamAper_5("DIAM.APER_5 diameter", &diamAper_str[5][0], STRING_LENGTH, this)
    ,diamAper_6("DIAM.APER_6 diameter", &diamAper_str[6][0], STRING_LENGTH, this)
    ,diamAper_7("DIAM.APER_7 diameter", &diamAper_str[7][0], STRING_LENGTH, this)
    ,diamAper_8("DIAM.APER_8 diameter", &diamAper_str[8][0], STRING_LENGTH, this)
    ,diamAper_9("DIAM.APER_9 diameter", &diamAper_str[9][0], STRING_LENGTH, this)
    ,diamAper_10("DIAM.APER_10 diameter", &diamAper_str[10][0], STRING_LENGTH, this)
    ,diamAper_11("DIAM.APER_11 diameter", &diamAper_str[11][0], STRING_LENGTH, this)
    ,diamAper_12("DIAM.APER_12 diameter", &diamAper_str[12][0], STRING_LENGTH, this)
    ,diamAper_13("DIAM.APER_13 diameter", &diamAper_str[13][0], STRING_LENGTH, this)
    ,diamAper_14("DIAM.APER_14 diameter", &diamAper_str[14][0], STRING_LENGTH, this)

    ,diamAper_0_f("DIAM.APER_0_F diameter float", this)    
    ,diamAper_1_f("DIAM.APER_1_F diameter float", this)
    ,diamAper_2_f("DIAM.APER_2_F diameter float", this)
    ,diamAper_3_f("DIAM.APER_3_F diameter float", this)
    ,diamAper_4_f("DIAM.APER_4_F diameter float", this)
    ,diamAper_5_f("DIAM.APER_5_F diameter float", this)
    ,diamAper_6_f("DIAM.APER_6_F diameter float", this)
    ,diamAper_7_f("DIAM.APER_7_F diameter float", this)
    ,diamAper_8_f("DIAM.APER_8_F diameter float", this)
    ,diamAper_9_f("DIAM.APER_9_F diameter float", this)
    ,diamAper_10_f("DIAM.APER_10_F diameter float", this)
    ,diamAper_11_f("DIAM.APER_11_F diameter float", this)
    ,diamAper_12_f("DIAM.APER_12_F diameter float", this)
    ,diamAper_13_f("DIAM.APER_13_F diameter float", this)
    ,diamAper_14_f("DIAM.APER_14_F diameter float", this)

    ,diamAper("DIAM.APER.ACT diameter of selected aperture", diamAperNext_str, STRING_LENGTH, this)
    ,diamAper_f("DIAM.APER.ACT_F float diameter of selected aperture", this)
    ,actAper("NB.APER.ACT nb. of selected aperture", this)
    ,diamNextAper("DIAM.APER.NEXT diameter of previewed aperture", diamAperThis_str, STRING_LENGTH, this)
    ,nextAper("NB.APER.NEXT nb. of previewed aperture", this)
    ,aperSetNb("APER_SER.NB aperature set used actually", this)
    ,button_changeAper("DOCHANGEAPER move to previewed aperture", this)
{
    if (!aperConf_done) {
	list_append();
	aperConf_done = 1;
    }
    // store pointers of properties to 4 arrays
    X_actAperP[0] = &X_Aper_0; Y_actAperP[0] = &Y_Aper_0; 
      Z_actAperP[0] = &Z_Aper_0;
      aperDiam_p[0] = &diamAper_0_f;
    X_actAperP[1] = &X_Aper_1; Y_actAperP[1] = &Y_Aper_1; 
      Z_actAperP[1] = &Z_Aper_1;
      aperDiam_p[1] = &diamAper_1_f;
    X_actAperP[2] = &X_Aper_2; Y_actAperP[2] = &Y_Aper_2; 
      Z_actAperP[2] = &Z_Aper_2;
      aperDiam_p[2] = &diamAper_2_f;
    X_actAperP[3] = &X_Aper_3; Y_actAperP[3] = &Y_Aper_3; 
      Z_actAperP[3] = &Z_Aper_3;
      aperDiam_p[3] = &diamAper_3_f;
    X_actAperP[4] = &X_Aper_4; Y_actAperP[4] = &Y_Aper_4; 
      Z_actAperP[4] = &Z_Aper_4;
      aperDiam_p[4] = &diamAper_4_f;
    X_actAperP[5] = &X_Aper_5; Y_actAperP[5] = &Y_Aper_5; 
      Z_actAperP[5] = &Z_Aper_5;
      aperDiam_p[5] = &diamAper_5_f;
    X_actAperP[6] = &X_Aper_6; Y_actAperP[6] = &Y_Aper_6; 
      Z_actAperP[6] = &Z_Aper_6;
      aperDiam_p[6] = &diamAper_6_f;
    X_actAperP[7] = &X_Aper_7; Y_actAperP[7] = &Y_Aper_7; 
      Z_actAperP[7] = &Z_Aper_7;
      aperDiam_p[7] = &diamAper_7_f;
    X_actAperP[8] = &X_Aper_8; Y_actAperP[8] = &Y_Aper_8; 
      Z_actAperP[8] = &Z_Aper_8;
      aperDiam_p[8] = &diamAper_8_f;
    X_actAperP[9] = &X_Aper_9; Y_actAperP[9] = &Y_Aper_9; 
      Z_actAperP[9] = &Z_Aper_9;
      aperDiam_p[9] = &diamAper_9_f;
    X_actAperP[10] = &X_Aper_10; Y_actAperP[10] = &Y_Aper_10; 
      Z_actAperP[10] = &Z_Aper_10;
      aperDiam_p[10] = &diamAper_10_f;
    X_actAperP[11] = &X_Aper_11; Y_actAperP[11] = &Y_Aper_11; 
      Z_actAperP[11] = &Z_Aper_11;
      aperDiam_p[11] = &diamAper_11_f;
    X_actAperP[12] = &X_Aper_12; Y_actAperP[12] = &Y_Aper_12; 
      Z_actAperP[12] = &Z_Aper_12;
      aperDiam_p[12] = &diamAper_12_f;
    X_actAperP[13] = &X_Aper_13; Y_actAperP[13] = &Y_Aper_13; 
      Z_actAperP[13] = &Z_Aper_13;
      aperDiam_p[13] = &diamAper_13_f;
    X_actAperP[14] = &X_Aper_14; Y_actAperP[14] = &Y_Aper_14; 
      Z_actAperP[14] = &Z_Aper_14;
      aperDiam_p[14] = &diamAper_14_f;
}

void EqFctLbl_Aperture::update() {
    EqFctLbl_XYZ_Device::update();
}

void EqFctLbl_Aperture::init() {

    EqFctLbl_XYZ_Device::init();
}

void EqFctLbl_Aperture::SaveActAper() {
    int idx = actAper.value() -1;
    X_actAperP[idx]->set_value(X_Nominal.value());
    Y_actAperP[idx]->set_value(Y_Nominal.value());
    Z_actAperP[idx]->set_value(Z_Nominal.value());
}

smStatus EqFctLbl_Aperture::SetAsNominal(int val) {
    smStatus stat = EqFctLbl_XYZ_Device::SetAsNominal(val);
    SaveActAper();
    return stat;
}
	
smStatus EqFctLbl_Aperture::PerformSeletedCmd(int val) {
    smStatus stat = 0;
    int baseCmd = val & cmdMask_baseCmds;
    switch(baseCmd) {
	case cmd_changeAper:
	    // go to the selected aperture:
	    if(nextAper.value() != actAper.value()) {
		// - save previous parameters
		SaveActAper();
		// - set new parameters
		int idx = nextAper.value();
		actAper.set_value(idx);
		idx--;
		X_Nominal.set_value((X_actAperP[idx])->value());
		Y_Nominal.set_value((Y_actAperP[idx])->value());
		Z_Nominal.set_value((Z_actAperP[idx])->value());
		diamAper.copy_value(&diamAper_str[idx][0]);
		diamAper_f.set_value(aperDiam_p[idx]->value());
		X_drive->UpdateLimits();
		Y_drive->UpdateLimits();
		Z_drive->UpdateLimits();
		// - perform the move
		pendingCommand = cmdS_X | cmdS_Y| cmdS_Z | cmd_moveToNominal;
		stat = EqFctLbl_XYZ_Device::PerformSeletedCmd(cmdS_X | cmdS_Y| cmdS_Z | cmd_moveToNominal);
	    }
	    break;
	default: // not a command special to EqFctLbl_Aperture
	    stat = EqFctLbl_XYZ_Device::PerformSeletedCmd(val);
    }
    return stat;
}

D_int_next::D_int_next(const char* pn, EqFctLbl_Aperture* theEqFct)
    : D_int(pn, (EqFct*)theEqFct)
    , itsEqFct(theEqFct){
}

void D_int_next::set_value(int val) {
    if((val > 0) && (val <= itsEqFct->maxApers)) {
	D_int::set_value(val);
	(itsEqFct->diamNextAper).copy_value(&((itsEqFct->diamAper_str)[val-1][0]));
    }
}

D_int_cmd_(D_int_cmd_changeAper);
