
// common_servertcp.hpp
// 2017 Jul 06

#ifndef __common_servertcp_hpp__
#define __common_servertcp_hpp__

#include "common_sockettcp.hpp"

struct sockaddr_in;

namespace common{

class ServerTCP : protected SocketBase
{
public:
	virtual ~ServerTCP(){}

	int	StartServer(
		int a_nPort,long int a_lnTimeout=1000, 
		bool bReuse = true, bool a_bLoopback=false);
	
	void StopServer(void);

private:
	virtual void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* remoteAddress)= 0;

protected:
	/*
	 * ServerAccept: server waiting for new connection
	 * Parameter:
	 *	a_ppClient:	accepted client socket
	 * Return:
	 *    < 0:	error
	 *   	0:	timeout
	 *	1:	ok
	 */
	int	ServerAccept(int& a_nClientSocket, int a_lnTimeout, sockaddr_in* remAddress);

	/*
	 * CreateServer: create server
	 * Parameter:
	 *	port: 	port number
	 * Return:
	 * 	0:	ok
	 *    < 0:	error
	 */
	int	 CreateServer(int a_nPort, bool bReuse,bool a_bLoopback);
	void RunServer(int a_lnTimeout);

private:
	// closed functions
	int		readC(void*, int, int) { return 0; }
	int		writeC(const void*, int) { return 0; }

protected:
	volatile int	m_nQUIT_FLAG;
};

}

#endif // #ifndef __common_servertcp_hpp__
