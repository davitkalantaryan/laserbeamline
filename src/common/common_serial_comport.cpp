/*
 *	File: common_serial_comport.cpp
 *
 *	Created on: 21 Jul, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include "common_serial_comport.hpp"

#define MAKE_MINUS(_x_) ((_x_)>0 ? -(_x_):(_x_))


common::serial::ComPort::ComPort()
	:
	m_handle(INVALID_COM_HANDLE)
{
}


common::serial::ComPort::~ComPort()
{
}


bool common::serial::ComPort::isOpenC(void)const
{
	return m_handle != INVALID_COM_HANDLE;
}


common::IODevice* common::serial::ComPort::Clone()const
{
	return new ::common::serial::ComPort(*this);
}

int common::serial::ComPort::writeC(const void* a_data, int a_nDataLen)
{
	DWORD dwWritten;
	BOOL bRet = WriteFile(m_handle, a_data, a_nDataLen, &dwWritten, NULL);
	if (bRet){return dwWritten;}
	return -((int)GetLastError());
}


int common::serial::ComPort::readC(void* a_buffer, int a_nBufLen)const
{
	DWORD dwReaded;
	BOOL bRet = ReadFile(m_handle, a_buffer,a_nBufLen, &dwReaded, NULL);
	if (bRet) { return dwReaded; }
	return -((int)GetLastError());
}


int common::serial::ComPort::readC(void* a_buffer, int a_nBufLen, int a_nTimeoutFirstMs)const
{
	COMMTIMEOUTS atimeouts0, atimeouts;
	DWORD dwReaded;
	BOOL bRet;

	if (!(::GetCommTimeouts(m_handle, &atimeouts0))) {
		int nRet(GetLastError());
		return MAKE_MINUS(nRet);
	}
	atimeouts = atimeouts0;
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


int	common::serial::ComPort::SetReadTimeouts(int a_nTimeoutFirstMs, int a_nTimeoutBtwMs)
{
	COMMTIMEOUTS atimeouts0, atimeouts;

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
	return 0;
}


int common::serial::ComPort::Read2(void* a_buffer, int a_nBufLen, int a_nTimeoutFirstMs, int a_nTimeoutBtwMs)
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


int common::serial::ComPort::OpenCom(const char* a_comPortName)
{
#ifdef WIN32
	closeC(); // First close any open handle

	m_handle = CreateFileA(
		a_comPortName,
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


void common::serial::ComPort::closeHard()
{
	//if (hComm && (hComm != INVALID_HANDLE_VALUE)){CloseHandle(hComm);}
#ifdef WIN32
	if (m_handle && (m_handle != INVALID_HANDLE_VALUE)){CloseHandle(m_handle);}
#else  // #ifdef WIN32
	//return FALSE;
#endif // #ifdef WIN32
	m_handle = INVALID_COM_HANDLE;
}


int common::serial::ComPort::SetupCommState(const DCB* a_DcbPtr, const COMMTIMEOUTS* a_timeouts,
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


int common::serial::ComPort::SetupCommState(
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


int common::serial::ComPort::GetCommStates(DCB* a_DcbPtr, COMMTIMEOUTS* a_timeouts)
{
	if (!(::GetCommState(m_handle, a_DcbPtr))) { return GetLastError(); }
	if (!(::GetCommTimeouts(m_handle, a_timeouts))) { return GetLastError(); }
	return 0;
}


common::serial::ComPort::operator COM_HANDLE&()
{
	return m_handle;
}

