/*
 *	File: main_tcp_to_virtcom_test.cpp
 *
 *	Created on: 29 Jun, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include <common/common_sockettcp.hpp>
#include "common/common_serial_comport.hpp"
#include <stdio.h>
#include <string>
#include "com_port_global_functions.h"
#include "common_argument_parser.hpp"
#include "tools_ioproxy_common_header.h"
#include <common/tools/overlapped_io.hpp>

#define PROG_BUFFER1	511

typedef ::common::tools::SDataForReadAndTransfer TDataForOverlappeedReadCom, TDataForOverlappeedReadSock;

//static bool PrintProgramStrings3(int a_nLength, const char* a_string);

static int s_nDebugLevel = 1;

int main(int a_argc, char* a_argv[])
{
	int nReturn = -1;
	const char* cpcHostName;
	const char* cpcSerialDeviceName;
	const char* cpcDebugLevel;
	common::SocketTCP aSocket;
	common::serial::ComPort serialProg;
	common::argument_parser aParser;
	int argc = a_argc - 1;
	char** argv = a_argv + 1;
	char vcBufferCom[PROG_BUFFER1 + 1], vcBufferSock[PROG_BUFFER1 + 1];

#ifdef _WIN32
	TDataForOverlappeedReadSock ovrReadSock(NULL,vcBufferSock, PROG_BUFFER1,&serialProg,NULL);
	TDataForOverlappeedReadCom ovrReadCom(NULL,vcBufferCom, PROG_BUFFER1,&aSocket,NULL);
	BOOL bRetByReadEx;
#else
#endif

	common::socketN::Initialize();

	printf("version 4!\n");

	aParser.
		AddOption("--host-name,-hn:Name of server host").
		AddOption("--com-name,-cn:Name of virtual com port").
		AddOption("--debug-level,-dl:Verbosity of program").
		AddOption("--help,-h:Print this help");

	aParser.ParseCommandLine(argc, argv);

	if(aParser["--help"]){ nReturn=0;goto returnPoint;}

	cpcSerialDeviceName = aParser["--com-name"];
	if (!cpcSerialDeviceName) { goto returnPoint; }

#ifdef DO_DEBUG
	PrepareSerial(&serialProg, cpcSerialDeviceName);
	return 0;
#endif

	cpcHostName = aParser["--host-name"];
	if (!cpcHostName) { goto returnPoint; }

	cpcDebugLevel = aParser["--debug-level"];
	if (cpcDebugLevel) { s_nDebugLevel= atoi(cpcDebugLevel); }

	if (PrepareSerial2(&serialProg, cpcSerialDeviceName)) { 
		nReturn = 1;
		goto returnPoint;
	}

	//////////////////////////////////////////////////
#ifdef _WIN32
	
	ovrReadCom.run = 1;
	ovrReadCom.handle = (HANDLE)serialProg.handle();

	while (ovrReadCom.run) {
		if (aSocket.connectC(cpcHostName, IO_PROXY_PORT_NAME,5000)) { continue; }
		ovrReadSock.handle = (HANDLE)aSocket.handle();
		bRetByReadEx = ReadFileEx(
			ovrReadCom.handle,
			vcBufferCom,
			PROG_BUFFER1,
			&ovrReadCom.ovrlp,
			&common::tools::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN );
		if (!bRetByReadEx) { break; }

		ovrReadSock.run = 1;
		bRetByReadEx = ReadFileEx(
			ovrReadSock.handle,
			vcBufferSock,
			PROG_BUFFER1,
			&ovrReadSock.ovrlp,
			&common::tools::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN );
		if (!bRetByReadEx) { ovrReadSock.run = 0; aSocket.closeC(); continue; }
		
		while (ovrReadSock.run && ovrReadCom.run) {
			SleepEx(INFINITE, TRUE);
		}
		ovrReadSock.run = 0; aSocket.closeC();
	}

#else   // #ifdef _WIN32
#endif  // #ifdef _WIN32
	//////////////////////////////////////////////////


	nReturn = 0;
returnPoint:
	aSocket.closeC();
	serialProg.closeC();
	common::socketN::Cleanup();

	return nReturn;
}


#if 0
static bool PrintProgramStrings3(int a_nLength, const char* a_vcBufferProg)
{
	static std::map<std::string, int> sExist;
	std::string aStrToPrintProg;

	if (a_nLength>2) { aStrToPrintProg = std::string(a_vcBufferProg, a_nLength - 2); }
	else { aStrToPrintProg = "UnknownFormat"; }

	//if(sExist.count(aStrToPrintProg)){return false;}
	//sExist.insert(std::pair<std::string,int>(aStrToPrintProg,1));
	printf("+++++ program (length=%d): %s\n", a_nLength, aStrToPrintProg.c_str());
	//if(a_nLength>0){printf("code={%d",(int)a_vcBufferProg[0]);}
	//for(int i(1);i<a_nLength;++i){printf(",%d", a_vcBufferProg[i]);}
	//if(a_nLength>0){printf("}\n");}
	return true;
}
#endif  // #if 0
