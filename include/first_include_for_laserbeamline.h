// 
// file:			first_include_for_laserbeamline.h
// created on:		2019 March 04 
// created by:		D. Kalantaryan
// 

#ifndef __pitz_laserbeamline_first_include_for_laserbeamline_h__
#define __pitz_laserbeamline_first_include_for_laserbeamline_h__

//#define _USE_COMMON_FUNCTIOALITY_UNICODE
#ifndef _USE_COMMON_FUNCTIOALITY_ASCII
#define _USE_COMMON_FUNCTIOALITY_ASCII
#endif

#ifdef _WIN64
#if defined(_USE_32BIT_TIME_T)
#undef _USE_32BIT_TIME_T
#endif  // #if !defined(_USE_32BIT_TIME_T)
#else
#if !defined(_USE_32BIT_TIME_T)
#define _USE_32BIT_TIME_T
#endif  // #if !defined(_USE_32BIT_TIME_T)
#endif

#if defined(__cplusplus) && !defined(NOMINMAX)  && !defined(NO_NOMINMAX)
#define NOMINMAX
#endif


#if defined(__cplusplus) && !defined(CINTERFACE)  && !defined(NO_CINTERFACE)
#define CINTERFACE
#endif

#ifdef _MSC_VER
#pragma warning (disable:4244)  // doocs_template_util.h(308): warning C4244: 'return': conversion from 'double' to 'float', possible loss of data
#pragma warning (disable:4267)  // d_fct_gen.h(591): warning C4267: 'return': conversion from 'size_t' to 'u_int', possible loss of data
#endif

#endif  // #ifndef __pitz_laserbeamline_first_include_for_laserbeamline_h__
