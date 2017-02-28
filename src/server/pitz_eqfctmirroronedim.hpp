/*
 *	File: pitz_eqfctmirroronedim.hpp
 *
 *	Created on: 28 Feb 2017
 *	Created by: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This is the header file for the class pitz::EqFctMirrorOneDim
 *  This class is the mimicrate of the class EqFctLbl_Mirror
 *
 */
#ifndef PITZ_EQFCTMIRRORONEDIM_HPP
#define PITZ_EQFCTMIRRORONEDIM_HPP

#define CodeLbl_OneDimMirror 311

#include "Lbl_XYDevice.h"

namespace pitz{


class EqFctMirrorOneDim : public EqFctLbl_XYDevice {

public:
    EqFctMirrorOneDim();		// constructor
    virtual ~EqFctMirrorOneDim();	// destructor

protected:
    virtual int	fct_code();
    virtual void init();
    smStatus PerformSeletedCmd(int val);

    //virtual void	update ();
    virtual void updateAllDrives();
    virtual void FillHistories(int error);
    virtual void CreateDrives();
    virtual void initHardware();
    virtual void online();

    //void PerformCommand(int val, D_int * itsButton);
    virtual smStatus TryReady();
    //void commandFinished(smStatus stat);

    //NumberedStrings* ErrorTexts() {return errorTexts;}

    //virtual smStatus SetAsNominal(int val);

    virtual void NotifyStatus();
    //smStatus NotifyAxisStatus(smStatus mostSignificantErr, smStatus thisErr,
    //    bool destinationReachable, D_int* errNb, D_string* errTxt);
    //void NotifyOverallStatus(smStatus stat);

};

}

#endif // PITZ_EQFCTMIRRORONEDIM_HPP
