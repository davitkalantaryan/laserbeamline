// 
// 

#ifndef __common_tools_overlapped_io_hpp__
#define __common_tools_overlapped_io_hpp__

#if 0

#include <stdint.h>
#include <stddef.h>
#include <common/common_iodevice.hpp>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#endif

namespace common{ namespace tools{

typedef void(*ClbkType)(void*,const char*,int);

#ifdef _WIN32

VOID WINAPI OVERLAPPED_READ_COMPLETION_ROUTINE_GEN(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped );

struct SDataForReadAndTransfer 
{
	OVERLAPPED			ovrlp;
	HANDLE				handle;
	//char*const			pcBuffer;
	char*			pcBuffer;
	size_t				bufSize;
	IODevice*			pToSend;
	void*				pCallBack;
	ClbkType			clbkFunc;
	volatile uint32_t	run	: 1;
	uint32_t			sendOk : 1;
	int32_t				error : 20;

public:  // this
	SDataForReadAndTransfer(HANDLE a_handle, char* a_buffer, size_t bufSize, IODevice* a_pToSend, void* pCallBack, ClbkType a_fpClbk);
};
#else
#endif

}}


#endif


#endif  // #ifndef __common_tools_overlapped_io_hpp__
