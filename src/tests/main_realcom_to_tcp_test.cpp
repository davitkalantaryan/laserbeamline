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
//#include "pitz_rpi_tools_serial.hpp"
#include "common/common_serial_comport.hpp"
#include <stdio.h>
#include <tchar.h>
#include <common/common_servertcp.hpp>
#include <string>
#include "com_port_global_functions.h"
#include "tools_comportserver.hpp"
#include "common_hashtbl.hpp"
#include "tools_ioproxyserver.hpp"

#ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM



int g_nDebugLevel = 1;

int main()
{
#if 0
	common::HashTbl<int> aHash(100);
	int nResultt;

	aHash.AddEntry("Hallo", 5, 1);
	aHash.AddEntry("Hallo2", 6, 2);

	aHash.FindEntry("Hallo", 5, &nResultt);
	printf("nResultt=%d\n", nResultt);

	return 0;
#endif

	//pitz::rpi::tools::Serial aSerial;
	common::serial::ComPort aSerial;
	//tools::ComServer aServer;
	tools::IoProxyServer aServer;

	common::socketN::Initialize();
	PrepareSerial2(&aSerial, _REAL_SERIAL_PORT_NAME_);
	aSerial.SetReadTimeouts(45, 8);
	printf("version 12\n");
	aServer.SetIoDevice(&aSerial);
	aServer.StartServerN();
	aSerial.closeC();
	common::socketN::Cleanup();

	return 0;
}
