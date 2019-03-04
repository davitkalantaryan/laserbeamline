// 
// file:			first_include_for_laserbeamline.h
// created on:		2019 March 04 
// created by:		D. Kalantaryan
// 

#ifndef __pitz_laserbeamline_first_include_for_laserbeamline_h__
#define __pitz_laserbeamline_first_include_for_laserbeamline_h__


#if !defined(_USE_32BIT_TIME_T)
#define _USE_32BIT_TIME_T
#endif  // #if !defined(_USE_32BIT_TIME_T)

#if defined(__cplusplus) && !defined(NOMINMAX)  && !defined(NO_NOMINMAX)
#define NOMINMAX
#endif


#if defined(__cplusplus) && !defined(CINTERFACE)  && !defined(NO_CINTERFACE)
#define CINTERFACE
#endif

#ifdef _MSC_VER
#pragma warning (disable:4244)  // doocs_template_util.h(308): warning C4244: 'return': conversion from 'double' to 'float', possible loss of data
#endif

#endif  // #ifndef __pitz_laserbeamline_first_include_for_laserbeamline_h__
