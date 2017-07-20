
// tools_comportserver.cpp
// 2017 Jul 20

#include "tools_comportserver.hpp"
#include <stddef.h>

#define PROG_BUFFER1	511
#define DEVICE_BUFFER1	511
#define LOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->lock();}
#define UNLOCK_IF(_mutex_ptr_)	if((_mutex_ptr_)){(_mutex_ptr_)->unlock();}

extern int g_nDebugLevel;

static bool PrintProgramString(char* a_vcBufferProg, int a_nReceived);

tools::ComServer::ComServer()
{
	m_pSerial = NULL;
	m_pMutex = NULL;
}


tools::ComServer::~ComServer()
{
}


void tools::ComServer::SetSerial(pitz::rpi::tools::Serial* a_pSerial)
{
	m_pSerial = a_pSerial;
}


void tools::ComServer::SetMutex(STDN::mutex* a_pMutex)
{
	m_pMutex = a_pMutex;
}


common::SocketTCP* tools::ComServer::GetCurrentSocket(void)
{
	return m_pCurSocket;
}


void tools::ComServer::AddClient(common::SocketTCP& a_ClientSocket, const sockaddr_in* a_bufForRemAddress)
{

	if(!m_pSerial){return;}
	pitz::rpi::tools::Serial& serialReal=*m_pSerial;
	std::string aStrToPrintProg, aStrToPrintDev;
	int dwReadProg, dwReadDev;
	char vcBufferProg[PROG_BUFFER1 + 1], vcBufferDev[DEVICE_BUFFER1 + 1];
	bool bDebug;

	GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("+++++++++++ Connection from host \"%s\"\n", vcBufferProg);
	m_pCurSocket = &a_ClientSocket;

	while (1) {
		dwReadProg = a_ClientSocket.readC(vcBufferProg, PROG_BUFFER1, -1);
		if (dwReadProg > 0) {

			if (g_nDebugLevel>0) {
				bDebug = PrintProgramString(vcBufferProg, dwReadProg);
			}
			LOCK_IF(m_pMutex);
			serialReal.Write(vcBufferProg, dwReadProg);
			dwReadDev = serialReal.Read4(vcBufferDev, DEVICE_BUFFER1, 50, 8);
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
	GetHostName(a_bufForRemAddress, vcBufferProg, PROG_BUFFER1);
	vcBufferProg[PROG_BUFFER1] = 0;
	printf("----------- Client from host \"%s\" disconnected\n", vcBufferProg);
}


static bool PrintProgramString(char* a_vcBufferProg, int a_nReceived)
{
#if 1
	return false;
#else
	static const char* scpcFilter = "getswst";
	std::string aStrToPrintProg;

	if (a_nReceived>2) { aStrToPrintProg = std::string(a_vcBufferProg, a_nReceived - 2); }
	else { aStrToPrintProg = "UnknownFowmat"; }

	if (!strstr(aStrToPrintProg.c_str(), scpcFilter)) { return false; }

	printf("+++++ program : %s\n", aStrToPrintProg.c_str());

	return true;
#endif
}
