// file eq_adcbpmxy.h
//
// FastADC_BPM Eq function class
//
//

#ifndef eq_adcict_h
#define eq_adcict_h

#include	"eq_fct.h"
//#include	"ip24digio.h"
#include	"linux_timer_io.h"

#include	"ScopeAdc.h"
//#include	"adcdaq.h"
#include 	"eq_client.h"

#include        "D_spec_calc.h"
#include <vector>

int current;

const int CodeSpectDiff = 889;
const int CodeSpectDiffIBPC = 888;

// Calculation constants
const float LPN = 600.0; // default lpn, used when no networking

const float BEAMENERGY = 17.0; // MeV, beam energy
const float CHARGE_THRESHOLD = 0.1; // nC, charge thresholf
const float MAX_POSSIBLE_CHARGE = 6.0; // nC, max possible charge
const int ACCISON = 0; // default value, used when no networking

//const int LOWSCR1OUT = 0; // screen is out
//const int LOWSCR2OUT = 0; // screen is out
//const int LOWSCR3OUT = -54; // screen is out

const float GUNCOEFF = 7.09; const float GUNMAXPOWER = 6.5;
const float BOOSTERCOEFF = 66.67; const float BOOSTERMAXPOWER = 5.5;

class EqFctADC_ICT;

class D_recordingInt : public D_int {
public:
    D_recordingInt(const char* pn, EqFct*eq) : D_int(pn, eq) {}
    void set_value(float val) {
        printftostderr(get_eqfct()->name_str(), "%s: value changed from %d to %d", value_, val);
        D_int::set_value(val);
    }
};

class D_rmtFloat : public D_float
{
private:
	D_string* remote;
        int indir_get(const char*, float&);
        EqFct* fct;

public:
	D_rmtFloat(const char *pn, D_string* adr, EqFct* ptr) :
            D_float(pn, ptr), remote(adr), fct(ptr) {}
        const char* getAddress() {
            return remote->value();
        }

        float   value      ();
	void	get       (EqAdr *, EqData *, EqData *, EqFct *);
	void	set       (EqAdr *, EqData *, EqData *, EqFct *);
};

// ========================= CLASS EqFctSpectDiff =====================
class EqFctSpectDiff : public EqFct {

protected:
       // float lpn, reprate;
       // int readFlag, chargeFlag, accIsOn;
        D_name          alias_;
        D_int		is_interrupt_;
	D_adcscope      *sig_;
	D_adcscope      *bckg_;
	D_Spectrum	*diff_;
        D_spec_calc     *calc_;
        D_polynom       *poly_;
        D_float         *res_;
        D_hist          *res_hist_;
        D_float         *mean_;
        D_hist          *mean_hist_;
        D_float         *rms_;
        D_hist          *rms_hist_;
        D_int           *onechN_;
        D_float         *onech_;
        D_hist          *onech_hist_;
        // for pulse edges
        D_float         *thresh_;
        D_int           *startCh_;
        D_int           *endCh_;
	ArchiveFile	*archloc_p;
        
	u_short control; // var for D_bit
        //struct timeval TM;
        //double oldT, newT; // To catch time interval 1sec
        std::vector<int> errorCases;
        
public:
	EqFctSpectDiff();
	void		interrupt_usr1 	(int signo);	// called on interrupt
	void		update 		();					// called on timer
	void		init 		();					// started after creation of all Eq's
	int		fct_code        ()	{ return CodeSpectDiff; }
	int             read_adc        ();
        void set_error_ones(int err_no, const char* err_str, int err_case);
        void unset_error_ones(const char* err_str, int err_case);
        static int		conf_done;
};
// ====================================================================


// ========================= CLASS EqFctSpectDiffIBPC =====================
class EqFctSpectDiffIBPC : public EqFctSpectDiff {

protected:
        // Remote data addresses
        TDS             diff_TDS;
	D_string        lpnAddr_;
        D_rmtFloat      lpn_;
       	D_string        accIsOnAddr_;
        D_rmtFloat      accIsOn_;
       	D_string        LowScr1Addr_;
        D_rmtFloat      LowScr1_;
       	D_string        LowScr2Addr_;
        D_rmtFloat      LowScr2_;
       	D_string        LowScr3Addr_;
        D_rmtFloat      LowScr3_;
        D_string        gunPowerAddr_;
        D_rmtFloat      gunPower_;
        D_string        boosterPowerAddr_;
        D_rmtFloat      boosterPower_;
        // IBPC specific data
        float lpn;
        float           maxInTrain, maxTrainCharge, maxChargeSec, maxChargeSecValue;
        double          powerIntegral, powerIntegral24, powerAddition;
        D_double         maxChargeSec_;
        D_hist          maxChargeSec_hist_;
        D_double         beamMomentum_;
        D_hist          beamMomentum_hist_;
        D_double         beamPower_;
        D_hist          beamPower_hist_;
        D_double         powerIntegral_;
        D_hist          powerIntegral_hist_;
        D_double         powerIntegral24_;
        D_hist          powerIntegral24_hist_;
        D_double         powerIntegral24old_;
        D_int           screenMask_;
                
        D_recordingInt LOWSCR1OUT;
        D_recordingInt LOWSCR2OUT;
        D_recordingInt LOWSCR3OUT;
        D_double         powerAddition_;
        D_hist          powerAddition_hist_;
        struct timeval TM;
        double oldT, newT; // To catch time interval 1sec

        ///////////////////////////////////////////

        static int	conf_done;

public:
	EqFctSpectDiffIBPC();
	void		interrupt_usr1 	(int signo);	// called on interrupt
	void		update 		();					// called on timer
	void		init 		();					// started after creation of all Eq's
	int		fct_code        ()	{ return CodeSpectDiffIBPC; }
	int             read_adc        ();
        void            fillHistory(); // to called in update
 };
// ====================================================================


#endif
