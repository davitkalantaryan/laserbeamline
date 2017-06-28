# File laserbeamline_server.pro
# File created : 08 Feb 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
# CONFIG += TEST
# For making test: '$qmake "CONFIG+=TEST" daqadcreceiver.pro' , then '$make'

include(../../libs/common_qt/doocs_client_common.pri)

greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
CONFIG += c++11
INCLUDEPATH += ../../../include

SOURCES += \
    ../../../src/client/main_polux_client.cpp \
    ../../../src/client/pitz_poluxclient_application.cpp \
    ../../../src/client/pitz_poluxclient_mainwindow.cpp \
    ../../../src/client/pitz_poluxclient_centralwidget.cpp \
    ../../../src/client/pitz_poluxclient_connectiondetails.cpp

HEADERS += \
    ../../../src/client/pitz_poluxclient_application.hpp \
    ../../../src/client/pitz_poluxclient_mainwindow.hpp \
    ../../../src/client/pitz_poluxclient_centralwidget.hpp \
    ../../../include/unnamedsemaphorelite.hpp \
    ../../../include/pitz_tool_fifo.hpp \
    ../../../include/pitz_tool_fifo.tos \
    ../../../src/client/pitz_poluxclient_connectiondetails.hpp

RESOURCES += \
    ../../../src/resources/toolbar1.qrc

