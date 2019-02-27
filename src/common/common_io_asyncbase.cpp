// 
// file:			common_io_serial_async.cpp 
// created on:		2019 Feb 20 
// 

#include <common/io/asyncbase.hpp>

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif

namespace __private{ namespace common{ namespace io{ namespace async{

static void DefaultReadCallback(void*, int, const char*, int) {}
static void DefaultWriteCallback(void*, int, const char*, int) {}

}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{


common::io::async::Base::Base(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	memset(&m_ovrlpRead, 0, sizeof(m_ovrlpRead));
	memset(&m_ovrlpWrite, 0, sizeof(m_ovrlpWrite));
	m_ovrlpRead.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovrlpWrite.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	this->SetCallbacks(a_clbkData, a_fpRead, a_fpWrite);
}



common::io::async::Base::~Base()
{
	if(m_ovrlpWrite.ovrlp.hEvent){CloseHandle(m_ovrlpWrite.ovrlp.hEvent);}
	if(m_ovrlpRead.ovrlp.hEvent){CloseHandle(m_ovrlpRead.ovrlp.hEvent);}
}


void common::io::async::Base::SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	m_ovrlpRead.clbkData=a_clbkData;
	m_ovrlpWrite.clbkData = a_clbkData;
	m_ovrlpRead.fpRead = a_fpRead?a_fpRead:&__private::common::io::async::DefaultReadCallback;
	m_ovrlpWrite.fpWrite = a_fpWrite?a_fpWrite: &__private::common::io::async::DefaultWriteCallback;
}


int common::io::async::Base::WaitForReadComplation(int a_timeoutMs)
{
	if(m_ovrlpRead.ovrlp.hEvent){
		DWORD dwCompletion = WAIT_IO_COMPLETION;
		while(dwCompletion == WAIT_IO_COMPLETION){dwCompletion=WaitForSingleObjectEx(m_ovrlpRead.ovrlp.hEvent,a_timeoutMs,TRUE);}
		return (dwCompletion==WAIT_OBJECT_0) ? 0:-1;
	}
	return -1;
}


int common::io::async::Base::WaitForWriteComplation(int a_timeoutMs)
{
	if (m_ovrlpWrite.ovrlp.hEvent) {
		DWORD dwCompletion = WAIT_IO_COMPLETION;
		while(dwCompletion == WAIT_IO_COMPLETION){dwCompletion=WaitForSingleObjectEx(m_ovrlpWrite.ovrlp.hEvent,a_timeoutMs,TRUE);}
		return (dwCompletion==WAIT_OBJECT_0) ? 0:-1;
	}
	return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////

namespace __private{ namespace common{ namespace io{ namespace async{

VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	::common::io::async::Base::SOvrlpdRead* pReadStr = lblcontainer_of(a_lpOverlapped, ::common::io::async::Base::SOvrlpdRead, ovrlp);
	pReadStr->fpRead(pReadStr->clbkData, a_dwErrorCode, pReadStr->buffer, a_dwNumberOfBytesTransfered);
	::SetEvent(pReadStr->ovrlp.hEvent);
}

VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	::common::io::async::Base::SOvrlpdWrite* pWriteStr = lblcontainer_of(a_lpOverlapped, ::common::io::async::Base::SOvrlpdWrite, ovrlp);
	pWriteStr->fpWrite(pWriteStr->clbkData,a_dwErrorCode,pWriteStr->buffer,a_dwNumberOfBytesTransfered);
	::SetEvent(pWriteStr->ovrlp.hEvent);
}

}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{
