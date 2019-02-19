//
// file:			common_tools_overlapped_io.cpp
// 

#include <common/tools/overlapped_io.hpp>

#ifndef lblcontainer_of
#define lblcontainer_of(_ptr,_type,_member) (_type*)(  ((char*)(_ptr)) + (size_t)( (char*)(&((_type *)0)->_member) )  )
#endif

::common::tools::SDataForReadAndTransfer::SDataForReadAndTransfer(HANDLE a_handle, char* a_buffer, size_t a_bufSize, IODevice* a_pToSend, void* a_pCallBack, ClbkType a_fpClbk)
	:
	handle(a_handle),
	pcBuffer(a_buffer),
	bufSize(a_bufSize),
	pToSend(a_pToSend),
	pCallBack(a_pCallBack),
	clbkFunc(a_fpClbk)
{
	this->ovrlp = { 0 };
	this->error = 0;
	this->run = 1;
	this->sendOk = 0;
}


namespace common{ namespace tools{

VOID WINAPI OVERLAPPED_READ_COMPLETION_ROUTINE_GEN(
	_In_    DWORD a_dwErrorCode,
	_In_    DWORD a_dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED a_lpOverlapped )
{
	HANDLE hOther;
	SDataForReadAndTransfer* pReadStr = lblcontainer_of(a_lpOverlapped, SDataForReadAndTransfer, ovrlp);
	
	if ((!a_dwErrorCode)||(a_dwErrorCode==ERROR_MORE_DATA)){
		// if ((a_dwErrorCode == 0) && (a_dwNumberOfBytesTransfered != 0)){
		BOOL bRetByReadEx;
		if(  (pReadStr->pToSend&&a_dwNumberOfBytesTransfered)&&(pReadStr->pToSend->writeC(pReadStr->pcBuffer,a_dwNumberOfBytesTransfered)>0)  ){ pReadStr->sendOk=1;}
		else {pReadStr->sendOk = 0;}
		(*pReadStr->clbkFunc)(pReadStr->pCallBack,pReadStr->pcBuffer,a_dwNumberOfBytesTransfered);
		bRetByReadEx = ReadFileEx(
			pReadStr->handle,
			pReadStr->pcBuffer,
			(DWORD)pReadStr->bufSize,
			&pReadStr->ovrlp,
			OVERLAPPED_READ_COMPLETION_ROUTINE_GEN );
		if (!bRetByReadEx) {goto errorPoint;}
	}
	else if(a_dwErrorCode){goto errorPoint;}

	return;
errorPoint:
	pReadStr->run = 0;
	if(pReadStr->pToSend){
		hOther = (HANDLE)pReadStr->pToSend->handle();
		if(hOther && (hOther!=INVALID_HANDLE_VALUE)){CancelIo(hOther);}
	}
}


}}  // namespace common{ namespace tools{
