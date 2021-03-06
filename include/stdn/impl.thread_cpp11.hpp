/*****************************************************************************
 * File:    thread_cpp11.tos
 * created: 2017 Apr 24
 *****************************************************************************
 * Author:	D.Kalantaryan, Tel:+49(0)33762/77552 kalantar
 * Email:	davit.kalantaryan@desy.de
 * Mail:	DESY, Platanenallee 6, 15738 Zeuthen
 *****************************************************************************
 * Description
 *   ...
 ****************************************************************************/
#ifndef IMPL_THREAD_CPP11_IMPL_HPP
#define IMPL_THREAD_CPP11_IMPL_HPP

#if (__CPP_STANDARD__<201101L) || defined(IMPLEMENT_STD_THREAD)

#ifndef STDN_THREAD_CPP11_HPP
#error this file should be included from thread_cpp11.hpp
#include "thread_cpp11.hpp"
#endif  // #ifndef THREAD_CPP11_HPP

template<typename TClass,typename TArg>
struct SArgs1{
    SArgs1(void (TClass::*a_startRoutine)(TArg a_aarg),
           TClass* a_owner,const TArg& a_arg);
    void (TClass::*startRoutine)(TArg a_arg);TClass* owner;TArg arg;
};


template<typename TClass,typename TArg>
static void ThreadFunctionWithArg(void* a_arg);


template <typename TClass>
STDN::thread::thread(void (TClass::*a_fpClbK)(),TClass* a_owner)
{
    InitAllMembersPrivate();
    ConstructThreadVoidPtr((TypeClbKVoidPtr)GetFuncPointer_common(0,a_fpClbK),(void*)a_owner);
}


template<typename TClass,typename TArg>
STDN::thread::thread(void (TClass::*a_fpClbK)(TArg aa_arg),TClass* a_owner,TArg a_arg)
{
    SArgs1<TClass,TArg>* pArgs = new SArgs1<TClass,TArg>(a_fpClbK,a_owner,a_arg);
    InitAllMembersPrivate();
    ConstructThreadVoidPtr(ThreadFunctionWithArg<TClass,TArg>,(void*)pArgs);
}

/*///////////////////////////////////////////////////////////////////////////*/
template<typename TClass,typename TArg>
SArgs1<TClass,TArg>::SArgs1(
        void (TClass::*a_startRoutine)(TArg aa_arg),
        TClass* a_owner,const TArg& a_arg)
    :
      startRoutine(a_startRoutine),
      owner(a_owner),
      arg(a_arg)
{
}


/*/////////////////////////////////////////////////////*/
template<typename TClass,typename TArg>
static void ThreadFunctionWithArg(void* a_arg)
{
    SArgs1<TClass,TArg>* pArgs = (SArgs1<TClass,TArg>*)a_arg;
    (pArgs->owner->*pArgs->startRoutine)(pArgs->arg);
    delete pArgs;
}


#endif //#ifndef __CPP11_DEFINED__

#endif // #ifndef IMPL_THREAD_CPP11_IMPL_HPP
