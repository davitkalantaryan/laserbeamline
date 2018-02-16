
// first_include_to_supress_doocs_warnings.h
// to include ->  #include "first_include_to_supress_doocs_warnings.h"
// created on 2018 Feb 16

#ifndef __first_include_to_supress_doocs_warnings_h__
#define __first_include_to_supress_doocs_warnings_h__


#ifdef _MSC_VER
#pragma warning(disable:4800)   // 'int': forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable:4244)   // 'argument': conversion from 'int' to 'float', possible loss of data
#endif  // #ifdef _MSC_VER


#endif  // #ifndef __first_include_to_supress_doocs_warnings_h__
