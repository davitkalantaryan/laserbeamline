/*****************************************************************************
 * File:    thread_cpp11.cpp
 * created: 2017 Apr 24
 *****************************************************************************
 * Author:	D.Kalantaryan, Tel:+49(0)33762/77552 kalantar
 * Email:	davit.kalantaryan@desy.de
 * Mail:	DESY, Platanenallee 6, 15738 Zeuthen
 *****************************************************************************
 * Description
 *   ...
 ****************************************************************************/

#include "thread_cpp11.hpp"

#ifndef __CPP11_DEFINED__

#ifdef WIN32
#include <process.h>
#endif

static STDN::SYSTHRRETTYPE ThreadStartupRoutine(void* a_thisThr);
static STDN::SYSTHRRETTYPE ThreadStartupRoutineVoid(void* a_thisThr);

namespace STDN{struct SThreadArgs{
    SThreadArgs(TypeClbKVoidPtr a_stFnc,void* a_thrArg):startRoutine(a_stFnc),thrArg(a_thrArg){}
    SThreadArgs(TypeClbKVoid2 a_stFnc):startRoutineVoid(a_stFnc),thrArg(NULL){}
    union{TypeClbKVoidPtr startRoutine;TypeClbKVoid2 startRoutineVoid;};void* thrArg;};}

STDN::thread::thread()
{
    InitAllMembersPrivate();
}


STDN::thread::thread(TypeClbKVoidPtr a_fpStartFunc,void* a_arg)
{
    InitAllMembersPrivate();
    ConstructThreadVoidPtr(a_fpStartFunc,a_arg);
}


STDN::thread::thread(TypeClbKVoid2 a_fpStartFunc)
{
    InitAllMembersPrivate();
    ConstructThreadVoid(a_fpStartFunc);
}


STDN::thread::~thread()
{
    if(0==(--m_nDublicates)){
        join();
    }
}


void STDN::thread::InitAllMembersPrivate()
{
    m_pThreadHandle = new thread_native_handle;
    if(!m_pThreadHandle){throw "Low memory!";}
    *m_pThreadHandle = (thread_native_handle)0;
    m_nDublicates = 1;
}


STDN::thread& STDN::thread::operator=(const STDN::thread& a_rS)
{
    m_nDublicates = (++a_rS.m_nDublicates);
    m_pThreadHandle = a_rS.m_pThreadHandle;

    return *this;
}


bool STDN::thread::joinable() const
{
    if(m_pThreadHandle && *m_pThreadHandle)
    {
        return true;
    }

    return false;
}


void STDN::thread::join()
{
    if(joinable())
    {
#ifdef WIN32
#error Not implemented yet
#else // #ifdef WIN32
        pthread_join(*m_pThreadHandle,NULL);
#endif // #ifdef WIN32

        *m_pThreadHandle = (thread_native_handle)0;
    }
}


void STDN::thread::ConstructThreadVoidPtr(TypeClbKVoidPtr a_fpStartFunc,void* a_arg)
{

    if((!m_pThreadHandle)||(*m_pThreadHandle)){return;}

    m_nDublicates = 1;
    SThreadArgs* pArgs = new SThreadArgs(a_fpStartFunc,a_arg);

#ifdef WIN32
#error should be implemented
    //unsigned ThreadID;
	m_pThreadHandle = (HANDLE)_beginthreadex( NULL, 0, (unsigned int(__stdcall*)(void*))&ThreadDv::ThreadFuncStatic, this, 0, &ThreadID );
#else  // #ifdef WIN32
    pthread_attr_t      tattr;
    pthread_attr_init(&tattr);
    //pthread_attr_setscope(&tattr, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
    pthread_create( m_pThreadHandle,  &tattr, &ThreadStartupRoutine,pArgs );
    pthread_attr_destroy(&tattr);

#endif // #ifdef WIN32

}


void STDN::thread::ConstructThreadVoid(TypeClbKVoid2 a_fpStartFunc)
{

    if((!m_pThreadHandle)||(*m_pThreadHandle)){return;}

    m_nDublicates = 1;
    SThreadArgs* pArgs = new SThreadArgs(a_fpStartFunc);

#ifdef WIN32
#error should be implemented
    //unsigned ThreadID;
    m_pThreadHandle = (HANDLE)_beginthreadex( NULL, 0, (unsigned int(__stdcall*)(void*))&ThreadDv::ThreadFuncStatic, this, 0, &ThreadID );
#else  // #ifdef WIN32
    pthread_attr_t      tattr;
    pthread_attr_init(&tattr);
    //pthread_attr_setscope(&tattr, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
    pthread_create( m_pThreadHandle,  &tattr, &ThreadStartupRoutineVoid,pArgs );
    pthread_attr_destroy(&tattr);

#endif // #ifdef WIN32

}


static STDN::SYSTHRRETTYPE ThreadStartupRoutine(void* a_thrArgs)
{
    STDN::SThreadArgs* pArgs = (STDN::SThreadArgs*)a_thrArgs;

    (*(pArgs->startRoutine))(pArgs->thrArg);

#ifdef WIN32
#error not implemented
#else // #ifdef WIN32
    ::pthread_exit(NULL);
#endif // #ifdef WIN32

    delete pArgs;
    return (STDN::SYSTHRRETTYPE)0;
}


static STDN::SYSTHRRETTYPE ThreadStartupRoutineVoid(void* a_thrArgs)
{
    STDN::SThreadArgs* pArgs = (STDN::SThreadArgs*)a_thrArgs;

    (*(pArgs->startRoutineVoid))();

#ifdef WIN32
#error not implemented
#else // #ifdef WIN32
    ::pthread_exit(NULL);
#endif // #ifdef WIN32

    delete pArgs;
    return (STDN::SYSTHRRETTYPE)0;
}


#endif  // #ifndef __CPP11_DEFINED__
