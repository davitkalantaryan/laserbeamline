
// common/io/socket/tcpbase.hpp
// 2017 Jul 06

#ifndef __common_io_socket_tcpbase_hpp__
#define __common_io_socket_tcpbase_hpp__

#include <common/io/socket/base.hpp>


namespace common{ namespace io{ namespace socket{

typedef class TcpBase : public Base
{
public:
	TcpBase() {}
	virtual ~TcpBase() {}

	virtual int		connectC(const char *svrName, int port, int connectionTimeoutMs = 1000, bool a_bAsync=false);
	virtual int		readC(void* buffer, int bufferLen)const;
	virtual int		readAny(void* buffer, int bufferLen)const;
	virtual int		writeC(const void* buffer, int bufferLen);

protected:
	virtual Device* Clone()const;
}TcpSync;

}}}  // namespace common{ namespace io{ namespace socket{


#endif  // #ifndef __common_io_socket_tcpbase_hpp__
