/*****************************************************************************
 * File:    common_compiler_and_system_definations.h
 * created: 2017 Apr 24
 *****************************************************************************
 * Author:	D.Kalantaryan, Tel:+49(0)33762/77552 kalantar
 * Email:	davit.kalantaryan@desy.de
 * Mail:	DESY, Platanenallee 6, 15738 Zeuthen
 *****************************************************************************
 * Description
 *   ...
 ****************************************************************************/
#ifndef COMMON_COMPILER_AND_SYSTEM_DEFINATION_H
#define COMMON_COMPILER_AND_SYSTEM_DEFINATION_H

#define __THISCALL__

#ifdef __cplusplus
#define __STATIC_CAST__(_Type,_value)	static_cast<_Type>(_value)
#define __REINTERPRET_CAST__(_Type,_value)	reinterpret_cast<_Type>(_value)
#define __CPP_STANDARD__	__cplusplus
#define __BEGIN_C_DECL__	extern "C"{
#else   // #ifdef __cplusplus
#define __STATIC_CAST__(_Type,_value)	( (_Type)(_value) )
#define __REINTERPRET_CAST__(_Type,_value)	( (_Type)(_value) )
#define __CPP_STANDARD__	2003L
#define __BEGIN_C_DECL__
#endif  // #ifdef __cplusplus

#ifdef _MSC_VER									// 1.
#define __DLL_PUBLIC__		__declspec(dllexport)
#define __DLL_PRIVATE__	
#define __DLL_IMPORT__		__declspec(dllimport)
#if defined(__cplusplus)	// should be also 
							// checked and others
#undef __THISCALL__
#define __THISCALL__ __thiscall
#endif
#ifdef _MSVC_LANG
#undef __CPP_STANDARD__
#define __CPP_STANDARD__	_MSVC_LANG
#endif   // #ifdef _MSVC_LANG
#elif defined(__CYGWIN__)							// 2. 
#define __DLL_PUBLIC__		__attribute__((dllexport))
#define __DLL_PRIVATE__	
#define __DLL_IMPORT__		__attribute__((dllimport))
#elif defined(__GNUC__) || defined(__clang__)		// 3. 
#define __DLL_PUBLIC__		__attribute__((visibility("default")))
#define __DLL_PRIVATE__		__attribute__((visibility("hidden")))
#define __DLL_IMPORT__	
#elif defined(__MINGW64__) || defined(__MINGW32__)	// 4. 
#define __DLL_PUBLIC__		__declspec(dllexport)
#define __DLL_PRIVATE__	
#define __DLL_IMPORT__		__declspec(dllimport)
#elif defined(__SUNPRO_CC)							// 5. 
#define __DLL_PUBLIC__
#define __DLL_PRIVATE__		__hidden
#define __DLL_IMPORT__	
#endif   // #ifdef _MSC_VER


// This should be done after check
#if (__CPP_STANDARD__>201101L)
#define __OVERRIDE__	override
#define __FINAL__	final
#else
#define __OVERRIDE__
#define __FINAL__
#endif

#endif // #ifndef COMMON_COMPILER_AND_SYSTEM_DEFINATION_H
