#pragma once
// base.hpp

// 
// file:			common/io/serial/base.hpp 
// created on:		2019 Feb 21 
// 

#ifndef __common_io_serial_base_hpp__
#define __common_io_serial_base_hpp__

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

typedef class Base : public IODevice{
public:
	Base(OVERLAPPED* pOvrlpdRead = nullptr, OVERLAPPED* pOvrlpdWrte = nullptr);
	virtual ~Base();

	bool				isOpenC(void)const override;
	ptrdiff_t			handle() override  {return (ptrdiff_t)m_handle;}
	virtual int			setTimeout(int a_timeoutMS) override;
	virtual int			writeC(const void* buffer, int bufferLen) override;
	virtual int			readC(void* a_buffer, int a_nBufLen)const override;
	
	virtual int			openC(const char* a_comPortName, bool a_bIsOverlabbed=false);

	int					GetCommStates(DCB* DcbPtr, COMMTIMEOUTS* timeouts);
	int					SetupCommState(const DCB* DcbPtr,const COMMTIMEOUTS* timeouts = NULL,int inQueue = 512,int outQueue = 512);

protected:
	virtual void		closeHard(void) override;
	virtual IODevice*	Clone()const override;

protected:
	COM_HANDLE		m_handle;
	OVERLAPPED		*m_pOvrlRead, *m_pOvrlWrite;
}Sync;


}}}  // namespace common{ namespace io{ namespace serial{


#endif  // #ifndef __common_io_serial_base_hpp__

