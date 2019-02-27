
// common_sockettcp.cpp
// 2017 Jul 06

#include <common/io/socket/tcpasync.hpp>

using namespace common::io;

socket::TcpAsync::TcpAsync(void* a_clbkData, async::ReadClbkType a_fpRead, async::WriteClbkType a_fpWrite)
	:
	async::Dev< socket::TcpBase >(a_clbkData, a_fpRead, a_fpWrite)
{
}


int socket::TcpAsync::connectC(const char *a_svrName, int a_port, int a_connectionTimeoutMs, bool)
{
	return async::Dev< socket::TcpBase >::connectC(a_svrName,a_port,a_connectionTimeoutMs,true);
}
