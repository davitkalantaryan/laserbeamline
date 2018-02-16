
// common_socketbase.cpp
// 2017 Jul 06

#include "common_socketbase.hpp"

#include <string.h>
#include <time.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <memory.h>
#endif

#ifdef WIN32
#ifndef __SOCET_LIB_LOADED
#pragma comment(lib, "Ws2_32.lib")
#define __SOCET_LIB_LOADED
#endif //__SOCET_LIB_LOADED

#else
#endif


common::SocketBase::SocketBase()
	:
	m_socket(-1)
{
}


common::SocketBase::~SocketBase()
{
	closeC();
}


void common::SocketBase::closeC(void)
{
	IODevice::closeC();
	m_socket = -1;
}


bool common::SocketBase::isOpenC(void)const
{
	return (m_socket >= 0);
}


void common::SocketBase::closeHard(void)
{
	if (m_socket > 0)
#ifdef	WIN32
		closesocket(m_socket);
#else
		close(m_socket);
#endif
	m_socket = -1;
}


common::SocketBase& common::SocketBase::operator=(const common::SocketBase& a_aM)
{
	IODevice::operator=(a_aM);
	m_socket = a_aM.m_socket;
	return *this;
}


common::SocketBase::operator const int&()const
{
	return m_socket;
}


common::SocketBase::operator int&()
{
	return m_socket;
}


void common::SocketBase::SetSockDescriptor(int a_rawSocket)
{
	closeC();
	m_socket = a_rawSocket;
}


int common::SocketBase::DublicateSocket(int a_nProcID, void* a_pProtInfo)const
{
#ifdef WIN32
	WSAPROTOCOL_INFO* pProtInfo = (WSAPROTOCOL_INFO*)a_pProtInfo;
	return WSADuplicateSocket(m_socket, (DWORD)a_nProcID, pProtInfo);
#else
	throw "For berkley sockets not implemented!";
	return (a_nProcID == (int)((size_t)a_pProtInfo)) ? 1 : 0;
#endif
}


int common::SocketBase::Initialize()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		return 0;
	}

	/* Confirm that the WinSock DLL supports 2.2.		*/
	/* Note that if the DLL supports versions greater	*/
	/* than 2.2 in addition to 2.2, it will still return*/
	/* 2.2 in wVersion since that is the version we		*/
	/* requested.										*/

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return 0;
	}

#endif

	return 1;
}

/*
 * Cleanup socket library
 */
void common::SocketBase::Cleanup()
{
#ifdef WIN32
	WSACleanup();
#endif
}


int common::SocketBase::SleepN(long long int a_nanoSec)
{
#ifdef WIN32
	struct timeval tv;
	long int lnNs;
	int maxsd, fh1;
	fd_set rfds;

	fh1 = (int)socket(AF_INET, SOCK_STREAM, 0);

	//fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET((unsigned int)fh1, &rfds);
	maxsd = fh1 + 1;

	//struct timeval tv;
	lnNs = (a_nanoSec % 1000000000L);
	tv.tv_sec = (long)(a_nanoSec / 1000000000L);
	tv.tv_usec = (lnNs%1000)?(lnNs/1000)+1: (lnNs / 1000);

	return (long int)(1000*select(maxsd, &rfds, 0, 0, &tv));
#else
	struct timespec waitspec;
        waitspec.tv_sec = a_nanoSec / 1000000000;
        waitspec.tv_nsec = a_nanoSec % 1000000000;
	return (long)nanosleep(&waitspec, NULL);
#endif
}

#ifdef sockaddr_in_using_ok

#ifdef _MSC_VER
#if(_MSC_VER >= 1400)
//#define		_WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable : 4996)
#endif
#endif


const char* common::SocketBase::GetIPAddress(
	const sockaddr_in* a_addr)
{
	if (!a_addr){return "NULL";}
	return inet_ntoa(a_addr->sin_addr);
}


#ifdef WIN32
//#include <Ws2tcpip.h>
//#include <Wspiapi.h>
//#include <ws2def>
#ifndef NI_NUMERICSERV
#define NI_NUMERICSERV  0x08  /* Return numeric form of the service (port #) */
#endif
typedef int  (WSAAPI* getnameinfo_type) (const struct sockaddr*,
	socklen_t, char*, DWORD,
	char*, DWORD, int);
#endif


const char* common::SocketBase::GetHostName(
	const sockaddr_in* a_addr, 
	char* a_pcBuffer, int a_nBuffLen)
{
	if (!a_addr) { return "NULL"; }
#ifndef WIN32
	getnameinfo((struct sockaddr *)a_addr, sizeof(sockaddr), a_pcBuffer, a_nBuffLen, NULL, 0, NI_NUMERICSERV);
#else
	HMODULE h = GetModuleHandleA("ws2_32.dll");
	getnameinfo_type getnameinfo_ptr = (getnameinfo_type)GetProcAddress(h, "getnameinfo");
	if (getnameinfo_ptr) {
		(*getnameinfo_ptr)((sockaddr *)a_addr,sizeof(sockaddr),a_pcBuffer,a_nBuffLen,NULL,0,NI_NUMERICSERV);
	}else{return "NULL";}
#endif
	return a_pcBuffer;
}



int common::SocketBase::GetPort(const sockaddr_in* a_addr)
{
	if (!a_addr){return 0;}
	return (int)ntohs(a_addr->sin_port);
}

#endif  // #ifdef sockaddr_in_using_ok
