#
# File doocs_client_common.pri
# File created : 12 Feb 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
#

message("!!! doocs_client_common.pri:")

LIBS += -L/doocs/lib
LIBS += -lDOOCSapi
LIBS += -lldap
LIBS += -lrt

include(../../libs/common_qt/sys_common.pri)

INCLUDEPATH += /doocs/lib/include
