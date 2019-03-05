// 
// file:			common/io/asyncbase.hpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_serial_asyncbase_hpp__
#define __common_io_serial_asyncbase_hpp__

#include <common/io/device.hpp>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#endif

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif

namespace common{ namespace io{ namespace async{

#ifdef _WIN32
typedef HANDLE	IoContext;
#else
typedef pthread_t IoContext;
#endif

typedef void(*ReadClbkType)(void* clbkData,int error,const char* data, int dataLen);
typedef void(*WriteClbkType)(void* clbkData,int error,const char* data, int dataLen);
class Base;

typedef struct SOvrlpdBase {
#ifdef _WIN32
	mutable OVERLAPPED	ovrlp;
	mutable char*		buffer;
	void*				clbkData;
	Base*				parent;
#else
	char*			buffer;
#endif
}const * CPtrSOvrlpdBase;

const SOvrlpdBase*	WaitForMultipleHandles(int number, CPtrSOvrlpdBase* handles, int timeoutMs);
void CancelIoForHandle(const SOvrlpdBase* handle);
IoContext GetIoContext();
void InteruptIoContext(IoContext context);

class Base
{
public:
	Base(void* clbkData, ReadClbkType fpRead, WriteClbkType fpWrite);
	virtual ~Base();

	virtual int				writeC(const void* buffer, int bufferLen) =0;
	virtual int				readC(void* buffer, int bufferLen)const =0;
	virtual ptrdiff_t		handle()const = 0;

	void					SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite);
	void					GetCallbacks(void** a_clbkDataPtr, ReadClbkType* a_fpReadPtr, WriteClbkType* a_fpWritePtr);
	int						WaitForReadComplation(int timeoutMs);
	int						WaitForWriteComplation(int timeoutMs);

	const SOvrlpdBase*		ReadHandle()const;
	const SOvrlpdBase*		WriteHandle()const;

protected:
	struct SOvrlpdRead : SOvrlpdBase { ReadClbkType		fpRead; };
	struct SOvrlpdWrite : SOvrlpdBase { WriteClbkType	fpWrite; };

protected:
	SOvrlpdRead		m_ovrlpRead;
	SOvrlpdWrite	m_ovrlpWrite;
};

template <typename BaseIoDevice>
class Dev : public BaseIoDevice, public Base
{
public:
	Dev(void* clbkData, ReadClbkType fpRead, WriteClbkType fpWrite);
	virtual ~Dev() {}

	virtual int			writeC(const void* buffer, int bufferLen) override;
	virtual int			readC(void* buffer, int bufferLen)const override;
	virtual ptrdiff_t	handle()const;

	int					ReadSync(void* a_buffer, int a_nBufLen)const;
	int					WriteSync(void* a_buffer, int a_nBufLen);

protected:
	virtual io::Device*	Clone()const;
};


}}}  // namespace common{ namespace io{ namespace async{


#include "impl.asyncbase.hpp"

#endif  // #ifndef __common_io_serial_asyncbase_hpp__
