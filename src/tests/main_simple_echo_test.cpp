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
#include <common/common_argument_parser.hpp>
#include <common/io/serial/async.hpp>
#include <com_port_global_functions.h>
#include <iostream>
#include <string.h>

#define DEFAULT_TIMEOUT		1000 // 1s
#define DEFAULT_ECHO_STRING	"1 nidenttify"

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif

#ifdef __ARM
 //#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
 //#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM

// echo strings examples
// 1. "1 nidenttify"

static void DefaultReadCallback(void*, int a_errCode, const char* a_pcBuffer, int a_nTransfer);


int main(int a_argc, char* a_argv[])
{
	int nReturn = -1;
	int argc = a_argc - 1;
	int nRW;
	DWORD dwSleepExReturn;
	int nTimeout = DEFAULT_TIMEOUT;
	char** argv = a_argv + 1;
	const char* cpcSerialDeviceName;
	const char* cpcStringToEchoInp;
	const char* cpcTimeout;
	char* pcStrinToEcho;
	size_t unStrToEchoLen;
	::common::argument_parser aParser;
	::common::io::serial::Async aSerial(NULL, DefaultReadCallback,NULL);
	char vcBuffer[1024];
	COMMTIMEOUTS aTimeouts = { 0 };

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
	if(!cpcSerialDeviceName){
		::std::cerr<<"Com name is not provided!"<< ::std::endl;
		::std::cout << aParser.HelpString() << ::std::endl;
		return -1;
	}
	printf("ComPortName=%s\n", cpcSerialDeviceName);

	if (aSerial.OpenCom(cpcSerialDeviceName)) {
		::std::cerr << "Unable to open serial " << cpcSerialDeviceName << ::std::endl;
		return -2;
	}

	cpcStringToEchoInp = aParser["--echo-string"];
	if(!cpcStringToEchoInp){ cpcStringToEchoInp =DEFAULT_ECHO_STRING;}

	cpcTimeout = aParser["--timeout"];
	if(cpcTimeout){nTimeout=atoi(cpcTimeout);}

	::SetCommTimeouts((HANDLE)aSerial.handle(), &aTimeouts);
	MakeStatisticForComT(&aSerial);

	unStrToEchoLen = strlen(cpcStringToEchoInp);
	//unStrToEchoLen += 4;
	pcStrinToEcho=(char*)_alloca(unStrToEchoLen+4);
	memcpy(pcStrinToEcho,cpcStringToEchoInp,unStrToEchoLen);
	pcStrinToEcho[unStrToEchoLen++]=13;
	pcStrinToEcho[unStrToEchoLen]=10;
	pcStrinToEcho[unStrToEchoLen+1]=0;
	printf("Sending string(len:%d) %s", (int)unStrToEchoLen,cpcStringToEchoInp); fflush(stdout);

	nRW=aSerial.writeC(pcStrinToEcho,(int)unStrToEchoLen);
	if(nRW<0){goto reurnPoint;}

	dwSleepExReturn = SleepEx(nTimeout, TRUE);
	if(dwSleepExReturn==WAIT_IO_COMPLETION){printf("write without timeout!\n");}
	else {printf("write timeout!!!\n");goto reurnPoint;}

#if 0
	nRW=aSerial.readC(vcBuffer,1023);
	if(nRW<0){goto reurnPoint;}

	dwSleepExReturn = SleepEx(INFINITE, TRUE);
	if (dwSleepExReturn == WAIT_IO_COMPLETION) { printf("read without timeout!\n"); }
	else { printf("read timeout!!!\n"); }
#endif
	nRW = aSerial.readSync(vcBuffer, 1023);

	if(nRW>0){ dwSleepExReturn == WAIT_IO_COMPLETION ;}
	else if(nRW==0){  // pending io
		dwSleepExReturn = SleepEx(nTimeout, TRUE);
	}
	else{goto reurnPoint;}

	if (dwSleepExReturn == WAIT_IO_COMPLETION) { 
		printf("read: %s\n", vcBuffer);
	}
	else { printf("read timeout!!!\n"); }
	
	nReturn = 0;
reurnPoint:
	if (nReturn) {MakeErrorReport();}
	aSerial.closeC();
	return 0;
}


static void DefaultReadCallback(void*, int a_errCode, const char* a_pcBuffer, int a_nTransfer) 
{
	if ((!a_errCode) || (a_errCode == ERROR_MORE_DATA)) {
		//fwrite(a_pcBuffer, 1, a_nTransfer, stdout);
	}
	else if (a_errCode) {
		fprintf(stderr, "Error accured!\n");
	}
}
