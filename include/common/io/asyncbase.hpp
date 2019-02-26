// 
// file:			common/io/asyncbase.hpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_serial_asyncbase_hpp__
#define __common_io_serial_asyncbase_hpp__

#include <common/io/device.hpp>


namespace common{ namespace io{ namespace async{

typedef void(*ReadClbkType)(void* clbkData,int error,const char* data, int dataLen);
typedef void(*WriteClbkType)(void* clbkData,int error,const char* data, int dataLen);

template <typename BaseIoDevice>
class Base : public BaseIoDevice 
{
public:
	Base(void* clbkData, ReadClbkType fpRead, WriteClbkType fpWrite);
	virtual ~Base();

	virtual int			writeC(const void* buffer, int bufferLen) override;
	virtual int			readC(void* buffer, int bufferLen)const override;

	void				SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite);
	int					ReadSync(void* a_buffer, int a_nBufLen)const;
	int					WriteSync(void* a_buffer, int a_nBufLen);
	int					WaitForReadComplation(int timeoutMs);
	int					WaitForWriteComplation(int timeoutMs);

protected:
	virtual Device*		Clone()const;

protected:
#ifdef _WIN32
	struct SOvrlpdBase {
		mutable OVERLAPPED	ovrlp;
		mutable char*		buffer;
		void*				clbkData;
	};
	struct SOvrlpdRead : SOvrlpdBase { ReadClbkType		fpRead;};
	struct SOvrlpdWrite : SOvrlpdBase {WriteClbkType	fpWrite;};
#else
	struct SOvrlpd {
		char*				buffer;
	};
#endif

protected:
	SOvrlpdRead		m_ovrlpRead;
	SOvrlpdWrite	m_ovrlpWrite;
};


}}}  // namespace common{ namespace io{ namespace async{


#include "impl.asyncbase.hpp"

#endif  // #ifndef __common_io_serial_asyncbase_hpp__
