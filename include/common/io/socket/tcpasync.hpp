// 
// file:			common/io/socket/async.hpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_socket_tcpasync_hpp__
#define __common_io_socket_tcpasync_hpp__

#include <common/io/socket/tcpbase.hpp>
#include <common/io/asyncbase.hpp>


namespace common{ namespace io{ namespace socket{

class TcpAsync : public async::Base< socket::TcpBase >
{
public:
	TcpAsync(void* clbkData, async::ReadClbkType fpRead, async::WriteClbkType fpWrite);
	virtual ~TcpAsync() {}

	virtual int		connectC(const char *svrName, int port, int connectionTimeoutMs = 1000, bool a_bAsync = false) override ;

};


}}}  // namespace common{ namespace io{ namespace serial{


#endif  // #ifndef __common_io_socket_tcpasync_hpp__
