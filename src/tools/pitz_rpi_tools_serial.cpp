/*
 *	File: pitz_rpi_tools_serial.cpp
 *
 *	Created on: 11 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include "pitz_rpi_tools_serial.hpp"
#include <stdio.h>  
#include <sys/timeb.h>  
#include <time.h>


pitz::rpi::tools::Serial::Serial()
	:
	m_handle(INVALID_COM_HANDLE)
{
}

pitz::rpi::tools::Serial::~Serial()
{
	CloseCom();
}


int pitz::rpi::tools::Serial::Write(const void* a_data, int a_data_len)
{
	DWORD dwWritten;
	BOOL bRet = WriteFile(m_handle, a_data, a_data_len, &dwWritten, NULL);
	if (bRet){return dwWritten;}
	return -((int)GetLastError());
}


int pitz::rpi::tools::Serial::Read1(void* a_buffer, int a_buf_len)
{
	DWORD dwReaded;
	BOOL bRet = ReadFile(m_handle, a_buffer, a_buf_len, &dwReaded, NULL);
	if (bRet) { return dwReaded; }
	return -((int)GetLastError());
}


int pitz::rpi::tools::Serial::Read2(void* a_buffer, int a_buf_len, long int a_lnTimeoutMS, long int a_lnSecondTimeoutMS)
{
	COMMTIMEOUTS atimeouts0, atimeouts;
	DWORD dwReaded;
	BOOL bRet;

	if (!(::GetCommTimeouts(m_handle, &atimeouts0))) { return GetLastError(); }
	atimeouts=atimeouts0;
	atimeouts.ReadTotalTimeoutMultiplier=0;atimeouts.ReadTotalTimeoutConstant=a_lnTimeoutMS;
	if (!SetCommTimeouts(m_handle, &atimeouts)) { return GetLastError(); }
	bRet=ReadFile(m_handle, a_buffer, 1, &dwReaded, NULL);
	if(bRet && (dwReaded>0) && (a_buf_len>1)){
		atimeouts.ReadTotalTimeoutConstant=a_lnSecondTimeoutMS;
		SetCommTimeouts(m_handle, &atimeouts);
		bRet=ReadFile(m_handle, ((char*)a_buffer)+1, a_buf_len-1, &dwReaded, NULL);
		if (bRet){++dwReaded;}
	}
	SetCommTimeouts(m_handle, &atimeouts0);
	if (bRet) { return dwReaded; }
	return -((int)GetLastError());
}


int pitz::rpi::tools::Serial::Read3(void* a_buffer, int a_buf_len, long int a_lnTimeoutMS,
	const void* a_terminationStr, int a_strLen, bool* a_bFound)
{
	char* pcBufferIn = (char*)a_buffer;
	char* pcBuffer = pcBufferIn;
	COMMTIMEOUTS atimeouts0, atimeouts;
	long int lnTimePassed;
	long int lnTimeoutRemained(a_lnTimeoutMS);
	int dwReadedAll(0);
	//int nBufLenToRead;
	DWORD dwReadedSngl;
	BOOL bRet;
	struct timeb timeBufferIn, timeBufferFnl;

	*a_bFound = false;
	if (a_strLen>a_buf_len) { return -2; }

	ftime(&timeBufferIn);
	if (!(::GetCommTimeouts(m_handle, &atimeouts0))) { return GetLastError(); }
	atimeouts = atimeouts0;
	atimeouts.ReadTotalTimeoutMultiplier = 0;

	atimeouts.ReadTotalTimeoutConstant = lnTimeoutRemained;
	SetCommTimeouts(m_handle, &atimeouts);
	bRet = ReadFile(m_handle, pcBuffer, a_strLen, &dwReadedSngl, NULL);

	while (
		bRet && (dwReadedSngl>0) &&
		((dwReadedAll + ((int)dwReadedSngl))<a_buf_len) && (lnTimeoutRemained>0)) {

		dwReadedAll += dwReadedSngl;
		if (memcmp(pcBufferIn++, a_terminationStr, a_strLen) == 0) {
			*a_bFound = true; break;
		}

		ftime(&timeBufferFnl);
		lnTimePassed = 1000 * ((long int)(timeBufferFnl.time - timeBufferIn.time)) +
			(((long int)timeBufferFnl.millitm) - ((long int)timeBufferIn.millitm));

		lnTimeoutRemained = lnTimePassed>a_lnTimeoutMS ? 0 : (a_lnTimeoutMS - lnTimePassed);
		atimeouts.ReadTotalTimeoutConstant = lnTimeoutRemained;

		SetCommTimeouts(m_handle, &atimeouts);
		pcBuffer += dwReadedSngl;
		bRet = ReadFile(m_handle, pcBuffer, 1, &dwReadedSngl, NULL);
	}

	SetCommTimeouts(m_handle, &atimeouts0);
	if (bRet) { return dwReadedAll; }
	return -((int)GetLastError());
}

#define MAKE_MINUS(_x_) ((_x_)>0 ? -(_x_):(_x_))


int pitz::rpi::tools::Serial::Read4(void* a_buffer,int a_nBufLen,int a_nTimeoutFirstMs,int a_nTimeoutBtwMs)
{
	COMMTIMEOUTS atimeouts0, atimeouts;
	DWORD dwReaded;
	BOOL bRet;

	if (!(::GetCommTimeouts(m_handle, &atimeouts0))) { 
		int nRet(GetLastError());
		return MAKE_MINUS(nRet); 
	}
	atimeouts = atimeouts0;
	atimeouts.ReadIntervalTimeout = a_nTimeoutBtwMs;
	atimeouts.ReadTotalTimeoutConstant = a_nTimeoutFirstMs;
	atimeouts.ReadTotalTimeoutMultiplier = 0;
	if (!(::SetCommTimeouts(m_handle, &atimeouts))) {
		int nRet(GetLastError());
		return MAKE_MINUS(nRet);
	}
	bRet = ReadFile(m_handle, a_buffer, a_nBufLen, &dwReaded, NULL);
	if (!bRet) { 
		int nRet(GetLastError());
		return MAKE_MINUS(nRet);
	}
	::SetCommTimeouts(m_handle, &atimeouts0);
	return dwReaded;
}


int pitz::rpi::tools::Serial::OpenSerial(const char* a_entry_name)
{
#ifdef WIN32
	CloseCom(); // First close any open handle

	m_handle = CreateFileA(
		a_entry_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);

	return m_handle != INVALID_HANDLE_VALUE ? 0 : -((int)GetLastError());
#else  // #ifdef WIN32
	return -1;
#endif // #ifdef WIN32
}


int pitz::rpi::tools::Serial::OpenSerial(const wchar_t* a_entry_name)
{
#ifdef WIN32
	CloseCom(); // First close any open handle

	m_handle = CreateFileW(
		a_entry_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);

	return m_handle != INVALID_HANDLE_VALUE ? 0 : -((int)GetLastError());
#else  // #ifdef WIN32
	return -1;
#endif // #ifdef WIN32
}


void pitz::rpi::tools::Serial::CloseCom()
{
	//if (hComm && (hComm != INVALID_HANDLE_VALUE)){CloseHandle(hComm);}
#ifdef WIN32
	if (m_handle && (m_handle != INVALID_HANDLE_VALUE)){CloseHandle(m_handle);}
#else  // #ifdef WIN32
	//return FALSE;
#endif // #ifdef WIN32
	m_handle = INVALID_COM_HANDLE;
}


int pitz::rpi::tools::Serial::SetupCommState(const DCB* a_DcbPtr, const COMMTIMEOUTS* a_timeouts,
	int a_inQueue, int a_outQueue)
{
	if (!SetupComm(m_handle, a_inQueue, a_outQueue)) {return GetLastError();}
	if (!SetCommState(m_handle, const_cast<DCB*>(a_DcbPtr))) {return GetLastError();}

	if (a_timeouts)
	{
		if (!SetCommTimeouts(m_handle, const_cast<COMMTIMEOUTS*>(a_timeouts))) { return GetLastError(); }
	}

	return 0;
}


int pitz::rpi::tools::Serial::SetupCommState(
	DWORD a_BaudSet, BYTE a_Parity, BYTE a_ByteSize,
	BYTE a_StopBits, DWORD a_fInX,
	DWORD a_fOutX, DWORD a_fOutxDsrFlow,
	DWORD a_fOutxCtsFlow,
	DWORD a_fDtrControl,
	DWORD a_fRtsControl,
	DWORD a_ReadIntervalTimeout,
	int a_inQueue, int a_outQueue)
{
	int nReturn(0);
	DCB	aDcb;
	COMMTIMEOUTS aTimeouts;

	if ((nReturn = this->GetCommStates(&aDcb, &aTimeouts))) { return nReturn; }

	aDcb.BaudRate = a_BaudSet;
	aDcb.Parity = a_Parity;
	// ...
	aTimeouts.ReadIntervalTimeout = a_ReadIntervalTimeout;
	return SetupCommState(&aDcb, &aTimeouts, a_inQueue, a_outQueue);
}


int pitz::rpi::tools::Serial::GetCommStates(DCB* a_DcbPtr, COMMTIMEOUTS* a_timeouts)
{
	if (!(::GetCommState(m_handle, a_DcbPtr))) { return GetLastError(); }
	if (!(::GetCommTimeouts(m_handle, a_timeouts))) { return GetLastError(); }
	return 0;
}


pitz::rpi::tools::Serial::operator COM_HANDLE&()
{
	return m_handle;
}
