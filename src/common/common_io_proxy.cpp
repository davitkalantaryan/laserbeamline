// 
// file:			common_io_proxy.cpp  
// created on:		2019 Feb 27  
//

#include <common/io/proxy.hpp>
#include <string.h>
#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

namespace __private{ namespace common{ namespace io{

class ProxyPrivate : public ::common::io::Proxy
{
public:
	static void ReadClbk(void* clbkData, int error, const char* data, int dataLen);
};

}}}  // namespace __private{ namespace common{ namespace io{


////////////////////////////////////////////////////////////////////////////////////////////

common::io::Proxy::Proxy()
{
	memset(m_devs2, 0, NUBER_OF_DEVICES*sizeof(NewDev));
#if 0
	for (int i(0); i < NUBER_OF_DEVICES; ++i) {
		m_devs[i] = a_devs[i];
		m_devs[i]->SetCallbacks(this, 0, 0);
	}
#endif
}


common::io::Proxy::~Proxy()
{
}


void common::io::Proxy::SetDevices(async::Base** a_devs)
{
	for(int i(0);i< NUBER_OF_DEVICES;++i){
		m_devs2[i]->aThis = this;
		m_devs2[i]->dev = a_devs[i];
		m_devs2[i]->dev->SetCallbacks(&m_devs2[i], &__private::common::io::ProxyPrivate::ReadClbk,nullptr);
	}
}


void common::io::Proxy::StartProxying()
{
	int i;
	async::CPtrSOvrlpdBase handleRead;
	async::CPtrSOvrlpdBase* pHandles = (async::CPtrSOvrlpdBase*)alloca(sizeof(async::CPtrSOvrlpdBase)*NUBER_OF_DEVICES);
	
	m_nError = 0;

	for (i=0; i < NUBER_OF_DEVICES; ++i) {
		pHandles[i]=m_devs2[i]->dev->ReadHandle();
		m_devs2[i]->dev->readC(m_devs2[i]->buff,PROXY_BUFFER_SIZE2);
	}

	while(!m_nError){
		handleRead=async::WaitForMultipleHandles(NUBER_OF_DEVICES,pHandles,INFINITE);
		if(!handleRead){break;}
	}

	for (i = 0; i < NUBER_OF_DEVICES; ++i) {
		pHandles[i] = m_devs2[i]->dev->ReadHandle();
		m_devs2[i]->dev->readC(m_devs2[i]->buff, PROXY_BUFFER_SIZE2);
	}

	// io-s shuld be stopped
}


///////////////////////////////////////////////////////////////////////
namespace __private{ namespace common{ namespace io{


void ProxyPrivate::ReadClbk(void* a_clbkData, int a_error, const char* a_data, int dataLen)
{
}


}}}  // namespace __private{ namespace common{ namespace io{
