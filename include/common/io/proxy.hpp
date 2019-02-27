// 
// file:			common/io/proxy.hpp  
// created on:		2019 Feb 27  
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)  
// 

#ifndef __common_io_proxy_hpp__
#define __common_io_proxy_hpp__

#include <common/io/asyncbase.hpp>

namespace common{ namespace io{

class Proxy
{
public:
	Proxy();
	virtual ~Proxy();

protected:
	async::Base		*m_pDev1, *m_pDev2;
};

}}  // namespace common{ namespace io{



#endif  // #ifndef __common_io_proxy_hpp__
