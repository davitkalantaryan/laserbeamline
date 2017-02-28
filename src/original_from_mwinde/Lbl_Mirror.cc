#include "Lbl_Mirror.h"

int	EqFctLbl_Mirror::mirrorConf_done = 0;

EqFctLbl_Mirror::EqFctLbl_Mirror() :
    EqFctLbl_XYDevice()
{
    if (!mirrorConf_done) {
	mirrorConf_done = 1;
    }
}

smStatus EqFctLbl_Mirror::PerformSeletedCmd(int val) {
    smStatus stat = 0;
    int baseCmd = val & cmdMask_baseCmds;
    switch(baseCmd) {
	case cmd_zeroPosition:
	    pendingCommand |= cmdS_X_fine;	// force additional fine-tuning
	default: ; // not a command special to EqFctLbl_Mirror
    }
    stat = EqFctLbl_XYDevice::PerformSeletedCmd(val);
    return stat;
}

