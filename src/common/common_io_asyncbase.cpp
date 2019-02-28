// 
// file:			common_io_serial_async.cpp 
// created on:		2019 Feb 20 
// 

#include <common/io/asyncbase.hpp>
#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif
#include <sys/timeb.h>

namespace __private{ namespace common{ namespace io{ namespace async{

static void DefaultReadCallback(void*, int, const char*, int) {}
static void DefaultWriteCallback(void*, int, const char*, int) {}

}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{


common::io::async::Base::Base(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	memset(&m_ovrlpRead, 0, sizeof(m_ovrlpRead));
	memset(&m_ovrlpWrite, 0, sizeof(m_ovrlpWrite));
	m_ovrlpRead.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovrlpWrite.ovrlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovrlpRead.parent = m_ovrlpWrite.parent = this;
	this->SetCallbacks(a_clbkData, a_fpRead, a_fpWrite);
}



common::io::async::Base::~Base()
{
	if(m_ovrlpWrite.ovrlp.hEvent){CloseHandle(m_ovrlpWrite.ovrlp.hEvent);}
	if(m_ovrlpRead.ovrlp.hEvent){CloseHandle(m_ovrlpRead.ovrlp.hEvent);}
}


void common::io::async::Base::SetCallbacks(void* a_clbkData, ReadClbkType a_fpRead, WriteClbkType a_fpWrite)
{
	m_ovrlpRead.clbkData=a_clbkData;
	m_ovrlpWrite.clbkData = a_clbkData;
	m_ovrlpRead.fpRead = a_fpRead?a_fpRead:&__private::common::io::async::DefaultReadCallback;
	m_ovrlpWrite.fpWrite = a_fpWrite?a_fpWrite: &__private::common::io::async::DefaultWriteCallback;
}


int common::io::async::Base::WaitForReadComplation(int a_timeoutMs)
{
	if(m_ovrlpRead.ovrlp.hEvent){
		DWORD dwCompletion = WAIT_IO_COMPLETION;
		while(dwCompletion == WAIT_IO_COMPLETION){dwCompletion=WaitForSingleObjectEx(m_ovrlpRead.ovrlp.hEvent,a_timeoutMs,TRUE);}
		return (dwCompletion==WAIT_OBJECT_0) ? 0:-1;
	}
	return -1;
}


int common::io::async::Base::WaitForWriteComplation(int a_timeoutMs)
{
	if (m_ovrlpWrite.ovrlp.hEvent) {
		DWORD dwCompletion = WAIT_IO_COMPLETION;
		while(dwCompletion == WAIT_IO_COMPLETION){dwCompletion=WaitForSingleObjectEx(m_ovrlpWrite.ovrlp.hEvent,a_timeoutMs,TRUE);}
		return (dwCompletion==WAIT_OBJECT_0) ? 0:-1;
	}
	return -1;
}


const common::io::async::SOvrlpdBase* common::io::async::Base::ReadHandle()const
{
	return &m_ovrlpRead;
}


const common::io::async::SOvrlpdBase* common::io::async::Base::WriteHandle()const
{
	return &m_ovrlpWrite;
}


void common::io::async::CancelIoForHandle(const SOvrlpdBase* a_handle)
{
#ifdef _WIN32
	CancelIo((HANDLE)a_handle->parent->handle());
#else
#endif
}


const common::io::async::SOvrlpdBase* common::io::async::WaitForMultipleHandles(int a_nNumber, CPtrSOvrlpdBase* a_pHandles, int a_timeoutMs)
{
	CPtrSOvrlpdBase handleToReturn = nullptr;
#ifdef _WIN32
	HANDLE*  pHandles = (HANDLE*)alloca(a_nNumber * sizeof(HANDLE));
	struct timeb timeBeg, timeCur;
	DWORD dwReturn;
	int nReturn;
	int nTimeDiff;

#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif

	for(int i(0);i<a_nNumber;++i){
		pHandles[i]=a_pHandles[i]->ovrlp.hEvent;
	}

	ftime(&timeBeg);

	dwReturn=WaitForMultipleObjectsEx(a_nNumber,pHandles,FALSE,a_timeoutMs,TRUE);
	nReturn=((int)dwReturn)-WAIT_OBJECT_0;

	if((nReturn<0)||(nReturn>=a_nNumber)){
		while (dwReturn == WAIT_IO_COMPLETION) {
			if(a_timeoutMs>=0){
				ftime(&timeCur);
				nTimeDiff=(int)((timeCur.time- timeBeg.millitm)/1000) + (int)timeCur.millitm -(int)timeBeg.millitm;
				a_timeoutMs -= nTimeDiff;
				if(a_timeoutMs<0){break;}
			}
			dwReturn = WaitForMultipleObjectsEx(a_nNumber, pHandles, FALSE, a_timeoutMs, TRUE);
			nReturn = ((int)dwReturn) - WAIT_OBJECT_0;
			if ((nReturn >= 0) || (nReturn < a_nNumber)){
				handleToReturn = a_pHandles[nReturn];
				break;
			}
		}
	}
	else {
		handleToReturn = a_pHandles[nReturn];
	}
#else   // #ifdef _WIN32
#endif  // #ifdef _WIN32

	return handleToReturn;
}


/////////////////////////////////////////////////////////////////////////////////////////

namespace __private{ namespace common{ namespace io{ namespace async{

VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_READ_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	::common::io::async::Base::SOvrlpdRead* pReadStr = lblcontainer_of(a_lpOverlapped, ::common::io::async::Base::SOvrlpdRead, ovrlp);
	pReadStr->fpRead(pReadStr->clbkData, a_dwErrorCode, pReadStr->buffer, a_dwNumberOfBytesTransfered);
	::SetEvent(pReadStr->ovrlp.hEvent);
}

VOID WINAPI CommonIoSerialAsyncPrivate::OVERLAPPED_WRITE_COMPLETION_ROUTINE_GEN_STAT(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	::common::io::async::Base::SOvrlpdWrite* pWriteStr = lblcontainer_of(a_lpOverlapped, ::common::io::async::Base::SOvrlpdWrite, ovrlp);
	pWriteStr->fpWrite(pWriteStr->clbkData,a_dwErrorCode,pWriteStr->buffer,a_dwNumberOfBytesTransfered);
	::SetEvent(pWriteStr->ovrlp.hEvent);
}

}}}}  // namespace __private{ namespace common{ namespace io{ namespace async{
