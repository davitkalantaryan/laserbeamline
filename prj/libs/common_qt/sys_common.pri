# File sys_common.pri
# File created : 12 Feb 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
#

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
QMAKE_CXXFLAGS_WARN_ON -= -Wunused-function

win32{
    SYSTEM_PATH = ../../../sys/win64
    }else {
        macx:SYSTEM_PATH = ../../../sys/mac
        else {
            DEFINES += LINUX
            CODENAME = $$system(lsb_release -c | cut -f 2)
            SYSTEM_PATH = ../../../sys/$$CODENAME
        }
}

message("!!! sys_common.pri: SYSTEM_PATH=$$SYSTEM_PATH")

# Debug:DESTDIR = debug1
DESTDIR = $$SYSTEM_PATH/bin
OBJECTS_DIR = $$SYSTEM_PATH/.objects
CONFIG += debug

#CONFIG += c++11
# greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
#QT -= core
#QT -= gui
