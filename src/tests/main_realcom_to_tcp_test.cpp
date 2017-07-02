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
#include <aservertcp.h>
#include <string>
#include "com_port_global_functions.h"

#ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
#define	_REAL_SERIAL_PORT_NAME_	"\\.\\COM1"
#endif // #ifdef __ARM

#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511


class ComServer : public AServerTCP
{
	int		AddClient(class ASocketTCP& a_ClientSocket, struct sockaddr_in* bufForRemAddress);
};

int main()
{
	ComServer aServer;
	sockaddr_in aSockAddr;

	ASocketB::Initialize();

	printf("version 2\n");

	aServer.StartServer(9030, 1000,true,&aSockAddr);

	ASocketB::Cleanup();

	return 0;
}

int ComServer::AddClient(class ASocketTCP& a_ClientSocket, struct sockaddr_in* a_bufForRemAddress)
{
	pitz::rpi::tools::Serial serialReal;
	std::string aStrToPrintProg, aStrToPrintDev;
	int dwReadProg, dwReadDev;
	char vcBufferProg[PROG_BUFFER1 + 1], vcBufferDev[DEVICE_BUFFER1 + 1];

	ASocketB::GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("+++++++++++ Connection from host \"%s\"\n", vcBufferProg);
	if (PrepareSerial(&serialReal, _REAL_SERIAL_PORT_NAME_)) { a_ClientSocket.Close(); return 0; }

	while (1) {
		dwReadProg = a_ClientSocket.RecvData(vcBufferProg,PROG_BUFFER1,100000,10);
		if (dwReadProg > 2) {

			aStrToPrintProg = std::string(vcBufferProg, dwReadProg - 2);
			printf("+++++ program : %s\n", aStrToPrintProg.c_str());
			serialReal.Write(vcBufferProg, dwReadProg);
			dwReadDev = serialReal.Read(vcBufferDev, DEVICE_BUFFER1, 50, 30);
			printf("----- device  : ");
			if (dwReadDev > 0) {
				//a_prog->Write(vcBufferDev, dwReadDev);
				a_ClientSocket.SendData(vcBufferDev, dwReadDev);
				aStrToPrintDev = std::string(vcBufferDev, dwReadDev);
				printf("%s", aStrToPrintDev.c_str());
			}
			else if(dwReadDev==0){ a_ClientSocket.SendData("null", 4); }
			printf("\n");

		} // if (dwReadProg > 0) {
		else if (dwReadProg != _SOCKET_TIMEOUT_) 
		{
			a_ClientSocket.Close();
			break;
		}
	} // while (1) {
	
	ASocketB::GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("----------- Client from host \"%s\" disconnected\n", vcBufferProg);
	serialReal.CloseCom();

	return 0;
}
