// 
// file:			common_io_serial_async.cpp 
// created on:		2019 Feb 20 
// 


#include <common/io/serial/async.hpp>


using namespace common::io;


serial::Async::Async(void* a_clbkData, async::ReadClbkType a_fpRead, async::WriteClbkType a_fpWrite)
	:
	async::Base< serial::Base >(a_clbkData, a_fpRead,a_fpWrite)
{
	m_pOvrlRead = &m_ovrlpRead.ovrlp;
	m_pOvrlWrite = &m_ovrlpWrite.ovrlp;
}


int serial::Async::openC(const char* a_comPortName,bool)
{
	return Base::openC(a_comPortName, true);
}

