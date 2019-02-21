// 
// file:			common/io/serial/async.hpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_serial_async_hpp__
#define __common_io_serial_async_hpp__

#include <common/io/serial/base.hpp>


namespace common{ namespace io{ namespace serial{

class Async : public Base{
public:
	typedef void(*ReadClbkType)(void* clbkData,int error,const char* data, int dataLen);
	typedef void(*WriteClbkType)(void* clbkData,int error,const char* data, int dataLen);

public:
	Async(void* clbkData, ReadClbkType fpRead, WriteClbkType fpWrite);
	virtual ~Async();

	virtual int			writeC(const void* buffer, int bufferLen) override;
	virtual int			readC(void* buffer, int bufferLen)const override;
	virtual int			openC(const char* a_comPortName, bool = false) override;

	void				SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite);
	int					ReadSync(void* a_buffer, int a_nBufLen)const;
	int					WriteSync(void* a_buffer, int a_nBufLen);
	int					WaitForReadComplation(int timeoutMs);
	int					WaitForWriteComplation(int timeoutMs);

protected:
	virtual IODevice*	Clone()const;

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


}}}  // namespace common{ namespace io{ namespace serial{


#endif  // #ifndef __common_io_serial_async_hpp__
