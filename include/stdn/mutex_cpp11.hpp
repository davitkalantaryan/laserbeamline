/*****************************************************************************
 * File:    mutex_cpp11.hpp
 * created: 2017 Apr 24
 *****************************************************************************
 * Author:	D.Kalantaryan, Tel:+49(0)33762/77552 kalantar
 * Email:	davit.kalantaryan@desy.de
 * Mail:	DESY, Platanenallee 6, 15738 Zeuthen
 *****************************************************************************
 * Description
 *   ...
 ****************************************************************************/
#ifndef MUTEX_CPP11_HPP
#define MUTEX_CPP11_HPP

#include <common_compiler_and_system_definations.h>

#if (__CPP_STANDARD__>=201101L) && !defined(IMPLEMENT_STD_MUTEX)
#include <mutex>
namespace STDN {
typedef std::mutex  mutex;
}
#else  // #if (__CPP_STANDARD__>=201101L) && !defined(IMPLEMENT_STD_MUTEX)

#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace STDN{

#ifdef _WIN32
typedef ::HANDLE  mutex_native_handle;
#else
typedef ::pthread_mutex_t mutex_native_handle;
#endif

class mutex
{
public:
    mutex();
    virtual ~mutex();

    void lock();
    void unlock();

private:
    mutex_native_handle m_mutexLock;
};

}

#endif // #if (__CPP_STANDARD__>=201101L) && !defined(IMPLEMENT_STD_MUTEX)

#endif // MUTEX_CPP11_HPP