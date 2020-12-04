
// common_servertcp.hpp
// 2017 Jul 06

#ifndef COMMON_IO_SOCKET_SERVERTCP_IMPL_HPP
#define COMMON_IO_SOCKET_SERVERTCP_IMPL_HPP

#ifndef COMMON_IO_SOCKET_SERVERTCP_HPP
//#error Do not include this file directly
#include "servertcp.hpp"
#endif


template <typename OwnerType, typename SocketType>
int	::common::io::socket::ServerTCP::StartServer(
	OwnerType* owner,
	void(OwnerType::* fpAddClient)(SocketType& clientSock, const sockaddr_in* remoteAddr),
	int a_nPort, bool bReuse = true, bool a_bLoopback = false, int a_lnTimeout = 1000, int* retCode = NULL);



namespace common{ namespace io{ namespace socket{

template <typename SocketType>
class AcptType {
	typedef void(__THISCALL__* TypeAccept)(void* owner, SocketType& clientSock, const sockaddr_in* remoteAddr);
};

class ServerTCP : public ::common::io::socket::Base
{
public:
	ServerTCP();
	virtual ~ServerTCP();

	template <typename OwnerType, typename SocketType>
	int	StartServer(
		OwnerType* owner,
		void(OwnerType::*fpAddClient)(SocketType& clientSock,const sockaddr_in*remoteAddr),
                int a_nPort, bool bReuse = true, bool a_bLoopback = false, int a_lnTimeout = 1000,int* retCode=NULL);

	template <typename SocketType>
	int	StartServerS(
		AcptType<SocketType>::TypeAccept fpAddClient,void* owner,
                int a_nPort, bool bReuse = true, bool a_bLoopback = false, int a_lnTimeout = 1000, int* retCode=NULL);
	
	void StopServer(void);

        int  InitServer(int a_nPort, bool bReuse = true, bool a_bLoopback = false);
        int  WaitForConnection(int timeoutMs, sockaddr_in* a_pRemoteAddr); // >0 is socket, 0 means timeout, -1 means othe error

private:
	//virtual void AddClient(common::SocketTCP& clientSocket, const sockaddr_in* remoteAddress)= 0;

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
	template <typename SocketType>
	void RunServer(int a_lnTimeout, AcptType<SocketType>::TypeAccept addClientFnc, void* owner);

private:
	// closed functions
	int		readC(void*, int)const { return 0; }
	int		writeC(const void*, int) { return 0; }

protected:
	volatile int	m_nWorkStatus;
	int				m_nServerThreadId;
#ifndef _WIN32
    pthread_t       m_serverThread;
#endif
};

}}}  // namespace common{ namespace io{ namespace socket{

#include "servertcp.impl.hpp"



#endif // #ifndef COMMON_IO_SOCKET_SERVERTCP_IMPL_HPP
