// 
// file:			common/base.hpp 
// created on:		2019 Feb 26 
// 

#ifndef __common_base_hpp__
#define __common_base_hpp__


#if defined(_USE_COMMON_FUNCTIOALITY_ASCII) || defined(_USE_COMMON_FUNCTIOALITY_UNICODE)

#include <stdarg.h>
#include <string>

namespace common {

#ifdef _USE_COMMON_FUNCTIOALITY_ASCII
typedef char Char;
typedef ::std::string	String;
#define Common_FormatMessage	::FormatMessageA
#define Common_Text(_text)		_text
#elif defined(_USE_COMMON_FUNCTIOALITY_UNICODE)
typedef wchar_t Char;
typedef ::std::wstring	String;
#define Common_FormatMessage	::FormatMessageW
#define Common_Text(_text)		L##_text
#endif

typedef int(*TypeReport)(void* a_pClbkData, const ::common::Char* frmt,...);
int MakeErrorReport(void* clbkData, TypeReport repFunc,::common::String* a_pBuffer);

}  // namespace common {
#endif  // #if defined(_USE_COMMON_FUNCTIOALITY_ASCII) || defined(_USE_COMMON_FUNCTIOALITY_UNICODE)

#endif  // #ifndef __common_base_hpp__
