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
	if (aSocket.CreateClient(DEVICE_HOST, 9030)) { return 2; }

	TwoComInOne(&serialProg,&aSocket);

	serialProg.CloseCom();

	ASocketB::Cleanup();

	return 0;
}


#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511

static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, ASocketTCP* a_device)
{
	const char* cpcFound;
	std::string aStrToPrint;
	int dwOffset(0), dwReadProg,dwReadDev, dwWriteToDev;
	char vcBufferProg[PROG_BUFFER1+1], vcBufferDev[DEVICE_BUFFER1+1];

	while (1) {
		dwReadProg = a_prog->Read(vcBufferProg + dwOffset, PROG_BUFFER1-dwOffset, 10000,25);
		if (dwReadProg > 0) {

			printf("+++program readed_len= %d\n", dwReadProg);
			
			vcBufferProg[dwReadProg] = 0;
			cpcFound = strstr(vcBufferProg, "\r\n");
			while(cpcFound && (dwReadProg>0)){
				dwOffset = 0;
				dwWriteToDev = (DWORD)((size_t)(cpcFound - vcBufferProg)) + 2;
				aStrToPrint = std::string(vcBufferProg, dwWriteToDev - 2);
				printf("+++program: %s\n", aStrToPrint.c_str());
				//a_device->Write(vcBufferProg, dwWriteToDev);
				a_device->SendData(vcBufferProg, dwWriteToDev);
				dwReadProg -= dwWriteToDev;
				if(dwReadProg>0){memmove(vcBufferProg, vcBufferProg + dwWriteToDev, dwReadProg);}
				//dwReadDev = a_device->Read(vcBufferDev, DEVICE_BUFFER1, 25,25);
				dwReadDev = a_device->RecvData(vcBufferDev, DEVICE_BUFFER1, 125, 25);
				if (dwReadDev > 0) {
					a_prog->Write(vcBufferDev, dwReadDev);
					aStrToPrint = std::string(vcBufferDev, dwReadDev);
					printf("---device : %s\n", aStrToPrint.c_str());
				}
				vcBufferProg[dwReadProg] = 0;
				cpcFound = strstr(vcBufferProg, "\r\n");
			} // while(cpcFound){
			dwOffset += dwReadProg;
		}
	} // while (1) {
}
