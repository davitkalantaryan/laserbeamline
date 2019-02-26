/*
 *	File: impl.com_port_global_functions.h
 *
 *	Created on: 20 Feb 2019
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */
#ifndef __common_io_serial_impl_base_hpp__
#define __common_io_serial_impl_base_hpp__

#ifndef __common_io_serial_base_hpp__
#error "do not include this file directly""
#include "base.h"
#endif

#if 0

#include <stdio.h>
 //#include "com_port_global_functions.h"
PCWSTR StringFromSerialStopBits(SerialStopBitsT a_stopBits);
PCWSTR StringFromSerialParity(SerialParityT a_parity);
PCWSTR StringFromDtrControl(DWORD a_dtrControl);
PCWSTR StringFromRtsControl(DWORD a_rtsControl);

template<typename SerialType>
int MakeStatisticForComT(SerialType* a_pSerial)
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

#endif  // #if 0


#endif  // #ifndef __common_io_serial_impl_base_hpp__
