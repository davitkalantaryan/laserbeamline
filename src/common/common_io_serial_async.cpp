// 
// file:			common_io_serial_async.cpp 
// created on:		2019 Feb 20 
// 


#include "common/io/serial/async.hpp"

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif


class CommonIoSerialAsyncPrivate : public ::common::io::serial::Async
{	
public:
	static VOID WINAPI OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT(
		_In_    DWORD a_dwErrorCode,
		_In_    DWORD a_dwNumberOfBytesTransfered,
		_Inout_ LPOVERLAPPED a_lpOverlapped);
	static VOID WINAPI OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT(
		_In_    DWORD a_dwErrorCode,
		_In_    DWORD a_dwNumberOfBytesTransfered,
		_Inout_ LPOVERLAPPED a_lpOverlapped);
};

using namespace common::io;




static void DefaultReadCallback (void*, int,const char*, int){}
static void DefaultWriteCallback (void*, int,const char*, int){}


serial::Async::Async(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
	:
	m_handle(INVALID_COM_HANDLE)
{
	this->SetCallbacks(a_clbkData, a_fpRead, a_fpWrite);
}


serial::Async::~Async()
{
}


bool serial::Async::isOpenC(void)const
{
	return m_handle != INVALID_COM_HANDLE;
}


common::IODevice* serial::Async::Clone()const
{
	return new serial::Async(*this);
}

int serial::Async::writeC(const void* a_data, int a_nDataLen)
{
	BOOL bRetByReadEx;

	m_ovrlpWrite.buffer = (char*)a_data;

	bRetByReadEx = WriteFileEx(
		m_handle,
		a_data,
		a_nDataLen,
		&m_ovrlpWrite.ovrlp,
		&CommonIoSerialAsyncPrivate::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT);

	return bRetByReadEx ? 0 : -1;
}


int serial::Async::readC(void* a_buffer, int a_nBufLen)const
{
	BOOL bRetByReadEx;
	
	m_ovrlpRead.buffer = (char*)a_buffer;

	bRetByReadEx = ReadFileEx(
		m_handle,
		a_buffer,
		a_nBufLen,
		&m_ovrlpRead.ovrlp,
		&CommonIoSerialAsyncPrivate::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT );

	return bRetByReadEx?0:-1;
}


int serial::Async::readSync(void* a_buffer, int a_nBufLen)const
{
	BOOL bRet;
	DWORD dwReaded;

	m_ovrlpRead.buffer = (char*)a_buffer;
	bRet = ReadFile(m_handle, a_buffer, a_nBufLen, &dwReaded, &m_ovrlpRead.ovrlp);//ERROR_IO_PENDING
	if (bRet) { return dwReaded; }

	if(GetLastError()==ERROR_IO_PENDING){return 0;}
	return -((int)GetLastError());
}


int serial::Async::OpenCom(const char* a_comPortName)
{
#ifdef WIN32
	closeC(); // First close any open handle

	m_handle = CreateFileA(
		a_comPortName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0);

	return m_handle != INVALID_HANDLE_VALUE ? 0 : -((int)GetLastError());
#else  // #ifdef WIN32
	return -1;
#endif // #ifdef WIN32
}


void serial::Async::SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	memset(&m_ovrlpRead,0,sizeof(m_ovrlpRead));
	memset(&m_ovrlpWrite, 0, sizeof(m_ovrlpWrite));
	m_ovrlpRead.clbkData=a_clbkData;
	m_ovrlpWrite.clbkData = a_clbkData;
	m_ovrlpRead.fpRead = a_fpRead?a_fpRead:DefaultReadCallback;
	m_ovrlpWrite.fpWrite = a_fpWrite?a_fpWrite:DefaultWriteCallback;
}


void serial::Async::closeHard()
{
	//if (hComm && (hComm != INVALID_HANDLE_VALUE)){CloseHandle(hComm);}
#ifdef WIN32
	if (m_handle && (m_handle != INVALID_HANDLE_VALUE)){CloseHandle(m_handle);}
#else  // #ifdef WIN32
	//return FALSE;
#endif // #ifdef WIN32
	m_handle = INVALID_COM_HANDLE;
}


int serial::Async::setTimeout(int a_timeoutMS)
{
	BOOL bRet;
	COMMTIMEOUTS aTimeout;

	memset(&aTimeout,0,sizeof(COMMTIMEOUTS));
	aTimeout.ReadTotalTimeoutConstant = a_timeoutMS;
	aTimeout.WriteTotalTimeoutConstant = a_timeoutMS;
	bRet = SetCommTimeouts(m_handle, &aTimeout);
	return bRet ? 0 : -1;
}


int serial::Async::SetupCommState(const DCB* a_DcbPtr, const COMMTIMEOUTS* a_timeouts,int a_inQueue, int a_outQueue)
{
	if (!SetupComm(m_handle, a_inQueue, a_outQueue)) {return GetLastError();}
	if (!SetCommState(m_handle, const_cast<DCB*>(a_DcbPtr))) {return GetLastError();}

	if (a_timeouts)
	{
		if (!SetCommTimeouts(m_handle, const_cast<COMMTIMEOUTS*>(a_timeouts))) { return GetLastError(); }
	}

	return 0;
}


int serial::Async::GetCommStates(DCB* a_DcbPtr, COMMTIMEOUTS* a_timeouts)
{
	if (!(::GetCommState(m_handle, a_DcbPtr))) { return GetLastError(); }
	if (!(::GetCommTimeouts(m_handle, a_timeouts))) { return GetLastError(); }
	return 0;
}








/////////////////////////////////////////////////////////////////////////////////////////

VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	::serial::Async::SOvrlpdRead* pReadStr = lblcontainer_of(a_lpOverlapped, ::serial::Async::SOvrlpdRead, ovrlp);
	pReadStr->fpRead(pReadStr->clbkData, a_dwErrorCode, pReadStr->buffer, a_dwNumberOfBytesTransfered);
}


VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{

	::serial::Async::SOvrlpdWrite* pWriteStr = lblcontainer_of(a_lpOverlapped, ::serial::Async::SOvrlpdWrite, ovrlp);
	pWriteStr->fpWrite(pWriteStr->clbkData,a_dwErrorCode,pWriteStr->buffer,a_dwNumberOfBytesTransfered);
	
}
