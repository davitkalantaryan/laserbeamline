QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
QMAKE_CXXFLAGS_WARN_ON -= -Wunused-function
LIBS += -L/doocs/lib
LIBS += -lDOOCSapi
LIBS += -lEqServer
LIBS += -lrt
LIBS += -lldap
DEFINES += LINUX
INCLUDEPATH += /doocs/lib/include
INCLUDEPATH += ../
OTHER_FILES += ../Lbl_Aperture.cc.old \
    ../laserBeamLine.hist.index \
    ../laserBeamLine.hist
HEADERS += ../timeClass.h \
    ../StepperMotor.h \
    ../StepperM_MICOS.h \
    ../SMTrapezProfile.h \
    ../SMProfile.h \
    ../simpleThreads.h \
    ../sema.h \
    ../RS485_Controller.h \
    ../OSis.h \
    ../NumberedStrings.h \
    ../MICOS_Controller.h \
    ../messageQ.h \
    ../Lbl_XYZ_Device.h \
    ../Lbl_XYDevice.h \
    ../Lbl_WedgePlates.h \
    ../Lbl_Mirror.h \
    ../Lbl_Aperture.h.old \
    ../Lbl_Aperture.h \
    ../IPUnidig.h \
    ../IPUnidig_server.h \
    ../IPModule.h \
    ../IP501_Acromag.h \
    ../IP500_Acromag.h \
    ../eq_laserBeamLine.h \
    ../doocsDebug.h \
    ../DigIO.h \
    ../definePosixSource.h
SOURCES += ../timeClass.cc \
    ../StepperMotor.cc \
    ../StepperM_MICOS.cc \
    ../SMTrapezProfile.cc \
    ../SMProfile.cc \
    ../simpleThreads.cc \
    ../NumberedStrings.cc \
    ../MICOS_Controller.cc \
    ../messageQ.cc \
    ../Lbl_XYZ_Device.cc \
    ../Lbl_XYDevice.cc \
    ../Lbl_WedgePlates.cc \
    ../Lbl_Mirror.cc \
    ../Lbl_Aperture.cc \
    ../laserBeamLine_rpc_server.cc \
    ../IPModule.cc \
    ../IP501_Acromag.cc \
    ../IP500_Acromag.cc \
    ../additional_sources/vme_functions_simul.cc \
    ../IPUnidig.cc
