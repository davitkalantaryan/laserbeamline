# File laserbeamline_server.pro
# File created : 08 Feb 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
# CONFIG += TEST
# For making test: '$qmake "CONFIG+=TEST" daqadcreceiver.pro' , then '$make'

include(../../libs/common_qt/doocs_server_common.pri)

QT -= core
QT -= gui

INCLUDEPATH += ../../../src/original_from_mwinde
INCLUDEPATH += ../../../src/server
OTHER_FILES += ../../../src/original_from_mwinde/Lbl_Aperture.cc.old \
    ../../../src/original_from_mwinde/laserBeamLine.hist.index \
    ../../../src/original_from_mwinde/laserBeamLine.hist
HEADERS += ../../../src/original_from_mwinde/timeClass.h \
    ../../../src/original_from_mwinde/StepperMotor.h \
    ../../../src/original_from_mwinde/StepperM_MICOS.h \
    ../../../src/original_from_mwinde/SMTrapezProfile.h \
    ../../../src/original_from_mwinde/SMProfile.h \
    ../../../src/original_from_mwinde/simpleThreads.h \
    ../../../src/original_from_mwinde/sema.h \
    ../../../src/original_from_mwinde/RS485_Controller.h \
    ../../../src/original_from_mwinde/OSis.h \
    ../../../src/original_from_mwinde/NumberedStrings.h \
    ../../../src/original_from_mwinde/MICOS_Controller.h \
    ../../../src/original_from_mwinde/messageQ.h \
    ../../../src/original_from_mwinde/Lbl_XYZ_Device.h \
    ../../../src/original_from_mwinde/Lbl_XYDevice.h \
    ../../../src/original_from_mwinde/Lbl_WedgePlates.h \
    ../../../src/original_from_mwinde/Lbl_Mirror.h \
    ../../../src/original_from_mwinde/Lbl_Aperture.h.old \
    ../../../src/original_from_mwinde/Lbl_Aperture.h \
    ../../../src/original_from_mwinde/IPUnidig.h \
    ../../../src/original_from_mwinde/IPUnidig_server.h \
    ../../../src/original_from_mwinde/IPModule.h \
    ../../../src/original_from_mwinde/IP501_Acromag.h \
    ../../../src/original_from_mwinde/IP500_Acromag.h \
    ../../../src/original_from_mwinde/eq_laserBeamLine.h \
    ../../../src/original_from_mwinde/doocsDebug.h \
    ../../../src/original_from_mwinde/DigIO.h \
    ../../../src/original_from_mwinde/definePosixSource.h \
    ../../../src/additional_sources/stdint.h \
    ../../../src/server/pitz_eqfctmirroronedim.hpp
SOURCES += ../../../src/original_from_mwinde/timeClass.cc \
    ../../../src/original_from_mwinde/StepperMotor.cc \
    ../../../src/original_from_mwinde/StepperM_MICOS.cc \
    ../../../src/original_from_mwinde/SMTrapezProfile.cc \
    ../../../src/original_from_mwinde/SMProfile.cc \
    ../../../src/original_from_mwinde/simpleThreads.cc \
    ../../../src/original_from_mwinde/NumberedStrings.cc \
    ../../../src/original_from_mwinde/MICOS_Controller.cc \
    ../../../src/original_from_mwinde/messageQ.cc \
    ../../../src/original_from_mwinde/Lbl_XYZ_Device.cc \
    ../../../src/original_from_mwinde/Lbl_XYDevice.cc \
    ../../../src/original_from_mwinde/Lbl_WedgePlates.cc \
    ../../../src/original_from_mwinde/Lbl_Mirror.cc \
    ../../../src/original_from_mwinde/Lbl_Aperture.cc \
    ../../../src/original_from_mwinde/laserBeamLine_rpc_server.cc \
    ../../../src/original_from_mwinde/IPModule.cc \
    ../../../src/original_from_mwinde/IP500_Acromag.cc \
    ../../../src/additional_sources/vme_functions_simul.cc \
    ../../../src/original_from_mwinde/IPUnidig.cc \
    ../../../src/server/pitz_eqfctmirroronedim.cpp
