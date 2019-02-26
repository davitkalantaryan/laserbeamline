/*
 *	File: main_realcom_to_tcp_test.cpp
 *
 *	Created on: 20 Feb 2019
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */


#define USE_ASYNC_COM

#include <common/tool/argument_parser.hpp>
#include <common/io/serial/async.hpp>
#include <iostream>
#include <string.h>
#include <common/base.hpp>

#define DEFAULT_TIMEOUT		5000 // 1s
#define DEFAULT_ECHO_STRING	"1 nidentify"
//#define t					"1 nidentify"

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif

#ifdef _M_ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM

// echo strings examples
// 1. "1 nidenttify"

static void DefaultReadCallback(void*, int a_errCode, const char* a_pcBuffer, int a_nTransfer);


int main(int a_argc, char* a_argv[])
{
	int nReturn = -1;
	int argc = a_argc - 1;
	int nRW;
	int nTimeout = DEFAULT_TIMEOUT;
	char** argv = a_argv + 1;
	const char* cpcSerialDeviceName;
	const char* cpcStringToEchoInp;
	const char* cpcTimeout;
	char* pcStrinToEcho;
	size_t unStrToEchoLen;
	::common::tool::argument_parser aParser;
#ifdef USE_ASYNC_COM
	::common::io::serial::Async aSerial(&nRW, DefaultReadCallback,NULL);
#else
	::common::io::serial::Sync aSerial;
#endif
	char vcBuffer[1024];
	//COMMTIMEOUTS aTimeouts = { 65536,0,5,0,5 }; // works with sync
	//COMMTIMEOUTS aTimeouts = { 65536,0,1000,0,1000 }; // works fine with sync and async
	//COMMTIMEOUTS aTimeouts = { 65536,65536,65536,65536,65536 };
	//COMMTIMEOUTS aTimeouts = { MAXDWORD,0,65536,0,65536 };
	COMMTIMEOUTS aTimeouts = { 5,0,65536,0,10 };

	printf("version 1!\n");

	aParser.
		AddOption("--com-name,-cn:Name of virtual com port").AddOption("--help,-h:Print this help").
		AddOption("--echo-string,-es:String to echo").AddOption("--timeout,-t:Timeout to wait for answer");

	aParser.ParseCommandLine(argc, argv);

	if (aParser["--help"]) {
		::std::cout << aParser.HelpString() << ::std::endl;
		return 0;
	}

	cpcSerialDeviceName = aParser["--com-name"];
	if(!cpcSerialDeviceName){cpcSerialDeviceName = _REAL_SERIAL_PORT_NAME_;}
	printf("ComPortName=%s\n", cpcSerialDeviceName);

#ifdef _USE_PITZ_RPI_SERIAL
	if(PrepareSerial(&aSerial,cpcSerialDeviceName)){goto reurnPoint;}
#else
	if (aSerial.openC(cpcSerialDeviceName)) {
		::std::cerr << "Unable to open serial " << cpcSerialDeviceName << ::std::endl;
		return -2;
	}
#endif
	SetCommTimeouts((HANDLE)aSerial.handle(), &aTimeouts);
	common::io::serial::MakeStatisticForCom(&aSerial);

	cpcStringToEchoInp = aParser["--echo-string"];
	if(!cpcStringToEchoInp){ cpcStringToEchoInp =DEFAULT_ECHO_STRING;}

	cpcTimeout = aParser["--timeout"];
	if(cpcTimeout){nTimeout=atoi(cpcTimeout);}

	unStrToEchoLen = strlen(cpcStringToEchoInp);
	//unStrToEchoLen += 4;
	pcStrinToEcho=(char*)_alloca(unStrToEchoLen+4);
	memcpy(pcStrinToEcho,cpcStringToEchoInp,unStrToEchoLen);
	pcStrinToEcho[unStrToEchoLen++]=13;
	pcStrinToEcho[unStrToEchoLen++]=10;
	pcStrinToEcho[unStrToEchoLen]=0;
	printf("Sending string(len:%d) %s", (int)unStrToEchoLen, pcStrinToEcho); fflush(stdout);

	nRW=aSerial.writeC(pcStrinToEcho,(int)unStrToEchoLen);
	if(nRW<0){goto reurnPoint;}

	nRW = aSerial.readC(vcBuffer, 1023);
	if (nRW < 0) { goto reurnPoint; }

#if defined(USE_ASYNC_COM) && !defined(_USE_PITZ_RPI_SERIAL)
	if (aSerial.WaitForWriteComplation(nTimeout)) { printf("write timeout!!!\n"); }
	else { printf("write is ok\n"); }

	if (aSerial.WaitForReadComplation(nTimeout)) { printf("read timeout!!!\n");goto reurnPoint; }
#endif

	vcBuffer[nRW] = 0;
	printf("read(%d): %s\n", nRW,vcBuffer);
	
	nReturn = 0;
reurnPoint:
	if (nReturn) {
		common::MakeErrorReport(nullptr, [](void* a_pClbkData, const wchar_t* a_fmt, ...) {
			int nRet;
			va_list aList;
			va_start(aList, a_fmt);
			nRet = vwprintf(a_fmt, aList);
			va_end(aList);
			return nRet;
		});
	}
	aSerial.closeC();
	return 0;
}


static void DefaultReadCallback(void* a_pClbk, int a_errCode, const char* a_pcBuffer, int a_nTransfer) 
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
