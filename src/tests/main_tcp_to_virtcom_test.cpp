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

#include <asockettcp.h>
#include "pitz_rpi_tools_serial.hpp"
#include <stdio.h>
#include <string>
#include "com_port_global_functions.h"

#define	_VIRT_SERIAL_PORT_NAME_	"\\.\\COM4"
#define	DEVICE_HOST	"znpi02"

static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, ASocketTCP* a_device);

int main()
{
	ASocketTCP aSocket;
	pitz::rpi::tools::Serial serialProg;

	ASocketB::Initialize();

	if (PrepareSerial(&serialProg, _VIRT_SERIAL_PORT_NAME_)) { return 1; }

	while(1){
		if (aSocket.CreateClient(DEVICE_HOST, 9030)) { return 2; }
		TwoComInOne(&serialProg, &aSocket);
		aSocket.Close();
		Sleep(100);
	}

	serialProg.CloseCom();

	ASocketB::Cleanup();

	return 0;
}


#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511

static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, ASocketTCP* a_device)
{
	std::string aStrToPrintProg, aStrToPrintDev;
	int dwReadProg,dwReadDev;
	char vcBufferProg[PROG_BUFFER1+1], vcBufferDev[DEVICE_BUFFER1+1];
	bool bFound;

	while (1) {
		
		dwReadProg = a_prog->Read(vcBufferProg, PROG_BUFFER1, 100000,"\r\n",2,&bFound);
		if (dwReadProg > 2) {

			aStrToPrintProg = std::string(vcBufferProg, dwReadProg - 2);
			printf("+++++ program : %s\n", aStrToPrintProg.c_str());

			a_device->SendData(vcBufferProg, dwReadProg);
			dwReadDev = a_device->RecvData(vcBufferDev, DEVICE_BUFFER1, 100000, 10);
			printf("----- device  : ");

			if (dwReadDev > 0) {
				if((dwReadDev==4) && (memcmp(vcBufferDev,"null",4)==0)){}
				else{
					aStrToPrintDev = std::string(vcBufferDev, dwReadDev);
					printf("%s\n", aStrToPrintDev.c_str());
					a_prog->Write(vcBufferDev, dwReadDev);
				}
			}
			else if((dwReadDev != 0)&& (dwReadDev != _SOCKET_TIMEOUT_)){
				fprintf(stderr,"server disconnected!\n");
				break;
			}
			printf("\n");

		}
	} // while (1) {
}
