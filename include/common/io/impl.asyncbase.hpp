// 
// file:			common_io_serial_async.cpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_serial_impl_asyncbase_hpp__
#define __common_io_serial_impl_asyncbase_hpp__


#ifndef __common_io_serial_asyncbase_hpp__
#error "Do not include this file directly"
#include "asyncbase.hpp"
#endif

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif

namespace __private{ namespace common{ namespace io{ namespace async{

template <typename IoBase>
class CommonIoSerialAsyncPrivate : public ::common::io::async::Base< IoBase >
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

static void DefaultReadCallback(void*, int, const char*, int) {}
static void DefaultWriteCallback(void*, int, const char*, int) {}

}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{


template <typename BaseIoDevice>
common::io::async::Base<BaseIoDevice>::Base(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	memset(&m_ovrlpRead, 0, sizeof(m_ovrlpRead));
	memset(&m_ovrlpWrite, 0, sizeof(m_ovrlpWrite));
	m_ovrlpRead.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovrlpWrite.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	this->SetCallbacks(a_clbkData, a_fpRead, a_fpWrite);
}


template <typename BaseIoDevice>
common::io::async::Base<BaseIoDevice>::~Base()
{
	if(m_ovrlpWrite.ovrlp.hEvent){CloseHandle(m_ovrlpWrite.ovrlp.hEvent);}
	if(m_ovrlpRead.ovrlp.hEvent){CloseHandle(m_ovrlpRead.ovrlp.hEvent);}
}


template <typename BaseIoDevice>
common::io::Device* common::io::async::Base<BaseIoDevice>::Clone()const
{
	return new common::io::async::Base<BaseIoDevice>(*this);
}


template <typename BaseIoDevice>
int common::io::async::Base<BaseIoDevice>::writeC(const void* a_data, int a_nDataLen)
{
#ifdef _WIN32
	BOOL bRetByReadEx;

	m_ovrlpWrite.buffer = (char*)a_data;

	bRetByReadEx = WriteFileEx(
		reinterpret_cast<HANDLE>(BaseIoDevice::m_handle),
		a_data,
		a_nDataLen,
		&m_ovrlpWrite.ovrlp,
		&__private::common::io::async::CommonIoSerialAsyncPrivate<BaseIoDevice>::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT);

	return bRetByReadEx ? 0 : -1;
#else
#endif
}


template <typename BaseIoDevice>
int common::io::async::Base<BaseIoDevice>::readC(void* a_buffer, int a_nBufLen)const
{
#ifdef _WIN32
	BOOL bRetByReadEx;
	
	m_ovrlpRead.buffer = (char*)a_buffer;

	bRetByReadEx = ReadFileEx(
		reinterpret_cast<HANDLE>(BaseIoDevice::m_handle),
		a_buffer,
		a_nBufLen,
		&m_ovrlpRead.ovrlp,
		&__private::common::io::async::CommonIoSerialAsyncPrivate<BaseIoDevice>::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT );

	return bRetByReadEx?0:-1;
#else
#endif
}


template <typename BaseIoDevice>
int common::io::async::Base<BaseIoDevice>::ReadSync(void* a_buffer, int a_nBufLen)const
{
	return Base::readC(a_buffer,a_nBufLen);
}


template <typename BaseIoDevice>
int common::io::async::Base<BaseIoDevice>::WriteSync(void* a_buffer, int a_nBufLen)
{
	return Base::writeC(a_buffer, a_nBufLen);
}


template <typename BaseIoDevice>
void common::io::async::Base<BaseIoDevice>::SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	m_ovrlpRead.clbkData=a_clbkData;
	m_ovrlpWrite.clbkData = a_clbkData;
	m_ovrlpRead.fpRead = a_fpRead?a_fpRead:&__private::common::io::async::DefaultReadCallback;
	m_ovrlpWrite.fpWrite = a_fpWrite?a_fpWrite: &__private::common::io::async::DefaultWriteCallback;
}


template <typename BaseIoDevice>
int common::io::async::Base<BaseIoDevice>::WaitForReadComplation(int a_timeoutMs)
{
	if(m_ovrlpRead.ovrlp.hEvent){
		DWORD dwCompletion = WAIT_IO_COMPLETION;
		while(dwCompletion == WAIT_IO_COMPLETION){dwCompletion=WaitForSingleObjectEx(m_ovrlpRead.ovrlp.hEvent,a_timeoutMs,TRUE);}
		return (dwCompletion==WAIT_OBJECT_0) ? 0:-1;
	}
	return -1;
}


template <typename BaseIoDevice>
int common::io::async::Base<BaseIoDevice>::WaitForWriteComplation(int a_timeoutMs)
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

template <typename IoBase>
VOID WINAPI CommonIoSerialAsyncPrivate<IoBase>::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	typename ::common::io::async::Base<IoBase>::SOvrlpdRead* pReadStr = lblcontainer_of(a_lpOverlapped, typename ::common::io::async::Base<IoBase>::SOvrlpdRead, ovrlp);
	pReadStr->fpRead(pReadStr->clbkData, a_dwErrorCode, pReadStr->buffer, a_dwNumberOfBytesTransfered);
	::SetEvent(pReadStr->ovrlp.hEvent);
}

template <typename IoBase>
VOID WINAPI CommonIoSerialAsyncPrivate<IoBase>::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	typename ::common::io::async::Base<IoBase>::SOvrlpdWrite* pWriteStr = lblcontainer_of(a_lpOverlapped, typename ::common::io::async::Base<IoBase>::SOvrlpdWrite, ovrlp);
	pWriteStr->fpWrite(pWriteStr->clbkData,a_dwErrorCode,pWriteStr->buffer,a_dwNumberOfBytesTransfered);
	::SetEvent(pWriteStr->ovrlp.hEvent);
}

}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{


#endif  // #ifndef __common_io_serial_impl_asyncbase_hpp__
