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


template <typename BaseIoDevice>
common::io::async::Dev<BaseIoDevice>::Dev(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
	:
	async::Base(a_clbkData,a_fpRead,a_fpWrite)
{
}


template <typename BaseIoDevice>
common::io::Device* common::io::async::Dev<BaseIoDevice>::Clone()const
{
	return new common::io::async::Dev<BaseIoDevice>(*this);
}


template <typename BaseIoDevice>
int common::io::async::Dev<BaseIoDevice>::writeC(const void* a_data, int a_nDataLen)
{
#ifdef _WIN32
	BOOL bRetByReadEx;

	m_ovrlpWrite.buffer = (char*)a_data;

	bRetByReadEx = WriteFileEx(
		reinterpret_cast<HANDLE>(BaseIoDevice::m_handle),
		a_data,
		a_nDataLen,
		&m_ovrlpWrite.ovrlp,
		&__private::common::io::async::CommonIoSerialAsyncPrivate::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT);

	return bRetByReadEx ? 0 : -1;
#else
#endif
}


template <typename BaseIoDevice>
int common::io::async::Dev<BaseIoDevice>::readC(void* a_buffer, int a_nBufLen)const
{
#ifdef _WIN32
	BOOL bRetByReadEx;
	
	m_ovrlpRead.buffer = (char*)a_buffer;

	bRetByReadEx = ReadFileEx(
		reinterpret_cast<HANDLE>(BaseIoDevice::m_handle),
		a_buffer,
		a_nBufLen,
		&m_ovrlpRead.ovrlp,
		&__private::common::io::async::CommonIoSerialAsyncPrivate::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT );

	return bRetByReadEx?0:-1;
#else
#endif
}


template <typename BaseIoDevice>
int common::io::async::Dev<BaseIoDevice>::ReadSync(void* a_buffer, int a_nBufLen)const
{
	return BaseIoDevice::readC(a_buffer,a_nBufLen);
}


template <typename BaseIoDevice>
int common::io::async::Dev<BaseIoDevice>::WriteSync(void* a_buffer, int a_nBufLen)
{
	return BaseIoDevice::writeC(a_buffer, a_nBufLen);
}

/////////////////////////////////////////////////////////
namespace __private{ namespace common{ namespace io{ namespace async{

class CommonIoSerialAsyncPrivate : public ::common::io::async::Base
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


}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{

#endif  // #ifndef __common_io_serial_impl_asyncbase_hpp__
