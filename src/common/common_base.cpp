// 
// file:			common_base.cpp 
// created on:		2019 Feb 26 
// 

#if defined(_USE_COMMON_FUNCTIOALITY_ASCII) || defined(_USE_COMMON_FUNCTIOALITY_UNICODE)

#include <common/base.hpp>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#else
#endif


int common::MakeErrorReport(void* a_clbkData,TypeReport a_repFunc, ::common::String* a_pBuffer)
{
#ifdef _WIN32
	DWORD dwError = GetLastError();

	void* lpMsgBuf;
	Common_FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		0, // Default language
		reinterpret_cast<Char*>(&lpMsgBuf),
		0,
		nullptr
	);

	if(a_repFunc){(*a_repFunc)(a_clbkData, Common_Text("err_code=%d, text=%s"),static_cast<int>(dwError), static_cast<Char*>(lpMsgBuf));}
	if(a_pBuffer){*a_pBuffer= static_cast<Char*>(lpMsgBuf);}
	LocalFree(lpMsgBuf);
	return static_cast<int>(dwError);
#else   // #ifdef _WIN32
#endif  // #ifdef _WIN32
}

#endif  // #if defined(_USE_COMMON_FUNCTIOALITY_ASCII) || defined(_USE_COMMON_FUNCTIOALITY_UNICODE)
