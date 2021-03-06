
// tools_comportserver.cpp
// 2017 Jul 20

#include "tools_ioproxyserver.hpp"
#include "tools_ioproxy_common_header.h"
#include <stddef.h>

//#define DEVICE_BUFFER1	511
#define LOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->lock();}
#define UNLOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->unlock();}

extern int g_nDebugLevel;

static void FromSock(void*, const char*, int);
static void FromCom(void*, const char* a_Buffer, int a_size);


tools::IoProxyServer::IoProxyServer()
#ifdef _WIN32
	:
		m_overlappedCom(NULL,m_vcBuffrForIo, PROG_BUFFER1,NULL,this, FromCom)
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


tools::TDataForOverlappeedReadSock ovrReadSock(NULL, NULL, 0, NULL, NULL, NULL);

void tools::IoProxyServer::AddClient(common::SocketTCP& a_ClientSocket, const sockaddr_in* a_bufForRemAddress)
{
	char vcBufferProg[PROG_BUFFER1 + 1];

	if(!m_pIoDevice){return;}
#ifdef _WIN32
	//TDataForOverlappeedReadSock ovrReadSock((HANDLE)a_ClientSocket.handle(),vcBufferProg, PROG_BUFFER1,m_pIoDevice,NULL, FromSock);
	BOOL bRetByReadEx;

	// tmp
	ovrReadSock.handle = (HANDLE)a_ClientSocket.handle();
	ovrReadSock.pcBuffer = vcBufferProg;
	ovrReadSock.bufSize = PROG_BUFFER1;
	ovrReadSock.pToSend = m_pIoDevice;
	ovrReadSock.pCallBack = &ovrReadSock;
	ovrReadSock.clbkFunc = &FromSock;
	// end tmp

	ovrReadSock.pToSend = m_pIoDevice;
	ovrReadSock.pCallBack = &ovrReadSock;
	m_overlappedCom.pToSend = &a_ClientSocket;
#else
#endif

	::common::socketN::GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("+++++++++++ Connection from host \"%s\"\n", vcBufferProg);
	m_pCurSocket = &a_ClientSocket;

#ifdef _WIN32

#if 1
	m_overlappedCom.handle = (HANDLE)m_pIoDevice->handle();
	m_overlappedCom.run = 1;
	bRetByReadEx = ReadFileEx(
		m_overlappedCom.handle,
		m_vcBuffrForIo,
		5,
		&m_overlappedCom.ovrlp,
		&common::tools::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN);
	if (!bRetByReadEx) { m_overlappedCom.run = 0; goto returnPoint; }
#endif

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


static void FromSock(void* a_pClbk, const char* a_Buffer, int a_size)
{
	tools::TDataForOverlappeedReadSock* pClbk = (tools::TDataForOverlappeedReadSock*)a_pClbk;
	// new
	if (a_size>0) {
		printf("request(size:%d): ", a_size);
		fwrite(a_Buffer, 1, a_size, stdout);
	}
}


static void FromCom(void*, const char* a_Buffer, int a_size)
{
	printf("reply(size:%d): ", a_size);
	if (a_size>0) {
		//printf("reply(size:%d): ",a_size);
		fwrite(a_Buffer, 1, a_size, stdout);
	}
	else { printf("\n"); }
}
