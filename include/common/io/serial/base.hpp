// 
// file:			common/io/serial/base.hpp 
// created on:		2019 Feb 21 
// 

#ifndef __common_io_serial_base_hpp__
#define __common_io_serial_base_hpp__

#include <common/io/device.hpp>

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

typedef int SerialParityT;
namespace SerialParity {enum {
	None,
	Odd,
	Even,
	Mark,
	Space,
};}

typedef int SerialStopBitsT;
namespace SerialStopBits {enum {
	One,
	OnePointFive,
	Two,
};}

const wchar_t* StringFromSerialStopBits(SerialStopBitsT a_stopBits);
const wchar_t* StringFromSerialParity(SerialParityT a_parity);
const wchar_t* StringFromDtrControl(DWORD a_dtrControl);
const wchar_t* StringFromRtsControl(DWORD a_rtsControl);

typedef class Base : public Device{
public:
	Base(OVERLAPPED* pOvrlpdRead = nullptr, OVERLAPPED* pOvrlpdWrte = nullptr);
	virtual ~Base();

	bool				isOpenC(void)const override;
	ptrdiff_t			handle()const override {return (ptrdiff_t)m_handle;}
	virtual int			setTimeout(int a_timeoutMS) override;
	virtual int			writeC(const void* buffer, int bufferLen) override;
	virtual int			readC(void* a_buffer, int a_nBufLen)const override;
	
	virtual int			openC(const char* a_comPortName, bool a_bAsync=false);

	int					GetCommStates(DCB* DcbPtr, COMMTIMEOUTS* timeouts);
	int					SetupCommState(const DCB* DcbPtr,const COMMTIMEOUTS* timeouts = NULL,int inQueue = 512,int outQueue = 512);

protected:
	virtual void		closeHard(void) override;
	virtual Device*		Clone()const override;

protected:
	COM_HANDLE		m_handle;
	OVERLAPPED		*m_pOvrlRead, *m_pOvrlWrite;
}Sync;


int MakeStatisticForCom(Base* a_pSerial);


}}}  // namespace common{ namespace io{ namespace serial{


#include "impl.base.hpp"


#endif  // #ifndef __common_io_serial_base_hpp__

