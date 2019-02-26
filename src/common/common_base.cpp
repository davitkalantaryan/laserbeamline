// 
// file:			common_base.cpp 
// created on:		2019 Feb 26 
// 

#ifdef _USE_COMMON_FUNCTIOALITY_

#include <common/base.hpp>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#else
#endif


int common::MakeErrorReport(void* a_clbkData,TypeReportW a_repFunc)
{
#ifdef _WIN32
	DWORD dwError = GetLastError();

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		0, // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	(*a_repFunc)(a_clbkData, L"err_code=%d, text=%s", (int)dwError, (LPTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return (int)dwError;
#else   // #ifdef _WIN32
#endif  // #ifdef _WIN32
}

#endif  // #ifdef _USE_COMMON_FUNCTIOALITY_
