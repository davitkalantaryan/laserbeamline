// file adcict_rpc_server.cc
//
// ADC Eq function class
//
// Bagrat Petrosyan,
//

#include	<sys/time.h>
#include	<signal.h>
#include	<setjmp.h>
#include	"eq_adcict.h"
#include        "eq_client.h"
#include	"eq_errors.h"
#include	"eq_fct_errors.h"
#include	"eq_sts_codes.h"
//#include	"VME.h"
///#include	"iobus.h"
///#include	"ip24digio.h"
//#include	"newtimer_io.h"
///#include 	"DOOCSregexp.h"
#include	"printtostderr.h"
#include	<math.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<sys/ioctl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include        <ctime>
#include <algorithm>

#include	"ADCDma.h"
#include "D_spec_calc.h"

#define     ADC_STORE		"pitz_ict.hist"	// file name:
#define     ADC_SPECT		"pitz_ict.spect"
#define     ADC_DEV		"pitz.ict.adc"		// group names :
#define     IPTIMER_IRQ_REG     0xFFFF
#define     IPTIMER_IRQ_REG_CH3 0x2F

extern int is_interrupt;
extern int chck_interrupt;
extern int errno;
int fd_timer;

extern ADC_MAP_CLT adc_map_;

enum {
    no_error_case,
    device_error_case,
    ill_parameter_case,
    map_error_case,
    bord_error_case,
    signal_error_case,
    spectrum_error_case,
    noise_error_case,
    bkSpectrum_error_case,
    interrupt_error_case,
    bstz_error_case,
    no_beam_case,
    lpn_error_case,
    screen1_error_case,
    screen2_error_case,
    screen3_error_case,
    no_booster_case,
    no_pulse_case,
    pulse_detection_case,
    no_interrupt_case
};


const char* object_name = "FastADC_ICT"; // name of this object (used in error messages)
EqFct* adc_fct = 0;
ArchiveSPEC* ar_spect_p = 0;
int EqFctSpectDiff::conf_done = 0;
int EqFctSpectDiffIBPC::conf_done = 0;
int EqFctADC_ready = 0;

extern int fct_code;
extern int ring_buffer;

extern std::vector<EqFct*> *eq_list;
extern Config* config;

int in_interrupt = 0;
int timer_irg_mask = 0;
time_t walltime;


// Global functions

bool time24hour() {
    static bool done = false;
    tm* hima = localtime(&walltime);
    if (hima->tm_hour == 0 && hima->tm_min == 0 && hima->tm_sec < 30) {
        if (done == false) {
            done = true;
            return true;
        } else return false;
    } else {
        done = false;
        return false;
    }
}

void eq_init_prolog() // called before init of all EqFct's
{

    if (!ar_spect_p) ar_spect_p = new ArchiveSPEC(const_cast<char*> (ADC_SPECT), const_cast<char*> (ADC_DEV));

    EqFctADC_ready = 0;
}

EqFct *eq_create(int code, void *) {
    EqFct *eqn;
    switch (code) {
        case 778:
            eqn = new D_ADCShm();
            break;
        case CodeSpectDiffIBPC:
            eqn = new EqFctSpectDiffIBPC();
            break;
        case CodeSpectDiff:
            eqn = new EqFctSpectDiff();
            break;
        default:
            printf("bad eq_fct number\n");
            eqn = (EqFct *) 0;
            break;
    }
    return eqn;
}

void eq_init_epilog() {
} // called at end of init of all EqFct's

void post_init_prolog() {
}

void post_init_epilog() {
    adcscope_post_init_epilog();
    adcscope_setup_interrupt();
}

void refresh_prolog() // called before "update"
{
    int cur_map_err = 0;
    int cur_brd_err = 0;
    int cur_bnch;
    struct timeval cur_tm;
    int tmp_event = 0;
    int cur_genevent; //new timing

    walltime = ::time(0);
    adcscope_refresh_prolog();
    if (is_interrupt || chck_interrupt > MAX_INT_MISSED) {

        adc_map_.get_cur_buf(&cur_bnch);
        cur_genevent = adc_map_.read_gen_buf_event_rw(cur_bnch); //new timing
        cur_map_err = adc_map_.get_map_err();
        cur_brd_err = adc_map_.get_brd_err();

        if (!cur_map_err) {
            cur_tm = adc_map_.read_time_stp_rw(cur_bnch);
            tmp_event = adc_map_.read_buf_evnt_rw(cur_bnch);
            tmp_event &= 0xFF;
        } else {
            gettimeofday(&cur_tm, 0);
        }

    }
}

void interrupt_usr1_prolog(int sig_no) {
    int cur_map_err = 0;
    int cur_brd_err = 0;
    int cur_bnch;
    struct timeval cur_tm;
    int dmaer = 0;
    int cur_genevent; //new timing

    switch (sig_no) {
        case 1: // SIGUSR1: ADC trigger interrupt
            adcscope_interrupt_usr1_prolog(sig_no);
            timer_irg_mask = 1;
            cur_map_err = adc_map_.get_map_err();
            cur_brd_err = adc_map_.get_brd_err();
            adc_map_.get_cur_buf(&cur_bnch);
            cur_genevent = adc_map_.read_gen_buf_event_rw(cur_bnch); //new timing
            if (!cur_map_err) {
                cur_tm = adc_map_.read_time_stp_rw(cur_bnch);
            } else {
                gettimeofday(&cur_tm, 0);
                dmaer = cur_map_err;
            }
            break;
        case 2: // SIGUSR2: reset ADCs interrupt
            break;
        default:
            break;
    }
}

void refresh_epilog() {
    if (is_interrupt || chck_interrupt > MAX_INT_MISSED) {
        adcscope_refresh_epilog();
    }
}

void interrupt_usr1_epilog(int sig_no) {
    switch (sig_no) {
        case 1: // SIGUSR1: ADC trigger interrupt
            adcscope_interrupt_usr1_epilog(sig_no);
            break;
        case 2: // SIGUSR2: reset ADCs interrupt
            adcscope_interrupt_usr1_epilog(sig_no);
            break;
        default:
            break;
    }
}

void interrupt_usr2_prolog() {
}

void interrupt_usr2_epilog() {
}

void eq_cancel() {
}

// Class D_rmtFloat

float D_rmtFloat::value() {
    float val;
    if (indir_get(remote->value(), val) == 0) value_ = val;
    else value_ = -1;
    return value_;
}

void D_rmtFloat::get(EqAdr *, EqData *, EqData *res, EqFct *) {
    res->set(value());
}

void D_rmtFloat::set(EqAdr *, EqData *, EqData *, EqFct *) {
    return;
}

int D_rmtFloat::indir_get(const char* addr, float& ret) {
    EqData *data_ptr_, main_data_;
    EqAdr doocs_addr_;
    EqCall call_ptr_;

    float val;

    doocs_addr_.adr(addr);
    data_ptr_ = call_ptr_.get(&doocs_addr_, &main_data_);
    val = data_ptr_->get_float();
    if (data_ptr_->error() != 0) return data_ptr_->error();
    ret = val;
    return 0;
}


// ========================= CLASS EqFctSpectDiff =====================

EqFctSpectDiff::EqFctSpectDiff() :
EqFct("NAME = location")
, alias_("ALIAS device name", this)
, is_interrupt_("INTERRUPT checking for interrupts", this) {
    sig_ = new D_adcscope("SIG", this);
    bckg_ = new D_adcscope("BCKG", this);
    diff_ = new D_Spectrum("DIFF.TD", (int) SPECTRUM_LENGTH, (EqFct*)this);
    res_ = new D_float("CALC.RES", this);
    poly_ = new D_polynom("DIFF.POLYPARA", this);
    res_hist_ = new D_hist("CALC.RES.HIST", this);
    mean_ = new D_float("CALC.MEAN", this);
    mean_hist_ = new D_hist("CALC.MEAN.HIST", this);
    rms_ = new D_float("CALC.RMS", this);
    rms_hist_ = new D_hist("CALC.RMS.HIST", this);
    onech_ = new D_float("ONE_CH single channel", this);
    onech_hist_ = new D_hist("ONE_CH.HIST single channel", this);

    thresh_ = new D_float("THRESHOLD to find the start point", this);
    startCh_ = new D_int("START_CH start channel of the pulse", this);
    endCh_ = new D_int("END_CH end channel of the pulse", this);

    calc_ = new D_spec_calc("DIFF.CALC.ALGO", diff_, startCh_, endCh_, mean_, rms_, onech_, this);

    g_sts_.online(0);

    list_append();
    conf_done = 1;
}
//
//void EqFctSpectDiff::set_error_ones(int err_no, const char* err_str, int err_case) {
//    std::vector<int>::iterator it = std::find(errorCases.begin(), errorCases.end(), err_case);
//    if (it == errorCases.end()) {
//        errorCases.push_back(err_case);
//        set_error(err_no, err_str);
//        printtostderr(name_str(), err_str);
//    }
//}
//
//void EqFctSpectDiff::unset_error_ones(const char* err_str, int err_case) {
//    std::vector<int>::iterator it = std::find(errorCases.begin(), errorCases.end(), err_case);
//    if (it != errorCases.end()) {
//        errorCases.erase(it);
//        if(err_str) {
//            printtostderr(name_str(), err_str);
//        }
//    }
//}

void EqFctSpectDiff::init() {
    char HistFile[80];

    //
    // create hist file per location
    //
    sprintf(HistFile, "hist/ICT_%s.hist", name_.value());
    archloc_p = new ArchiveFile(HistFile, ADC_DEV, NOINDEX);

    //////sig_->init(archloc_p, ar_spect_p, ADC_SPECT, ADC_DEV);
    //////bckg_->init(archloc_p, ar_spect_p, ADC_SPECT, ADC_DEV);
    sig_->init(archloc_p);
    bckg_->init(archloc_p);

    res_hist_->book_hist(archloc_p, name_str(), ring_buffer);
    mean_hist_->book_hist(archloc_p, name_str(), ring_buffer);
    rms_hist_->book_hist(archloc_p, name_str(), ring_buffer);
    onech_hist_->book_hist(archloc_p, name_str(), ring_buffer);
}

void EqFctSpectDiff::update() {
    TDS diff_TDS;
    diff_TDS.tm = walltime;

    // Case of bad threshold
    if (!g_sts_.online()) {
        diff_TDS.status = sts_offline;

        diff_TDS.data = mean_->value();
        mean_hist_->fill_hist(&diff_TDS);

        diff_TDS.data = rms_->value();
        rms_hist_->fill_hist(&diff_TDS);

        diff_TDS.data = res_->value();
        res_hist_->fill_hist(&diff_TDS);

        diff_TDS.data = onech_->value();
        onech_hist_->fill_hist(&diff_TDS);

        return;
    }

    if (is_interrupt || chck_interrupt > MAX_INT_MISSED) {
        int err = read_adc();
        if (!err) {
            set_error(no_interrupt_case, "NO interrupt anymore");
        }
    }

    is_interrupt_.set_value(chck_interrupt);

    res_->set_value(calc_->evaluate());
    diff_TDS.status = sts_ok;
    diff_TDS.data = res_->value();
    res_hist_->fill_hist(&diff_TDS);
    diff_TDS.data = onech_->value();
    onech_hist_->fill_hist(&diff_TDS);

    // Case of bad threshold
    if (startCh_->value() >= endCh_->value()) {
        set_error(ill_parameter_case, "bad threshold");

        diff_TDS.status = sts_err;

        diff_TDS.data = mean_->value();
        mean_hist_->fill_hist(&diff_TDS);

        diff_TDS.data = rms_->value();
        rms_hist_->fill_hist(&diff_TDS);

        return;

    } else {
        set_error(no_error_case, "no error");
    }

    // Filling history together with error bar
    // Error bar color is ok2
    diff_TDS.data = mean_->value();
    mean_hist_->fill_hist(&diff_TDS);

    diff_TDS.status = sts_ok2;
    diff_TDS.data = mean_->value() - rms_->value();
    mean_hist_->fill_hist(&diff_TDS);
    diff_TDS.data = mean_->value() + rms_->value();
    mean_hist_->fill_hist(&diff_TDS);

    diff_TDS.status = sts_ok;
    diff_TDS.data = mean_->value();
    mean_hist_->fill_hist(&diff_TDS);

    diff_TDS.data = rms_->value();
    rms_hist_->fill_hist(&diff_TDS);
}

void EqFctSpectDiff::interrupt_usr1(int sig_no) {
    switch (sig_no) {
        case 1: // SIGUSR1: ADC trigger interrupt
        {
            if (chck_interrupt > 0) chck_interrupt--;
            is_interrupt_.set_value(chck_interrupt);
            in_interrupt = 1;
            read_adc();
            in_interrupt = 0;
        }
            break;

        case 2: // SIGUSR2: reset ADCs interrupt
            break;
        default:
            break;
    }
}

int EqFctSpectDiff::read_adc() {
    TDS diff_TDS;
    int cur_map_err = 0, err = 0;
    int cur_brd_err = 0;
    cur_map_err = adc_map_.get_map_err();
    cur_brd_err = adc_map_.get_brd_err();

    if (cur_map_err) {

        g_sts_.online(0);
        set_error(map_error_case, "map error");

        diff_TDS.data = 0;
        diff_TDS.status = sts_err;
        return cur_map_err;
    }

    if (cur_brd_err) {
        g_sts_.online(0);
        set_error(bord_error_case, "bord error");

        diff_TDS.data = 0;
        diff_TDS.status = sts_err;
        return cur_brd_err;
    }

    // Set speclength equal
    int len = sig_->ch_samples_->value();
    bckg_->ch_samples_->set_value(len);
    diff_->length(len);

    // Set spec parameters equal
    sig_->ch_spectrum_->spectrum_parameter(::time(0), 0, 1, 0);
    bckg_->ch_spectrum_->spectrum_parameter(::time(0), 0, 1, 0);
    diff_->spectrum_parameter(::time(0), 0, 1, 0);


    err = sig_->read_adc();
    if (err) {
        set_error(signal_error_case, "ADC error at SIGNAL");
    } else {
        sig_->fill_spectrum(0, len, &err);
        if (err) {
            set_error(spectrum_error_case, "ADC error fill spectrum of SIGNAL");
        }
    }

    err = bckg_->read_adc();
    if (err) {
        set_error(noise_error_case, "ADC error at NOICE");
    } else {
        bckg_->fill_spectrum(0, len, &err);
        if (err) {
            set_error(bkSpectrum_error_case, "ADC error fill spectrum of NOISE");
        }
    }

    float dv;
    int edges = 0;
    startCh_->set_value(len);
    endCh_->set_value(0);
    for (int i = 0; i < len; i++) {
        dv = sig_->ch_spectrum_->read_spectrum(i) -
                bckg_->ch_spectrum_->read_spectrum(i);
        dv = poly_->evaluate(dv);
        diff_->fill_spectrum(i, dv);
        if (dv > thresh_->value() && edges == 0) {
            startCh_->set_value(i);
            edges = 1;
        }
        if (edges == 1 && dv < thresh_->value()) {
            endCh_->set_value(i);
            edges = 2;
        }
    }
    return 0;
}
// ====================================================================


// ========================= CLASS EqFctSpectDiffIBPC =====================

EqFctSpectDiffIBPC::EqFctSpectDiffIBPC() :
lpnAddr_("LPN.ADR laser pulse number", this)
, lpn_("LPN laser pulse number", &lpnAddr_, this)
, accIsOnAddr_("GUNSTS.ADR value of BSTZ", this)
, accIsOn_("GUNSTS value of BSTZ", &accIsOnAddr_, this)
, LowScr1Addr_("LOWSCR1STS.ADR position", this)
, LowScr1_("LOWSCR1STS position", &LowScr1Addr_, this)
, LowScr2Addr_("LOWSCR2STS.ADR position", this)
, LowScr2_("LOWSCR2STS position", &LowScr2Addr_, this)
, LowScr3Addr_("LOWSCR3STS.ADR position", this)
, LowScr3_("LOWSCR3STS position", &LowScr3Addr_, this)
, gunPowerAddr_("GUNPOWER.ADR power in gun", this)
, gunPower_("GUNPOWER power in gun", &gunPowerAddr_, this)
, boosterPowerAddr_("BOOSTERPOWER.ADR power in booster", this)
, boosterPower_("BOOSTERPOWER power in booster", &boosterPowerAddr_, this)
, maxChargeSec_("MAXCURRENT of bunch during second", this)
, maxChargeSec_hist_("MAXCURRENT.HIST of bunch during second", this)
, beamMomentum_("BEAMMOMENTUM in MeV/c", this)
, beamMomentum_hist_("BEAMMOMENTUM.HIST in MeV/c", this)
, beamPower_("BEAMPOWER max beam power pro sec", this)
, beamPower_hist_("BEAMPOWER.HIST max beam power", this)
, powerIntegral_("P_INTEGRAL integrated power of beam", this)
, powerIntegral_hist_("P_INTEGRAL.HIST integrated power of beam", this)
, powerIntegral24_("P_INTEGRAL24 integrated power of beam", this)
, powerIntegral24_hist_("P_INTEGRAL24.HIST integrated power of beam", this)
, powerIntegral24old_("P_INTEGRAL24_OLD integrated power of beam", this)
, screenMask_("SCR_MASK for screens of low section", this)
, LOWSCR1OUT("LOWSCR1OUT value", this)
, LOWSCR2OUT("LOWSCR2OUT value", this)
, LOWSCR3OUT("LOWSCR3OUT value", this)
, powerAddition_("POWERADDITION pro 1 sec", this)
, powerAddition_hist_("POWERADDITION.HIST pro 1 sec", this) {
    g_sts_.online(0);

    list_append();
    conf_done = 1;
}

void EqFctSpectDiffIBPC::init() {
    char HistFile[80];

    //
    // create hist file per location
    //
    sprintf(HistFile, "hist/ICT_%s.hist", name_.value());
    archloc_p = new ArchiveFile(HistFile, ADC_DEV, NOINDEX);

    //////sig_->init(archloc_p, ar_spect_p, ADC_SPECT, ADC_DEV);
    //////bckg_->init(archloc_p, ar_spect_p, ADC_SPECT, ADC_DEV);
    sig_->init(archloc_p);
    bckg_->init(archloc_p);

    res_hist_->book_hist(archloc_p, name_str(), ring_buffer);
    mean_hist_->book_hist(archloc_p, name_str(), ring_buffer);
    rms_hist_->book_hist(archloc_p, name_str(), ring_buffer);
    onech_hist_->book_hist(archloc_p, name_str(), ring_buffer);

    // IBPC specific
    maxChargeSec_hist_.book_hist(archloc_p, name_str(), ring_buffer);
    beamMomentum_hist_.book_hist(archloc_p, name_str(), ring_buffer);
    beamPower_hist_.book_hist(archloc_p, name_str(), ring_buffer);
    powerIntegral_hist_.book_hist(archloc_p, name_str(), ring_buffer);
    powerIntegral24_hist_.book_hist(archloc_p, name_str(), ring_buffer);
    powerAddition_hist_.book_hist(archloc_p, name_str(), ring_buffer);
    powerAddition = 0;
    powerIntegral = powerIntegral_.value();
    powerIntegral24 = powerIntegral24_.value();
}

void EqFctSpectDiffIBPC::fillHistory() {
    // Common spectrum data
    res_->set_value(calc_->evaluate());
    diff_TDS.data = res_->value();
    res_hist_->fill_hist(&diff_TDS);
    diff_TDS.data = onech_->value();
    onech_hist_->fill_hist(&diff_TDS);

    // Pulse specific data
    diff_TDS.data = mean_->value();
    mean_hist_->fill_hist(&diff_TDS);
    diff_TDS.data = rms_->value();
    rms_hist_->fill_hist(&diff_TDS);

    // IBPC specific data
    diff_TDS.data = maxChargeSec_.value();
    maxChargeSec_hist_.fill_hist(&diff_TDS);
    diff_TDS.data = beamMomentum_.value();
    beamMomentum_hist_.fill_hist(&diff_TDS);
    diff_TDS.data = beamPower_.value();
    beamPower_hist_.fill_hist(&diff_TDS);
    powerIntegral_.set_value(powerIntegral);
    diff_TDS.data = powerIntegral;
    powerIntegral_hist_.fill_hist(&diff_TDS);
    powerIntegral24_.set_value(powerIntegral24);
    diff_TDS.data = powerIntegral24;
    powerIntegral24_hist_.fill_hist(&diff_TDS);
    diff_TDS.data = powerAddition;
    powerAddition_hist_.fill_hist(&diff_TDS);
}

void EqFctSpectDiffIBPC::update() {
    bool withBooster = true;
    float tmpVal;
    if (time24hour()) {
        powerIntegral24old_.set_value(powerIntegral24);
        powerIntegral24 = 0;
    }
    //TDS diff_TDS;
    diff_TDS.tm = walltime;

    if (is_interrupt || chck_interrupt > MAX_INT_MISSED) {
        int err = read_adc();
        if (!err) {
            set_error(interrupt_error_case, "NO interrupt anymore");
        }
    }

    is_interrupt_.set_value(chck_interrupt);

    // Read BSTZ for gun state
    tmpVal = accIsOn_.value();
    if (tmpVal == -1) {
        set_error(bstz_error_case, "BSTZ inaccessible");
    } else if (tmpVal != ACCISON) {
        set_error(no_beam_case, "no electron beam");
        beamMomentum_.set_value(0); // no beam
        maxChargeSec_.set_value(0);
        lpn_.set_value(0);
        diff_TDS.status = sts_ok;
        fillHistory();
        return;
    }

    // Read laser pulse number
    if (lpn_.value() == -1) {
        set_error(lpn_error_case, "laser pulse number inaccessible");
        lpn = LPN;
    } else {
        lpn = lpn_.value();
    }

    // Case LOW.SCR1 is not out, mask bit 0
    tmpVal = LowScr1_.value();
    if (tmpVal == -1) {
        set_error(screen1_error_case, "low screen 1 inaccessible");
    }
    if (LowScr1_.value() != LOWSCR1OUT.value()) {
        withBooster = false;
        set_error(no_error_case, "default maximum charge is used: low screen 1");
    }

    // Case LOW.SCR2 is not out, mask bit 1
    tmpVal = LowScr2_.value();
    if (tmpVal == -1) {
        set_error(screen2_error_case, "low screen 2 inaccessible");
    }
    if (LowScr2_.value() != LOWSCR2OUT.value()) {
        set_error(no_error_case, "no booster: low screen 2");
        withBooster = false;
    }

    // Case LOW.SCR3 is not out, mask bit 2
    tmpVal = LowScr3_.value();
    if (tmpVal == -1) {
        set_error(screen3_error_case, "low screen 3 inaccessible");
    }
    if ((LowScr3_.value() - LOWSCR3OUT.value() > 0.1) || (LowScr3_.value() - LOWSCR3OUT.value() < -0.1)) {
        withBooster = false;
        set_error(no_error_case, "no booster: low screen 3");
    }

    // No booster
    if (withBooster == false) {

        if (gunPower_.value() == -1) beamMomentum_.set_value(sqrt(GUNCOEFF * GUNMAXPOWER));
        else beamMomentum_.set_value(sqrt(GUNCOEFF * gunPower_.value())); // gun partition

        //printf("bm = %f * %f = %f: %f\n", GUNCOEFF, gunPower_.value(), GUNCOEFF * gunPower_.value(), sqrt(GUNCOEFF * gunPower_.value()));

        maxChargeSec_.set_value(maxChargeSecValue);
        diff_TDS.status = sts_ok;
        fillHistory();
        return;
    }

    // Case pulse is not found
    float val;
    if (gunPower_.value() == -1) val = sqrt(GUNCOEFF * GUNMAXPOWER); // max power 6
    else val = sqrt(GUNCOEFF * gunPower_.value()); // gun partition
    if (boosterPower_.value() == -1) val += sqrt(BOOSTERCOEFF * BOOSTERMAXPOWER); // max power 5.5
    else val += sqrt(BOOSTERCOEFF * boosterPower_.value()); // booster partition
    beamMomentum_.set_value(val);
    maxChargeSec_.set_value(maxChargeSecValue);

    if (startCh_->value() >= endCh_->value()) {
        set_error(no_pulse_case, "laser pulse not detected: default charge is used");
        diff_TDS.status = sts_ok2;
    } else { // Pulse detected
        set_error(no_error_case, "electron beam detected");
        diff_TDS.status = sts_ok;
    }
    fillHistory();
    return;
}

void EqFctSpectDiffIBPC::interrupt_usr1(int sig_no) {
    switch (sig_no) {
        case 1: // SIGUSR1: ADC trigger interrupt
        {
            if (chck_interrupt > 0) chck_interrupt--;
            is_interrupt_.set_value(chck_interrupt);
            in_interrupt = 1;
            read_adc();
            in_interrupt = 0;
        }
            break;

        case 2: // SIGUSR2: reset ADCs interrupt
            break;
        default:
            break;
    }
}

int EqFctSpectDiffIBPC::read_adc() {
    int cur_map_err = 0, err = 0;
    int cur_brd_err = 0;
    cur_map_err = adc_map_.get_map_err();
    cur_brd_err = adc_map_.get_brd_err();

    if (cur_map_err) {

        g_sts_.online(0);
        set_error(map_error_case, "map_error");
        return cur_map_err;
    }

    if (cur_brd_err) {
        g_sts_.online(0);
        set_error(bord_error_case, "bord error");

        return cur_brd_err;
    }

    // Set speclength equal
    int len = sig_->ch_samples_->value();
    bckg_->ch_samples_->set_value(len);
    diff_->length(len);

    // Set spec parameters equal
    time_t T = ::time(0);
    sig_->ch_spectrum_->spectrum_parameter(T, 0, 1, 0);
    bckg_->ch_spectrum_->spectrum_parameter(T, 0, 1, 0);
    diff_->spectrum_parameter(T, 0, 1, 0);


    err = sig_->read_adc();
    if (err) {
        set_error(signal_error_case, "ADC error at SIGNAL");
    } else {
        sig_->fill_spectrum(0, len, &err);
        if (err) {
            set_error(spectrum_error_case, "ADC error fill spectrum of SIGNAL");
        }
    }

    err = bckg_->read_adc();
    if (err) {
        set_error(bkSpectrum_error_case, "ADC error at NOISE");
    } else {
        bckg_->fill_spectrum(0, len, &err);
        if (err) {
            set_error(noise_error_case, "ADC error fill spectrum of NOISE");
        }
    }

    float dv;
    int edges = 0;
    startCh_->set_value(len);
    endCh_->set_value(0);

    // IBPC specific
    if (LowScr1_.value() != LOWSCR1OUT.value()) { 
        maxInTrain = MAX_POSSIBLE_CHARGE;
    } else {
        maxInTrain = CHARGE_THRESHOLD;
        for (int i = 0; i < len; i++) {
            dv = sig_->ch_spectrum_->read_spectrum(i) -
                    bckg_->ch_spectrum_->read_spectrum(i);
            diff_->fill_spectrum(i, dv);
            if (dv > thresh_->value() && edges == 0) {
                startCh_->set_value(i);
                edges = 1;
            }
            if (edges == 1 && (dv < thresh_->value() || i==len-1)) {
                endCh_->set_value(i);
                edges = 2;
            }

            //IBPC specific
            if (maxInTrain < dv && edges == 1) maxInTrain = dv;
        };
    }

    //IBPC specific
    maxTrainCharge = maxInTrain*lpn;
    maxChargeSec += maxTrainCharge;
    gettimeofday(&TM, 0);
    newT = (double) TM.tv_sec + ((double) TM.tv_usec) / 1000000;
    if (newT - oldT > 1.0) {
        oldT = newT;
        {
            maxChargeSecValue = maxChargeSec;
            //beamMomentum in Mev
            beamPower_.set_value(beamMomentum_.value() * maxChargeSec / 1000.0); // To have it in Watts
            // in kW*h -> divide by 1000 for kW and by 3600 for hour
            powerAddition = (beamMomentum_.value() * maxChargeSec / 1000.0) / 3600000.0;
            powerAddition_.set_value(powerAddition);
            powerIntegral += powerAddition;
            powerIntegral24 += powerAddition;
        }
        maxChargeSec = 0;
    }
    return 0;
}
