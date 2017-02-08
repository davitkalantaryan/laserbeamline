# File adcict_to_undestand_iobus.pro
# File created : 08 Feb 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
# CONFIG += TEST
# For making test: '$qmake "CONFIG+=TEST" daqadcreceiver.pro' , then '$make'
#

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
QMAKE_CXXFLAGS_WARN_ON -= -Wunused-function

win32:SYSTEM_PATH = ../../../sys/win64
else {
    macx:SYSTEM_PATH = ../../../sys/mac
    else {
        CODENAME = $$system(lsb_release -c | cut -f 2)
        SYSTEM_PATH = ../../../sys/$$CODENAME
    }
}

DESTDIR = $$SYSTEM_PATH/bin
OBJECTS_DIR = $$SYSTEM_PATH/.objects
CONFIG += debug

#CONFIG += c++11
# greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
#greaterThan(QT_MAJOR_VERSION, 4):message("!!!!!! greater than 4")
QT -= core
QT -= gui

LIBS += -L/doocs/lib
#LIBS += /doocs/lib/libADCShm.a
#LIBS += /doocs/lib/libADCShm.a
LIBS += -lADCDma
LIBS += -lADCShm
LIBS += -lEqServer
LIBS += -lDOOCSapi
LIBS += -lrt
LIBS += -lldap
DEFINES += LINUX
INCLUDEPATH += /doocs/lib/include

HEADERS += \
    ../../../src/tests/adcict_server/D_spec_calc.h \
    ../../../src/tests/adcict_server/eq_adcict.h

SOURCES += \
    ../../../src/tests/adcict_server/adcict_rpc_server.cc \
    ../../../src/tests/adcict_server/D_spec_calc.cc

OTHER_FILES += \
    ../../../src/tests/adcict_server/Makefile
