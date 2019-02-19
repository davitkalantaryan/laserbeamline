
// tools_comportserver.cpp
// 2017 Jul 20

#include "tools_ioproxyserver.hpp"
#include "tools_ioproxy_common_header.h"
#include <stddef.h>

//#define DEVICE_BUFFER1	511
#define LOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->lock();}
#define UNLOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->unlock();}

extern int g_nDebugLevel;

static bool PrintProgramString(char* a_vcBufferProg, int a_nReceived);

tools::IoProxyServer::IoProxyServer()
#ifdef _WIN32
	:
		m_overlappedCom(NULL,m_vcBuffrForIo, PROG_BUFFER1,NULL,this)
#endif
{
	m_pIoDevice = NULL;
	m_pMutex = NULL;
}


tools::IoProxyServer::~IoProxyServer()
{
}


void tools::IoProxyServer::StartServerN(void)
{
	common::ServerTCP::StartServer(this,&IoProxyServer::AddClient,IO_PROXY_PORT_NAME);
}


void tools::IoProxyServer::StopServerN(void)
{
	common::ServerTCP::StopServer();
	if(m_pCurSocket){m_pCurSocket->closeC();}
}


int tools::IoProxyServer::SendToCom(const char* a_cpcBuffer, int a_nBufLen)
{
	if((!m_pIoDevice)||(!m_pIoDevice->isOpenC())){return -1;}
	return m_pIoDevice->writeC(a_cpcBuffer, a_nBufLen);
}


int tools::IoProxyServer::SendToClient(const char* a_cpcBuffer, int a_nBufLen)
{
	if ((!m_pCurSocket) || (!m_pCurSocket->isOpenC())) { return -1; }
	return m_pCurSocket->writeC(a_cpcBuffer, a_nBufLen);
}


int tools::IoProxyServer::SetIoDevice(common::IODevice* a_pIoDevice)
{
	m_pIoDevice = a_pIoDevice;
#if 0
#ifdef _WIN32
	BOOL bRetByReadEx;

	m_overlappedCom.handle = (HANDLE)a_pIoDevice->handle();
	m_overlappedCom.run = 1;
	bRetByReadEx = ReadFileEx(
		m_overlappedCom.handle,
		m_vcBuffrForIo,
		PROG_BUFFER1,
		&m_overlappedCom.ovrlp,
		&common::tools::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN );
	if (!bRetByReadEx) { m_overlappedCom.run=0;return -1; }
#else
#endif
#endif
	return 0;
}


void tools::IoProxyServer::SetMutex(STDN::mutex* a_pMutex)
{
	m_pMutex = a_pMutex;
}


void tools::IoProxyServer::AddClient(common::SocketTCP& a_ClientSocket, const sockaddr_in* a_bufForRemAddress)
{
	char vcBufferProg[PROG_BUFFER1 + 1];

	if(!m_pIoDevice){return;}
#ifdef _WIN32
	TDataForOverlappeedReadSock ovrReadSock((HANDLE)a_ClientSocket.handle(),vcBufferProg, PROG_BUFFER1,m_pIoDevice,this);
	BOOL bRetByReadEx;
#else
#endif

	::common::socketN::GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("+++++++++++ Connection from host \"%s\"\n", vcBufferProg);
	m_pCurSocket = &a_ClientSocket;

#ifdef _WIN32

	m_overlappedCom.handle = (HANDLE)m_pIoDevice->handle();
	m_overlappedCom.run = 1;
	bRetByReadEx = ReadFileEx(
		m_overlappedCom.handle,
		m_vcBuffrForIo,
		PROG_BUFFER1,
		&m_overlappedCom.ovrlp,
		&common::tools::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN);
	if (!bRetByReadEx) { m_overlappedCom.run = 0; goto returnPoint; }

	bRetByReadEx = ReadFileEx(
		ovrReadSock.handle,
		vcBufferProg,
		PROG_BUFFER1,
		&ovrReadSock.ovrlp,
		&common::tools::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN );
	if (!bRetByReadEx) { goto returnPoint; }

	while(ovrReadSock.run && m_overlappedCom.run){
		SleepEx(INFINITE, TRUE);
	}

#else   // #ifdef _WIN32
#endif  // #ifdef _WIN32
	
returnPoint:
	a_ClientSocket.closeC();
	m_pCurSocket = NULL;
	::common::socketN::GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("----------- Client from host \"%s\" disconnected\n", vcBufferProg);
}


static bool PrintProgramString(char* a_vcBufferProg, int a_nReceived)
{
#if 0
	return false;
#else
	static const char* scpcFilter = "getswst";
	std::string aStrToPrintProg;

	if (a_nReceived>2) { aStrToPrintProg = std::string(a_vcBufferProg, a_nReceived - 2); }
	else { aStrToPrintProg = "UnknownFowmat"; }

	//if (!strstr(aStrToPrintProg.c_str(), scpcFilter)) { return false; }

	printf("+++++ program : %s\n", aStrToPrintProg.c_str());

	return true;
#endif
}
