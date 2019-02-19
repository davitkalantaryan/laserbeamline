
// tools_ioproxyserver.hpp
// 2017 Jul 20

#ifndef __tools_ioproxyserver_hpp__
#define __tools_ioproxyserver_hpp__

#include <common/common_servertcp.hpp>
#include <common/common_iodevice.hpp>
#include <common/tools/overlapped_io.hpp>
#include "mutex_cpp11.hpp"

#define PROG_BUFFER1	511

namespace tools{

class IoProxyServer;

typedef ::common::tools::SDataForReadAndTransfer TDataForOverlappeedReadCom, TDataForOverlappeedReadSock;


class IoProxyServer : protected common::ServerTCP
{
public:
	IoProxyServer();
	virtual ~IoProxyServer();

	int  SetIoDevice(common::IODevice* serial);
	void SetMutex(STDN::mutex* mutex);
	void StartServerN(void);
	void StopServerN(void);

	// new
	int SendToCom(const char* a_cpcBuffer, int a_nBufLen);
	int SendToClient(const char* a_cpcBuffer, int a_nBufLen);

protected:
	virtual void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* bufForRemAddress);

protected:
#ifdef _WIN32
	TDataForOverlappeedReadCom	m_overlappedCom;
#endif
	common::IODevice*			m_pIoDevice;
	STDN::mutex*				m_pMutex;
	common::SocketTCP*			m_pCurSocket;
	char						m_vcBuffrForIo[PROG_BUFFER1+1];
};

}


#endif  // #ifndef __tools_ioproxyserver_hpp__
