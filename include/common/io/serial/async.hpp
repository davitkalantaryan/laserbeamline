// 
// file:			common/io/serial/async.hpp 
// created on:		2019 Feb 20 
// 

#ifndef __common_io_serial_async_hpp__
#define __common_io_serial_async_hpp__

#include <common/common_iodevice.hpp>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#ifndef INVALID_COM_HANDLE
typedef HANDLE	COM_HANDLE;
#define	INVALID_COM_HANDLE	 INVALID_HANDLE_VALUE 
#endif
#else
#endif


namespace common{ namespace io{ namespace serial{

class Async : public IODevice{
public:
	typedef void(*ReadClbkType)(void* clbkData,int error,const char* data, int dataLen);
	typedef void(*WriteClbkType)(void* clbkData,int error,const char* data, int dataLen);

public:
	Async(void* clbkData, ReadClbkType fpRead, WriteClbkType fpWrite);
	virtual ~Async();

	virtual bool		isOpenC(void)const;
	virtual int			writeC(const void* buffer, int bufferLen);
	virtual int			readC(void* buffer, int bufferLen)const;
	ptrdiff_t			handle() {return (ptrdiff_t)m_handle;}
	virtual int			setTimeout(int a_timeoutMS);

	int					readSync(void* a_buffer, int a_nBufLen)const;
	int					OpenCom(const char* a_comPortName);
	void				SetCallbacks(void* clbkData, ReadClbkType fpRead, WriteClbkType fpWrite);
	int					GetCommStates(DCB* DcbPtr, COMMTIMEOUTS* timeouts);
	int					SetupCommState(const DCB* DcbPtr,const COMMTIMEOUTS* timeouts = NULL,int inQueue = 512,int outQueue = 512);

protected:
	virtual void		closeHard(void);
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
	COM_HANDLE		m_handle;
	SOvrlpdRead		m_ovrlpRead;
	SOvrlpdWrite	m_ovrlpWrite;
};


}}}  // namespace common{ namespace io{ namespace serial{


#endif  // #ifndef __common_io_serial_async_hpp__
