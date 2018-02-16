
// tools_ioproxyserver.hpp
// 2017 Jul 20

#ifndef __tools_ioproxyserver_hpp__
#define __tools_ioproxyserver_hpp__

#include "common_servertcp.hpp"
#include "common_iodevice.hpp"
#include "mutex_cpp11.hpp"

namespace tools{


class IoProxyServer : protected common::ServerTCP
{
public:
	IoProxyServer();
	virtual ~IoProxyServer();

	void SetIoDevice(common::IODevice* serial);
	void SetMutex(STDN::mutex* mutex);
	void StartServerN(void);
	void StopServerN(void);

protected:
	virtual void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* bufForRemAddress);

protected:
	common::IODevice*			m_pIoDevice;
	STDN::mutex*				m_pMutex;
	common::SocketTCP*			m_pCurSocket;
};

}


#endif  // #ifndef __tools_ioproxyserver_hpp__
