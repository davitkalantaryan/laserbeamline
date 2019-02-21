// 
// file:			common_io_serial_base.cpp 
// created on:		2019 Feb 21 
// 


#include "common/io/serial/base.hpp"

using namespace common::io;


serial::Base::Base(OVERLAPPED* a_pOvrlpdRead, OVERLAPPED* a_pOvrlpdWrte)
	:
	m_handle(INVALID_COM_HANDLE),
	m_pOvrlRead(a_pOvrlpdRead),
	m_pOvrlWrite(a_pOvrlpdWrte)
{
}


serial::Base::~Base()
{
}


bool serial::Base::isOpenC(void)const
{
	return m_handle != INVALID_COM_HANDLE;
}


int serial::Base::writeC(const void* a_data, int a_nDataLen)
{
	BOOL bRet;
	DWORD dwWriten;

	bRet = WriteFile(
		m_handle,
		a_data,
		a_nDataLen,
		&dwWriten,
		m_pOvrlWrite);

	if (bRet) { return dwWriten; }
	else if (GetLastError() == ERROR_IO_PENDING) { return 0; }
	return -((int)GetLastError());

}



int serial::Base::readC(void* a_buffer, int a_nBufLen)const
{
	BOOL bRet;
	DWORD dwReaded;

	bRet = ReadFile(m_handle, a_buffer, a_nBufLen, &dwReaded, m_pOvrlRead);//ERROR_IO_PENDING
	
	if (bRet) { return dwReaded; }
	else if(GetLastError()==ERROR_IO_PENDING){return 0;}
	return -((int)GetLastError());
}


common::IODevice* serial::Base::Clone()const
{
	return new serial::Base(*this);
}


int serial::Base::openC(const char* a_comPortName, bool a_bIsOverlabbed)
{
#ifdef _WIN32
	closeC(); // First close any open handle

	m_handle = CreateFileA(
		a_comPortName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		a_bIsOverlabbed?FILE_FLAG_OVERLAPPED:0,
		0);

	return m_handle != INVALID_HANDLE_VALUE ? 0 : -((int)GetLastError());
#else  // #ifdef WIN32
	return -1;
#endif // #ifdef WIN32
}


void serial::Base::closeHard()
{
	//if (hComm && (hComm != INVALID_HANDLE_VALUE)){CloseHandle(hComm);}
#ifdef _WIN32
	if (m_handle && (m_handle != INVALID_HANDLE_VALUE)){CloseHandle(m_handle);}
#else  // #ifdef WIN32
	//return FALSE;
#endif // #ifdef WIN32
	m_handle = INVALID_COM_HANDLE;
}


int serial::Base::setTimeout(int a_timeoutMS)
{
	BOOL bRet;
	COMMTIMEOUTS aTimeout;

	memset(&aTimeout,0,sizeof(COMMTIMEOUTS));
	aTimeout.ReadTotalTimeoutConstant = a_timeoutMS;
	aTimeout.WriteTotalTimeoutConstant = a_timeoutMS;
	bRet = SetCommTimeouts(m_handle, &aTimeout);
	return bRet ? 0 : -1;
}


int serial::Base::SetupCommState(const DCB* a_DcbPtr, const COMMTIMEOUTS* a_timeouts,int a_inQueue, int a_outQueue)
{
	if (!SetupComm(m_handle, a_inQueue, a_outQueue)) {return GetLastError();}
	if (!SetCommState(m_handle, const_cast<DCB*>(a_DcbPtr))) {return GetLastError();}

	if (a_timeouts)
	{
		if (!SetCommTimeouts(m_handle, const_cast<COMMTIMEOUTS*>(a_timeouts))) { return GetLastError(); }
	}

	return 0;
}


int serial::Base::GetCommStates(DCB* a_DcbPtr, COMMTIMEOUTS* a_timeouts)
{
	if (!(::GetCommState(m_handle, a_DcbPtr))) { return GetLastError(); }
	if (!(::GetCommTimeouts(m_handle, a_timeouts))) { return GetLastError(); }
	return 0;
}
