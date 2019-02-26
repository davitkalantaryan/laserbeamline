/*
 *	File: prepare_com_port_test_win.cpp
 *
 *	Created on: 29 Jun, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include <stdio.h>
#include <tchar.h>
#include <string>
#include "com_port_global_functions.h"


//int MakeErrorReport(void);
//int PrepareSerial(pitz::rpi::tools::Serial* a_pSerial, const char* a_portName);

//int PrepareSerial(pitz::rpi::tools::Serial* a_pSerial, const char* a_portName)
int MakeStatisticForCom(common::serial::ComPort* a_pSerial)
{
	DCB actualDcb;
	COMMTIMEOUTS aTimeouts;
	int nRet(0);

	if (a_pSerial->GetCommStates(&actualDcb, &aTimeouts)) {
		nRet = MakeErrorReport();
		goto returnPoint;
	}
	
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
		L" DSR circuit sensitivity = %s\n"
		L"                timeouts = {%d,%d,%d,%d,%d}\n",
		actualDcb.BaudRate,
		StringFromSerialParity(actualDcb.Parity),
		actualDcb.ByteSize,
		StringFromSerialStopBits(actualDcb.StopBits),
		(actualDcb.fInX && actualDcb.fOutX) ? L"on" : L"off",
		actualDcb.fOutxDsrFlow ? L"on" : L"off",
		actualDcb.fOutxCtsFlow ? L"on" : L"off",
		StringFromDtrControl(actualDcb.fDtrControl),
		StringFromRtsControl(actualDcb.fRtsControl),
		actualDcb.fDsrSensitivity ? L"on" : L"off",
		
		aTimeouts.ReadIntervalTimeout,
		aTimeouts.ReadTotalTimeoutMultiplier,
		aTimeouts.ReadTotalTimeoutConstant,
		aTimeouts.WriteTotalTimeoutMultiplier,
		aTimeouts.WriteTotalTimeoutConstant);

returnPoint:
	if (nRet) { a_pSerial->closeC(); }
	return nRet;
}


int PrepareSerial2_old(common::serial::ComPort* a_pSerial, const char* a_portName)
{
	DCB actualDcb;
	COMMTIMEOUTS aTimeouts;
	int nRet(0);

	if (a_pSerial->OpenCom(a_portName)) {
		nRet = MakeErrorReport();
		goto returnPoint;
	}

	printf(
		"\n"
		"                portName = %s\n",
		a_portName);

	nRet = MakeStatisticForCom(a_pSerial);

	actualDcb.BaudRate = 19200;
	aTimeouts.ReadTotalTimeoutMultiplier = 10;
	aTimeouts.ReadTotalTimeoutConstant = 50000;
	aTimeouts.WriteTotalTimeoutMultiplier = 0;
	aTimeouts.WriteTotalTimeoutConstant = 5;
	if (a_pSerial->SetupCommState(&actualDcb, &aTimeouts)) {
		nRet = MakeErrorReport();
		goto returnPoint;
	}

returnPoint:
	if (nRet) { a_pSerial->closeC(); }
	return nRet;
}


#ifdef _USE_PITZ_RPI_SERIAL

int PrepareSerial(pitz::rpi::tools::Serial* a_pSerial, const char* a_portName)
{
	DCB actualDcb;
	COMMTIMEOUTS aTimeouts;
	int nRet(0);

	if (a_pSerial->openC(a_portName)) {
		nRet = MakeErrorReport();
		goto returnPoint;
	}

	if (a_pSerial->GetCommStates(&actualDcb, &aTimeouts)) {
		nRet = MakeErrorReport();
		goto returnPoint;
	}
	printf(
		"\n"
		"                portName = %s\n",
		a_portName);

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
		StringFromSerialParity(actualDcb.Parity),
		actualDcb.ByteSize,
		StringFromSerialStopBits(actualDcb.StopBits),
		(actualDcb.fInX && actualDcb.fOutX) ? L"on" : L"off",
		actualDcb.fOutxDsrFlow ? L"on" : L"off",
		actualDcb.fOutxCtsFlow ? L"on" : L"off",
		StringFromDtrControl(actualDcb.fDtrControl),
		StringFromRtsControl(actualDcb.fRtsControl),
		actualDcb.fDsrSensitivity ? L"on" : L"off");

#ifdef DO_DEBUG
	printf(
		"\tReadIntervalTimeout         =%d\n"
		"\tReadTotalTimeoutMultiplier  =%d\n"
		"\tReadTotalTimeoutConstant    =%d\n"
		"\tWriteTotalTimeoutMultiplier =%d\n"
		"\tWriteTotalTimeoutConstant   =%d\n\n",
		aTimeouts.ReadIntervalTimeout,
		aTimeouts.ReadTotalTimeoutMultiplier,
		aTimeouts.ReadTotalTimeoutConstant,
		aTimeouts.WriteTotalTimeoutMultiplier,
		aTimeouts.WriteTotalTimeoutConstant);
	goto retiurnPoint;
#endif

	actualDcb.BaudRate = 19200;
	aTimeouts.ReadTotalTimeoutMultiplier = 0;
	aTimeouts.ReadTotalTimeoutConstant = 5;
	aTimeouts.WriteTotalTimeoutMultiplier = 0;
	aTimeouts.WriteTotalTimeoutConstant = 5;
	if (a_pSerial->SetupCommState(&actualDcb, &aTimeouts)) {
		nRet = MakeErrorReport();
		goto returnPoint;
	}

returnPoint:
	if (nRet) { a_pSerial->closeC(); }
	return nRet;
}


#endif  // #ifdef _USE_PITZ_RPI_SERIAL


int MakeErrorReport(void)
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


/*/////////////////////////////////////////////////////////////////////////////*/


