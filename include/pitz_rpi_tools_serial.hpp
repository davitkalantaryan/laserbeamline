/*
 *	File: pitz_rpi_tools_serial.hpp
 *
 *	Created on: 11 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */
#ifndef __pitz_rpi_tools_serial_hpp__
#define __pitz_rpi_tools_serial_hpp__

#ifdef WIN32
#include <windows.h>
typedef HANDLE	COM_HANDLE;
#define	INVALID_COM_HANDLE	 INVALID_HANDLE_VALUE 
#else
#endif

namespace pitz{ namespace rpi{ namespace tools{

class Serial
{
public:
	Serial();
	virtual ~Serial();

	int OpenSerial(const char* entry_name);
	int OpenSerial(const wchar_t* entry_name);
	void CloseCom();

	int SetupCommState(const DCB* DcbPtr, const COMMTIMEOUTS* timeouts=NULL,
		int inQueue=512, int outQueue=512);
	int SetupCommState(
		DWORD BaudSet, BYTE Parity, BYTE ByteSize,
		BYTE StopBits, DWORD fInX,
		DWORD fOutX, DWORD fOutxDsrFlow,
		DWORD fOutxCtsFlow,
		DWORD fDtrControl,
		DWORD fRtsControl,
		DWORD ReadIntervalTimeout =10,
		int inQueue = 512, int outQueue = 512);
	int GetCommStates(DCB* DcbPtr, COMMTIMEOUTS* timeouts);

	int Write(const void* data, int data_len);
	int Read1(void* buffer, int buf_len);
	int Read2(void* buffer, int buf_len, long int timeoutMS, long int secondTimeoutMS);
	int Read3(void* a_buffer, int a_buf_len, long int a_lnTimeoutMS,
		const void* a_terminationStr, int a_strLen, bool* a_bFound);
	int Read4(void* buffer, int bufLen, int timeoutFirstMs, int timeoutBtwMs);

	operator COM_HANDLE&();

protected:
	COM_HANDLE		m_handle;
};

}}}


#endif  // #ifndef __pitz_rpi_tools_serial_hpp__
