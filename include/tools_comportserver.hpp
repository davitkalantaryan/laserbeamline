
// tools_comportserver.hpp
// 2017 Jul 20

#ifndef __tools_comportserver_hpp__
#define __tools_comportserver_hpp__

#include "common_servertcp.hpp"
#include "pitz_rpi_tools_serial.hpp"
#include "mutex_cpp11.hpp"

namespace tools{


class ComServer : public common::ServerTCP
{
public:
	ComServer();
	virtual ~ComServer();

	void SetSerial(pitz::rpi::tools::Serial* serial);
	void SetMutex(STDN::mutex* mutex);
	common::SocketTCP* GetCurrentSocket(void);

protected:
	virtual void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* bufForRemAddress);

protected:
	pitz::rpi::tools::Serial*	m_pSerial;
	STDN::mutex*				m_pMutex;
	common::SocketTCP*			m_pCurSocket;
};

}


#endif  // #ifndef __tools_comportserver_hpp__
