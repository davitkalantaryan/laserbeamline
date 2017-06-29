#ifndef lbl_Mirror_h
#define lbl_Mirror_h

#include "Lbl_XYDevice.h"

class EqFctLbl_Mirror : public EqFctLbl_XYDevice {

public:
    EqFctLbl_Mirror();		// constructor
    ~EqFctLbl_Mirror() { ; };	// destructor

    virtual int	fct_code()	{ return CodeLbl_Mirror; }

    static int	mirrorConf_done;
    
    virtual smStatus PerformSeletedCmd(int val);

private:
};

#endif
