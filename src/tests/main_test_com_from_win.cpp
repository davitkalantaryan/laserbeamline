
#include <WinSock2.h>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "Setupapi.h"
#include <tchar.h>
#include <string>
#include "pitz_rpi_tools_serial.hpp"
#include <thread>

#ifdef _MSC_VER
#if(_MSC_VER >= 1400)
#pragma warning(disable : 4996)
#endif
#endif

//
// DTR Control Flow Values.
//
#define DTR_CONTROL_DISABLE    0x00
#define DTR_CONTROL_ENABLE     0x01
#define DTR_CONTROL_HANDSHAKE  0x02

#define	_REAL_SERIAL_PORT_NAME_	L"\\.\\COM1"

#ifdef _M_ARM
#define	_SERIAL_PORT_NAME_	L"\\\\?\\ACPI#BCM2836#0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
#else  // #ifdef __ARM
//#define	_SERIAL_PORT_NAME_	"COM1"
#define	_SERIAL_PORT_NAME_	L"\\.\\COM4"
#endif // #ifdef __ARM
#define DEBUG_APP(...) do{printf("ln:%d, fnc:%s\n",__LINE__,__FUNCTION__);}while(0)

static int MakeErrorReport(void);

enum class SerialParity {
	None,
	Odd,
	Even,
	Mark,
	Space,
};

enum class SerialStopBits {
	One,
	OnePointFive,
	Two,
};

PCWSTR StringFromSerialParity(SerialParity Parity)
{
	switch (Parity) {
	case SerialParity::None: return L"none";
	case SerialParity::Odd: return L"odd";
	case SerialParity::Even: return L"even";
	case SerialParity::Mark: return L"mark";
	case SerialParity::Space: return L"space";
	default: return L"[invalid parity]";
	}
}

PCWSTR StringFromSerialStopBits(SerialStopBits StopBits)
{
	switch (StopBits) {
	case SerialStopBits::One: return L"1";
	case SerialStopBits::OnePointFive: return L"1.5";
	case SerialStopBits::Two: return L"2";
	default: return L"[invalid serial stop bits]";
	}
}

PCWSTR StringFromDtrControl(DWORD DtrControl)
{
	switch (DtrControl) {
	case DTR_CONTROL_ENABLE: return L"on";
	case DTR_CONTROL_DISABLE: return L"off";
	case DTR_CONTROL_HANDSHAKE: return L"handshake";
	default: return L"[invalid DtrControl value]";
	}
}


PCWSTR StringFromRtsControl(DWORD RtsControl)
{
	switch (RtsControl) {
	case RTS_CONTROL_ENABLE: return L"on";
	case RTS_CONTROL_DISABLE: return L"off";
	case RTS_CONTROL_HANDSHAKE: return L"handshake";
	case RTS_CONTROL_TOGGLE: return L"toggle";
	default: return L"[invalid RtsControl value]";
	}
}


static pitz::rpi::tools::Serial s_serialVirt;
static pitz::rpi::tools::Serial s_serialReal;

static void VirtualComThread(void);
static void RealComThread(void);
static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, pitz::rpi::tools::Serial* a_device);
static void TwoComInOne2(pitz::rpi::tools::Serial* a_prog, pitz::rpi::tools::Serial* a_device);

int _tmain(int argc, wchar_t* argv[])
{
	std::thread aThreadVirt;
	std::thread aThreadReal;

	int nReturn(0);
	const wchar_t *cpcComPortNameVirt(_SERIAL_PORT_NAME_);
	DCB actualDcb;
	COMMTIMEOUTS aTimeouts;

	if(argc>1){ cpcComPortNameVirt =argv[1];}

	if (s_serialVirt.OpenSerial(cpcComPortNameVirt)){
		MakeErrorReport();
		goto retiurnPoint;
	}
	if (s_serialReal.OpenSerial(_REAL_SERIAL_PORT_NAME_)){
		MakeErrorReport();
		goto retiurnPoint;
	}

	if (s_serialVirt.GetCommStates(&actualDcb, &aTimeouts)) {
		MakeErrorReport();
		goto retiurnPoint;
	}
	wprintf(
		L"\n"
		L"                portName = %s\n",
		cpcComPortNameVirt);

	wprintf(
		L"                    baud = %d\n"
		L"                  parity = %s\n"
		L"               data bits = %d\n"
		L"               stop bits = %s\n"
		L"   XON/XOFF flow control = %s\n"
		L" output DSR flow control = %s\n"
		L" output CTS flow control = %s\n"
		L"             DTR control = %s\n"
		L"             RTS control = %s\n"
		L" DSR circuit sensitivity = %s\n",
		actualDcb.BaudRate,
		StringFromSerialParity(SerialParity(actualDcb.Parity)),
		actualDcb.ByteSize,
		StringFromSerialStopBits(SerialStopBits(actualDcb.StopBits)),
		(actualDcb.fInX && actualDcb.fOutX) ? L"on" : L"off",
		actualDcb.fOutxDsrFlow ? L"on" : L"off",
		actualDcb.fOutxCtsFlow ? L"on" : L"off",
		StringFromDtrControl(actualDcb.fDtrControl),
		StringFromRtsControl(actualDcb.fRtsControl),
		actualDcb.fDsrSensitivity ? L"on" : L"off");

	actualDcb.BaudRate = 19200;
	aTimeouts.ReadTotalTimeoutMultiplier = 0;
	aTimeouts.ReadTotalTimeoutConstant = 5;
	aTimeouts.WriteTotalTimeoutMultiplier = 0;
	aTimeouts.WriteTotalTimeoutConstant = 5;
	if (s_serialVirt.SetupCommState(&actualDcb, &aTimeouts)){
		MakeErrorReport();
		goto retiurnPoint;
	}

	/* Do the same for real port */
	if (s_serialReal.GetCommStates(&actualDcb, &aTimeouts)) {
		MakeErrorReport();
		goto retiurnPoint;
	}
	wprintf(
		L"\n"
		L"            portNameReal = %s\n",
		_REAL_SERIAL_PORT_NAME_);

	wprintf(
		L"                    baud = %d\n"
		L"                  parity = %s\n"
		L"               data bits = %d\n"
		L"               stop bits = %s\n"
		L"   XON/XOFF flow control = %s\n"
		L" output DSR flow control = %s\n"
		L" output CTS flow control = %s\n"
		L"             DTR control = %s\n"
		L"             RTS control = %s\n"
		L" DSR circuit sensitivity = %s\n",
		actualDcb.BaudRate,
		StringFromSerialParity(SerialParity(actualDcb.Parity)),
		actualDcb.ByteSize,
		StringFromSerialStopBits(SerialStopBits(actualDcb.StopBits)),
		(actualDcb.fInX && actualDcb.fOutX) ? L"on" : L"off",
		actualDcb.fOutxDsrFlow ? L"on" : L"off",
		actualDcb.fOutxCtsFlow ? L"on" : L"off",
		StringFromDtrControl(actualDcb.fDtrControl),
		StringFromRtsControl(actualDcb.fRtsControl),
		actualDcb.fDsrSensitivity ? L"on" : L"off");

	actualDcb.BaudRate = 19200;
	aTimeouts.ReadTotalTimeoutMultiplier = 0;
	aTimeouts.ReadTotalTimeoutConstant = 5;
	aTimeouts.WriteTotalTimeoutMultiplier = 0;
	aTimeouts.WriteTotalTimeoutConstant = 5;
	if (s_serialVirt.SetupCommState(&actualDcb, &aTimeouts)) {
		MakeErrorReport();
		goto retiurnPoint;
	}

	//aThreadVirt = std::thread(VirtualComThread);
	//aThreadReal = std::thread(RealComThread);

	//aThreadReal.join();
	//aThreadVirt.join();

	TwoComInOne(&s_serialVirt, &s_serialReal);
	//TwoComInOne2(&s_serialVirt, &s_serialReal);

retiurnPoint:
	
	s_serialVirt.CloseCom();
	s_serialReal.CloseCom();
#ifndef __NO_GETCH
	//Sleep(100000);
	printf("Press any key to exit!"); fflush(stdout);
	_getch();
	printf("\n");
#endif

	return 0;
}


static int MakeErrorReport(void)
{
	DWORD dwError = GetLastError();

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		0, // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	_ftprintf(stderr, _T("err_code=%d, text=%s"), (int)dwError, (LPTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return (int)dwError;
}


static void ComThread(pitz::rpi::tools::Serial* a_src, pitz::rpi::tools::Serial* a_dst);

static void VirtualComThread(void)
{
	ComThread(&s_serialVirt,&s_serialReal);
}

static void RealComThread(void)
{
	//ComThread(&s_serialReal, &s_serialVirt);
}


static void ComThread(pitz::rpi::tools::Serial* a_src, pitz::rpi::tools::Serial* a_dst)
{
	const char* cpcFound;
	DWORD dwOffset(0), dwRead, dwWrite;
	char vcBuffer[128];

	while (1) {
		dwRead = a_src->Read1(vcBuffer + dwOffset, 127 - dwOffset);
		if (dwRead > 0) {
			cpcFound = strstr(vcBuffer, "\r\n");

			printf("read\t=\t\"%.20s\"\n", vcBuffer);

			if (cpcFound) {
				dwWrite = (DWORD)((size_t)(cpcFound - vcBuffer)) + 2;
				a_dst->Write(vcBuffer, dwWrite);
				dwOffset = dwRead - dwWrite;
				memmove(vcBuffer, vcBuffer + dwWrite, dwOffset);
			}
			else { dwOffset += dwRead; }
		}
	} // while (1) {
}


#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511

static void TwoComInOne(pitz::rpi::tools::Serial* a_prog, pitz::rpi::tools::Serial* a_device)
{
	const char* cpcFound;
	std::string aStrToPrint;
	int dwOffset(0), dwReadProg,dwReadDev, dwWriteToDev;
	char vcBufferProg[PROG_BUFFER1+1], vcBufferDev[DEVICE_BUFFER1+1];

	while (1) {
		dwReadProg = a_prog->Read2(vcBufferProg + dwOffset, PROG_BUFFER1-dwOffset, 10000,10);
		if (dwReadProg > 0) {

			printf("+++program readed_len= %d\n", dwReadProg);
			
			vcBufferProg[dwReadProg] = 0;
			cpcFound = strstr(vcBufferProg, "\r\n");
			while(cpcFound && (dwReadProg>0)){
				dwOffset = 0;
				dwWriteToDev = (DWORD)((size_t)(cpcFound - vcBufferProg)) + 2;
				aStrToPrint = std::string(vcBufferProg, dwWriteToDev - 2);
				printf("+++program: %s\n", aStrToPrint.c_str());
				a_device->Write(vcBufferProg, dwWriteToDev);
				dwReadProg -= dwWriteToDev;
				if(dwReadProg>0){memmove(vcBufferProg, vcBufferProg + dwWriteToDev, dwReadProg);}
				dwReadDev = a_device->Read2(vcBufferDev, DEVICE_BUFFER1, 25,25);
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


#define PROG_BUFFER2		1024
#define DEVICE_BUFFER2	1024

static void TwoComInOne2(pitz::rpi::tools::Serial* a_prog, pitz::rpi::tools::Serial* a_device)
{
	//std::string aStrToPrint;
	int dwReadProg, dwReadDev;
	char vcBufferProg[PROG_BUFFER2], vcBufferDev[DEVICE_BUFFER2];

	while (1) {
		dwReadProg = a_prog->Read2(vcBufferProg, PROG_BUFFER2, 10000,10);
		if (dwReadProg > 0) {
			printf("+++ BytesFromProg=%d\n", dwReadProg);
			a_device->Write(vcBufferProg, dwReadProg);
			dwReadDev = a_device->Read2(vcBufferDev, DEVICE_BUFFER2, 50,50);

			if(dwReadDev>0){
				printf("--- BytesFromDev =%d\n", dwReadDev);
				a_prog->Write(vcBufferDev, dwReadDev);
			}
		}
	} // while (1) {
}
