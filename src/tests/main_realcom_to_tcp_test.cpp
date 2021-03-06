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
#include <common/io/serial/async.hpp>
//#include "common/common_serial_comport.hpp"
#include <common/common_argument_parser.hpp>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <common/common_servertcp.hpp>
#include <string>
#include "com_port_global_functions.h"
#include "tools_comportserver.hpp"
#include "common_hashtbl.hpp"
#include "tools_ioproxyserver.hpp"

#ifdef __ARM
//#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
//#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM

class IoProxyPrivate : public tools::IoProxyServer {
public:
	static void ReadCallback(void* a_pClbk, int a_errCode, const char* a_pcBuffer, int a_nTransfer);
	static void WriteCallback(void* a_pClbk, int a_errCode, const char* a_pcBuffer, int a_nTransfer);
};

int g_nDebugLevel = 1;

int main(int a_argc, char* a_argv[])
{
	int argc = a_argc - 1;
	char** argv = a_argv + 1;
	const char* cpcSerialDeviceName;
	::common::argument_parser aParser;
#if 0
	common::HashTbl<int> aHash(100);
	int nResultt;

	aHash.AddEntry("Hallo", 5, 1);
	aHash.AddEntry("Hallo2", 6, 2);

	aHash.FindEntry("Hallo", 5, &nResultt);
	printf("nResultt=%d\n", nResultt);

	return 0;
#endif
	COMMTIMEOUTS aTimeouts = { 5,0,65536,0,10 };

	printf("version 4!\n");

	aParser.AddOption("--com-name,-cn:Name of virtual com port").AddOption("--help,-h:Print this help");

	aParser.ParseCommandLine(argc, argv);

	if (aParser["--help"]) {
		::std::cout << aParser.HelpString() << ::std::endl;
		return 0;
	}

	cpcSerialDeviceName = aParser["--com-name"];
	if(!cpcSerialDeviceName){
		::std::cerr<<"Com name is not provided!"<< ::std::endl;
		::std::cout << aParser.HelpString() << ::std::endl;
		return -1;
	}
	printf("ComPortName=%s\n", cpcSerialDeviceName);

	//pitz::rpi::tools::Serial aSerial;
	//common::serial::ComPort aSerial;
	//tools::ComServer aServer;
	IoProxyPrivate aServer;
	::common::io::serial::Async aSerial(&aServer, &IoProxyPrivate::ReadCallback, &IoProxyPrivate::WriteCallback);

	common::socketN::Initialize();
	if(aSerial.openC(cpcSerialDeviceName)){
		::std::cerr << "Unable to open serial "<< cpcSerialDeviceName << ::std::endl;
		return -2;
	}
	SetCommTimeouts((HANDLE)aSerial.handle(), &aTimeouts);
	MakeStatisticForComT(&aSerial);
	//aSerial.SetReadTimeouts(45, 8);
	printf("version 12\n");
	aServer.SetIoDevice(&aSerial);
	aServer.StartServerN();
	aSerial.closeC();
	common::socketN::Cleanup();

	return 0;
}


void IoProxyPrivate::ReadCallback(void* a_pClbk, int a_errCode, const char* a_pcBuffer, int a_nTransfer)
{
	if ((!a_errCode) || (a_errCode == ERROR_MORE_DATA)) {
		//fwrite(a_pcBuffer, 1, a_nTransfer, stdout);
		int* pnRead = static_cast<int*>(a_pClbk);
		*pnRead = a_nTransfer;
	}
	else if (a_errCode) {
		fprintf(stderr, "Error accured!\n");
	}
}


void IoProxyPrivate::WriteCallback(void* a_pClbk, int a_errCode, const char* a_pcBuffer, int a_nTransfer)
{
	if ((!a_errCode) || (a_errCode == ERROR_MORE_DATA)) {
		//fwrite(a_pcBuffer, 1, a_nTransfer, stdout);
		int* pnRead = static_cast<int*>(a_pClbk);
		*pnRead = a_nTransfer;
	}
	else if (a_errCode) {
		fprintf(stderr, "Error accured!\n");
	}
}
