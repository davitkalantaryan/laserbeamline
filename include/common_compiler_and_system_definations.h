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

#ifdef __cplusplus
#define __BEGIN_C_DECL__ extern "C"{
#else   // #ifdef __cplusplus
#define __BEGIN_C_DECL__
#endif  // #ifdef __cplusplus

#ifdef _MSC_VER
#ifdef __cplusplus
#define __THISCALL__ __thiscall
#else   // #ifdef __cplusplus
#define __THISCALL__
#endif  // #ifdef __cplusplus
#ifdef _MSVC_LANG
#define __CPP_STANDARD__	_MSVC_LANG
#else
#define __CPP_STANDARD__	2003L
#endif   // #ifdef _MSVC_LANG
#elif defined(__CYGWIN__)
#define __THISCALL__
#elif defined(__GNUC__) || defined(__clang__)
#endif   // #ifdef _MSC_VER




#ifndef __THISCALL__
#ifdef _MSC_VER
#define __THISCALL__ __thiscall
#else
#define __THISCALL__
#endif
#endif

// Is C++11
#ifndef __NOT_USE_CPP11__
#ifndef __CPP11_DEFINED__
#if defined(_MSC_VER)
#if __cplusplus >= 199711L
#define __CPP11_DEFINED__
#endif // #if __cplusplus >= 199711L
#elif defined(__GNUC__) // #if defined(_MSC_VER)
#if __cplusplus > 199711L
#define __CPP11_DEFINED__
#endif // #if __cplusplus > 199711L
#else // #if defined(_MSC_VER)
#error this compiler is not supported
#endif // #if defined(_MSC_VER)
#endif  // #ifndef __CPP11_DEFINED__
#endif  // #ifndef __NOT_USE_CPP11__

// Is C++14
#ifndef __NOT_USE_CPP14__
#ifndef __CPP14_DEFINED__
#if defined(_MSC_VER)
#if __cplusplus >= 199711L
#define __CPP14_DEFINED__
#endif // #if __cplusplus >= 199711L
#elif defined(__GNUC__) // #if defined(_MSC_VER)
#if __cplusplus > 201103L
#define __CPP14_DEFINED__
#endif // #if __cplusplus > 199711L
#else // #if defined(_MSC_VER)
#error this compiler is not supported
#endif // #if defined(_MSC_VER)
#endif  // #ifndef __CPP14_DEFINED__
#endif  // #ifndef __NOT_USE_CPP14__

// This should be done after check
#ifdef __CPP11_DEFINED__
#define __OVERRIDE__	override
#define __FINAL__	final
#else
#define __OVERRIDE__
#define __FINAL__
#endif

#include <stdarg.h>

#ifdef __cplusplus
template <typename FncType>
static inline void* FUNCTION_POINTER_TO_VOID_POINTER(FncType _a_fnc_)
{
    FncType aFnc = _a_fnc_;
    void** ppFnc = (void**)&aFnc;
    void* pRet = *ppFnc;
    return pRet;
}
#endif // #ifdef __cplusplus

#ifdef __cplusplus
extern "C"{
#endif

//extern void* GetFuncPointer_common(int,...);

#ifdef __cplusplus
}
#endif

#endif // COMMON_DEFINATION_H
