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

#ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM

#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511

int g_nDebugLevel = 1;


class ComServer : public common::ServerTCP
{
	void AddClient(common::SocketTCP& a_ClientSocket, const sockaddr_in* bufForRemAddress);
};

int main()
{
	ComServer aServer;

	common::SocketBase::Initialize();
	printf("version 3\n");
	aServer.StartServer(9030, 1000,false);
	common::SocketBase::Cleanup();

	return 0;
}


extern int g_nDebugLevel;

void ComServer::AddClient(common::SocketTCP& a_ClientSocket, const sockaddr_in* a_bufForRemAddress)
{
	pitz::rpi::tools::Serial serialReal;
	std::string aStrToPrintProg, aStrToPrintDev;
	int dwReadProg, dwReadDev;
	char vcBufferProg[PROG_BUFFER1 + 1], vcBufferDev[DEVICE_BUFFER1 + 1];

	GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("+++++++++++ Connection from host \"%s\"\n", vcBufferProg);
	if (PrepareSerial(&serialReal, _REAL_SERIAL_PORT_NAME_)) { a_ClientSocket.closeC(); return; }

	while (1) {
		dwReadProg = a_ClientSocket.readC(vcBufferProg,PROG_BUFFER1,100000);
		if (dwReadProg > 0) {

			if(g_nDebugLevel>0){
				if(dwReadProg>2){aStrToPrintProg = std::string(vcBufferProg, dwReadProg - 2);}
				else { aStrToPrintProg = "UnknownFowmat"; }
				printf("+++++ program : %s\n", aStrToPrintProg.c_str());
			}
			serialReal.Write(vcBufferProg, dwReadProg);
			dwReadDev = serialReal.Read4(vcBufferDev, DEVICE_BUFFER1, 200, 15);
			if (g_nDebugLevel>0){printf("----- device  : ");}
			if (dwReadDev > 0) {
				a_ClientSocket.writeC(vcBufferDev, dwReadDev);
				aStrToPrintDev = std::string(vcBufferDev, dwReadDev);
				if (g_nDebugLevel>0){printf("%s", aStrToPrintDev.c_str());}
			}
			else if(dwReadDev==0){ a_ClientSocket.writeC("", 1); }
			if (g_nDebugLevel>0){printf("\n");}

		} // if (dwReadProg > 0) {
		else if (dwReadProg != _SOCKET_TIMEOUT_) 
		{
			a_ClientSocket.closeC();
			break;
		}
	} // while (1) {
	
	GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("----------- Client from host \"%s\" disconnected\n", vcBufferProg);
	serialReal.CloseCom();
}
