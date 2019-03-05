// 
// file:			common/io/proxy.hpp  
// created on:		2019 Feb 27  
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)  
// 

#ifndef __common_io_proxy_hpp__
#define __common_io_proxy_hpp__

#include <common/io/asyncbase.hpp>

#define NUBER_OF_DEVICES	2
#define PROXY_BUFFER_SIZE2	1024

namespace common{ namespace io{

class Proxy
{
public:
	Proxy();
	virtual ~Proxy();

	void SetDevices(async::Base** a_devs);
	void Start();
	void Stop();

protected:
	struct NewDev { 
		Proxy* aThis; async::Base* dev; char buff[PROXY_BUFFER_SIZE2];
		void *clbkData; async::ReadClbkType fpRead; async::WriteClbkType fpWrite;
	};
protected:
	NewDev*				m_devs2[NUBER_OF_DEVICES];
	volatile int		m_nError;
	volatile int		m_nRun;
	async::IoContext	m_ioContext;
};

}}  // namespace common{ namespace io{



#endif  // #ifndef __common_io_proxy_hpp__
