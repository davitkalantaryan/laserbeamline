/*
 *	File: main_realcom_to_tcp_test.cpp
 *
 *	Created on: 29 Jun, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */
#include "pitz_rpi_tools_serial.hpp"
#include <stdio.h>
#include <tchar.h>
#include <common_servertcp.hpp>
#include <string>
#include "com_port_global_functions.h"
#include "tools_comportserver.hpp"

#ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM



int g_nDebugLevel = 1;

int main()
{
	pitz::rpi::tools::Serial aSerial;
	tools::ComServer aServer;

	common::SocketBase::Initialize();
	PrepareSerial(&aSerial, _REAL_SERIAL_PORT_NAME_);
	printf("version 10\n");
	aServer.SetSerial(&aSerial);
	aServer.StartServer(9030, 1000,false);
	aSerial.CloseCom();
	common::SocketBase::Cleanup();

	return 0;
}
