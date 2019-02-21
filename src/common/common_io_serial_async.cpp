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
	Base(&m_ovrlpRead.ovrlp,&m_ovrlpWrite.ovrlp)
{
	memset(&m_ovrlpRead, 0, sizeof(m_ovrlpRead));
	memset(&m_ovrlpWrite, 0, sizeof(m_ovrlpWrite));
	m_ovrlpRead.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovrlpWrite.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	this->SetCallbacks(a_clbkData, a_fpRead, a_fpWrite);
}


serial::Async::~Async()
{
	if(m_ovrlpWrite.ovrlp.hEvent){CloseHandle(m_ovrlpWrite.ovrlp.hEvent);}
	if(m_ovrlpRead.ovrlp.hEvent){CloseHandle(m_ovrlpRead.ovrlp.hEvent);}
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


int serial::Async::ReadSync(void* a_buffer, int a_nBufLen)const
{
	return Base::readC(a_buffer,a_nBufLen);
}


int serial::Async::WriteSync(void* a_buffer, int a_nBufLen)
{
	return Base::writeC(a_buffer, a_nBufLen);
}


int serial::Async::openC(const char* a_comPortName,bool)
{
	return Base::openC(a_comPortName, true);
}


void serial::Async::SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	m_ovrlpRead.clbkData=a_clbkData;
	m_ovrlpWrite.clbkData = a_clbkData;
	m_ovrlpRead.fpRead = a_fpRead?a_fpRead:DefaultReadCallback;
	m_ovrlpWrite.fpWrite = a_fpWrite?a_fpWrite:DefaultWriteCallback;
}


int serial::Async::WaitForReadComplation(int a_timeoutMs)
{
	if(m_ovrlpRead.ovrlp.hEvent){
		return WaitForSingleObjectEx(m_ovrlpRead.ovrlp.hEvent,a_timeoutMs,TRUE)== WAIT_IO_COMPLETION ? 0:-1;
	}
	return -1;
}


int serial::Async::WaitForWriteComplation(int a_timeoutMs)
{
	if (m_ovrlpWrite.ovrlp.hEvent) {
		return WaitForSingleObjectEx(m_ovrlpWrite.ovrlp.hEvent, a_timeoutMs, TRUE) == WAIT_IO_COMPLETION ? 0 : -1;
	}
	return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////

VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	::serial::Async::SOvrlpdRead* pReadStr = lblcontainer_of(a_lpOverlapped, ::serial::Async::SOvrlpdRead, ovrlp);
	pReadStr->fpRead(pReadStr->clbkData, a_dwErrorCode, pReadStr->buffer, a_dwNumberOfBytesTransfered);
	SetEvent(pReadStr->ovrlp.hEvent);
}


VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{

	::serial::Async::SOvrlpdWrite* pWriteStr = lblcontainer_of(a_lpOverlapped, ::serial::Async::SOvrlpdWrite, ovrlp);
	pWriteStr->fpWrite(pWriteStr->clbkData,a_dwErrorCode,pWriteStr->buffer,a_dwNumberOfBytesTransfered);
	SetEvent(pWriteStr->ovrlp.hEvent);
	
}
