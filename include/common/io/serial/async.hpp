// 
// file:			common/io/serial/async.hpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_serial_async_hpp__
#define __common_io_serial_async_hpp__

#include <common/io/serial/base.hpp>
#include <common/io/asyncbase.hpp>


namespace common{ namespace io{ namespace serial{

class Async : public async::Base< serial::Base >
{
public:
	Async(void* clbkData, async::ReadClbkType fpRead, async::WriteClbkType fpWrite);
	virtual ~Async() {}

	virtual int			openC(const char* a_comPortName, bool = false) override;

};


}}}  // namespace common{ namespace io{ namespace serial{


#endif  // #ifndef __common_io_serial_async_hpp__
