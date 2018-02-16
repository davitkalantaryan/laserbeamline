
// common_serial_comport.hpp
// 2017 Jul 21

#ifndef __common_serial_comport_hpp__
#define __common_serial_comport_hpp__

#include "common_iodevice.hpp"

#ifdef WIN32
#include <windows.h>
typedef HANDLE	COM_HANDLE;
#define	INVALID_COM_HANDLE	 INVALID_HANDLE_VALUE 
#else
#endif

namespace common{ namespace serial{

class ComPort : public IODevice
{
public:
	ComPort();
	virtual ~ComPort();

	virtual bool	isOpenC(void)const;
	virtual int		writeC(const void* buffer, int bufferLen);
	virtual int		readC(void* buffer, int bufferLen, int timeoutMS)const;
	virtual int		readC(void* buffer, int bufferLen)const;

	int				Read2(void* buffer, int bufLen, int timeoutFirstMs, int timeoutBtwMs);
	int				OpenCom(const char* comPortName);
	int				SetReadTimeouts(int timeoutFirstMs, int timeoutBtwMs);

	int SetupCommState(const DCB* DcbPtr, const COMMTIMEOUTS* timeouts = NULL,
		int inQueue = 512, int outQueue = 512);
	int SetupCommState(
		DWORD BaudSet, BYTE Parity, BYTE ByteSize,
		BYTE StopBits, DWORD fInX,
		DWORD fOutX, DWORD fOutxDsrFlow,
		DWORD fOutxCtsFlow,
		DWORD fDtrControl,
		DWORD fRtsControl,
		DWORD ReadIntervalTimeout = 10,
		int inQueue = 512, int outQueue = 512);
	int GetCommStates(DCB* DcbPtr, COMMTIMEOUTS* timeouts);

	operator COM_HANDLE&();

protected:
	virtual void	closeHard(void);
	virtual common::IODevice* Clone()const;

protected:
	COM_HANDLE		m_handle;
};

}}


#endif  // #ifndef __common_serial_comport_hpp__
