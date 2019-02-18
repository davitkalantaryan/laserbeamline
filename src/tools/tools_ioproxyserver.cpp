
// tools_comportserver.cpp
// 2017 Jul 20

#include "tools_ioproxyserver.hpp"
#include "tools_ioproxy_common_header.h"
#include <stddef.h>

#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511
#define LOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->lock();}
#define UNLOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->unlock();}

extern int g_nDebugLevel;

static bool PrintProgramString(char* a_vcBufferProg, int a_nReceived);

tools::IoProxyServer::IoProxyServer()
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


void tools::IoProxyServer::SetIoDevice(common::IODevice* a_pIoDevice)
{
	m_pIoDevice = a_pIoDevice;
}


void tools::IoProxyServer::SetMutex(STDN::mutex* a_pMutex)
{
	m_pMutex = a_pMutex;
}


void tools::IoProxyServer::AddClient(common::SocketTCP& a_ClientSocket, const sockaddr_in* a_bufForRemAddress)
{

	if(!m_pIoDevice){return;}
	std::string aStrToPrintProg, aStrToPrintDev;
	int dwReadProg, dwReadDev;
	char vcBufferProg[PROG_BUFFER1 + 1], vcBufferDev[DEVICE_BUFFER1 + 1];
	bool bDebug;

	::common::socketN::GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("+++++++++++ Connection from host \"%s\"\n", vcBufferProg);
	m_pCurSocket = &a_ClientSocket;

	while (1) {
		dwReadProg = a_ClientSocket.readAny(vcBufferProg, PROG_BUFFER1);
		if (dwReadProg > 0) {

			if (g_nDebugLevel>0) {
				bDebug = PrintProgramString(vcBufferProg, dwReadProg);
			}
			LOCK_IF(m_pMutex);
			m_pIoDevice->writeC(vcBufferProg, dwReadProg);
			dwReadDev = m_pIoDevice->readC(vcBufferDev, DEVICE_BUFFER1);
			UNLOCK_IF(m_pMutex);
			if ((g_nDebugLevel>0) && bDebug) { printf("----- device  : "); }
			if (dwReadDev > 0) {
				a_ClientSocket.writeC(vcBufferDev, dwReadDev);
				aStrToPrintDev = std::string(vcBufferDev, dwReadDev);
				if ((g_nDebugLevel>0) && bDebug) { printf("%s (length=%d)\n", aStrToPrintDev.c_str(), dwReadDev); }
			}
			else if (dwReadDev == 0) { a_ClientSocket.writeC("", 1); }
			if ((g_nDebugLevel>0) && bDebug) { printf("\n"); }

		} // if (dwReadProg > 0) {
		else if (dwReadProg != _SOCKET_TIMEOUT_)
		{
			a_ClientSocket.closeC();
			break;
		}
	} // while (1) {

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
