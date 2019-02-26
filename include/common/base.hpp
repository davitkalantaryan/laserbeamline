// 
// file:			common/base.hpp 
// created on:		2019 Feb 26 
// 

#ifndef __common_base_hpp__
#define __common_base_hpp__

#include <stdarg.h>

#ifdef _USE_COMMON_FUNCTIOALITY_
namespace common {

typedef int(*TypeReportW)(void* a_pClbkData, const wchar_t* frmt,...);
int MakeErrorReport(void* clbkData, TypeReportW repFunc);

}  // namespace common {
#endif

#endif  // #ifndef __common_base_hpp__
