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

#include <common_sockettcp.hpp>
#include "pitz_rpi_tools_serial.hpp"
#include <stdio.h>
#include <string>
#include "com_port_global_functions.h"
#include "common_argument_parser.hpp"

#define	VIRT_SERIAL_PORT_NAME		"\\.\\COM5"
#define	DEVICE_HOST2				"znpi02"

static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, common::SocketTCP* a_device);
static bool PrintProgramStrings3(int a_nLength, const char* a_string);

static int s_nDebugLevel = 1;

int main(int argc, char* argv[])
{
	const char* cpcHostName;
	const char* cpcSerialDeviceName= VIRT_SERIAL_PORT_NAME;
	common::SocketTCP aSocket;
	pitz::rpi::tools::Serial serialProg;
	common::argument_parser aParser;
	int nReturn(0);

	common::SocketBase::Initialize();

	printf("version 4!\n");

	aParser.AddOption("--host-name", 1, DEVICE_HOST2).AddOption("--com-name",1, VIRT_SERIAL_PORT_NAME);
	aParser.AddOption("-hn", 1, DEVICE_HOST2).AddOption("-cn", 1, VIRT_SERIAL_PORT_NAME);
	aParser.AddOption("--debug-level",1,"1").AddOption("-dl",1,"1");

	aParser.ParseCommandLine(argc - 1, argv + 1);

	if (aParser["--com-name"]) { cpcSerialDeviceName = aParser["--com-name"]; }
	else if (aParser["-cn"]) { cpcSerialDeviceName = aParser["-cn"]; }

#ifdef DO_DEBUG
	PrepareSerial(&serialProg, cpcSerialDeviceName);
	return 0;
#endif

	if((!aParser["--host-name"])&&(!aParser["-hn"])){
		fprintf(stderr,
			"Host name should be provided\n"
			"Examples:\n"
			"tcp_to_virtcom -hn znpi02\n");
		nReturn = 1;
		goto returnPoint;
	}
	else if(aParser["--host-name"]){cpcHostName = aParser["--host-name"];}
	else { cpcHostName = aParser["-hn"]; }

	if (aParser["--debug-level"]) { s_nDebugLevel = atoi(aParser["--debug-level"]); }
	else if (aParser["-dl"]) { s_nDebugLevel = atoi(aParser["-dl"]); }

	if (PrepareSerial(&serialProg, cpcSerialDeviceName)) { 
		nReturn = 1;
		goto returnPoint;
	}

	while(1){
		if (aSocket.connectC(cpcHostName, 9030)) { continue; }
		TwoComInOne(&serialProg, &aSocket);
		aSocket.closeC();
		Sleep(100);
	}

returnPoint:
	aSocket.closeC();
	serialProg.CloseCom();
	common::SocketBase::Cleanup();

	return nReturn;
}


#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511

static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, common::SocketTCP* a_device)
{
	std::string aStrToPrintDev;
	int dwReadProg,dwReadDev;
	char vcBufferProg[PROG_BUFFER1+1], vcBufferDev[DEVICE_BUFFER1+1];
	bool bWrite(true);
	bool bPrint;

	while (1) {
		
		dwReadProg = a_prog->Read4(vcBufferProg, PROG_BUFFER1, 100000,20);
		if (dwReadProg > 0) {

			if(s_nDebugLevel>0){
				bPrint=PrintProgramStrings3(dwReadProg, vcBufferProg);
			}

			if (bWrite) { a_device->writeC(vcBufferProg, dwReadProg); }
			dwReadDev = a_device->readC(vcBufferDev, DEVICE_BUFFER1, 500);
			if((s_nDebugLevel>0)&&bPrint){printf("----- device (received:%d)  : ",dwReadDev);}

			if (dwReadDev > 0) {
				bWrite = true;
				if((dwReadDev==1) && (vcBufferDev[0]==0)){ }
				else{
					if((s_nDebugLevel>0) && bPrint){
						aStrToPrintDev = std::string(vcBufferDev, dwReadDev);
						printf("%s\n", aStrToPrintDev.c_str());
					}
					a_prog->Write(vcBufferDev, dwReadDev);
				}
			}
			else if((dwReadDev != 0)&& (dwReadDev != _SOCKET_TIMEOUT_)){
				fprintf(stderr,"server disconnected!\n");
				break;
			}
			else { bWrite = false; }
			if((s_nDebugLevel>0) && bPrint){printf("\n");}

		}
	} // while (1) {
}


static bool PrintProgramStrings3(int a_nLength, const char* a_vcBufferProg)
{
	static std::map<std::string, int> sExist;
	std::string aStrToPrintProg;

	if (a_nLength>2) { aStrToPrintProg = std::string(a_vcBufferProg, a_nLength - 2); }
	else { aStrToPrintProg = "UnknownFormat"; }

	if(sExist.count(aStrToPrintProg)){return false;}
	sExist.insert(std::pair<std::string,int>(aStrToPrintProg,1));
	printf("+++++ program (length=%d): %s\n", a_nLength, aStrToPrintProg.c_str());
	if(a_nLength>0){printf("code={%d",(int)a_vcBufferProg[0]);}
	for(int i(1);i<a_nLength;++i){printf(",%d", a_vcBufferProg[i]);}
	if(a_nLength>0){printf("}\n");}
	return true;
}
