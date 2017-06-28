/*
 *	File      : pitz_poluxclient_application.hpp
 *
 *	Created on: 22 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */
#ifndef PITZ_POLUXCLIENT_APPLICATION_HPP
#define PITZ_POLUXCLIENT_APPLICATION_HPP

#include <QApplication>
#include <eq_client.h>
#include <string>
#include <thread>
#include <unnamedsemaphorelite.hpp>
#include <pitz_tool_fifo.hpp>
#include <string.h>

#ifndef __THISCALL__
#ifdef __MSC_VER
#define __THISCALL__ __thiscall
#else  // #ifdef __MSC_VER
#define __THISCALL__
#endif  // #ifdef __MSC_VER
#endif  // #ifndef __THISCALL__

extern int g_nDebugApplication;

#define __ONLY__FILE__(__full_path__) \
    ( strrchr((__full_path__),'/') ? \
        (strrchr((__full_path__),'/') + 1): \
        ( strrchr((__full_path__),'\\') ?  (strrchr((__full_path__),'\\')+1) : (__full_path__) )  )

#define __DEBUG__APP__(__log_level__,...) \
    do{ \
        if((__log_level__)<=g_nDebugApplication){\
            printf("fl:\"%s\", ln:%d, fnc:%s, ",__ONLY__FILE__(__FILE__),__LINE__,__FUNCTION__); \
            printf(__VA_ARGS__); printf("\n");}}while(0)

namespace pitz{ namespace PoluxClient{
enum class TaskType{AnyCommand,CheckDoocsAdr};
struct SCallArgs{void* clb_data;std::string input;};
struct SCallArgsAll{
    SCallArgsAll(TaskType a_tp=TaskType::CheckDoocsAdr,const std::string& a_in="",void* a_clb=NULL):type(a_tp)
    {args.input=a_in;args.clb_data=a_clb;}
                    TaskType type;SCallArgs args;};
//struct SCallbackArgs{SCallArgsAll argsAll;int64_t err;};
}}

typedef void (__THISCALL__ *TypeCallback1)(void* owner,pitz::PoluxClient::SCallArgsAll args, int64_t err);

namespace pitz{ namespace PoluxClient{

struct STaskStruct{TypeCallback1 fpClbk;void* owner;SCallArgsAll args;};
struct STaskDoneStruct{STaskStruct task; int64_t err;};

void* GetFunctionPointer(int,...);
int SetNewTaskGlb(TypeCallback1 fpClb,void* owner,SCallArgsAll args);
template <typename ClsType>
int SetNewTaskGlb( void(ClsType::*fpClb)(pitz::PoluxClient::SCallArgsAll,int64_t err),
                   ClsType* owner,SCallArgsAll a_args)
{
    return SetNewTaskGlb((TypeCallback1)GetFunctionPointer(0,fpClb),(void*)owner,a_args);
}

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int& argc, char* argv[]);
    ~Application();

    void SetNewTask(TypeCallback1 fpClb,void* owner,SCallArgsAll args);
    template <typename ClsType>
    void SetNewTask( void(ClsType::*fpClb)(SCallArgsAll),ClsType* owner,SCallArgsAll a_args)
    {
        SetNewTask((TypeCallback1)GetFunctionPointer(0,fpClb),(void*)owner,a_args);
    }

private:
    void DoocsThreadFunction();

private slots:
    void TaskDoneSlot1(pitz::PoluxClient::STaskDoneStruct tsk_done);

private:
signals:
    void TaskDoneSig1(pitz::PoluxClient::STaskDoneStruct tsk_done);

public:
    std::string                 m_strFacility;
    std::string                 m_strDevice;
    std::string                 m_strLocation;
    std::thread                 m_doocsThread;
    tools::UnnamedSemaphoreLite m_semaphore;
    volatile int                m_nWork;
    tools::FiFo<STaskStruct>    m_taskFifo;
};


pitz::PoluxClient::Application* GetCurentApplication();


}}

#endif // PITZ_POLUXCLIENT_APPLICATION_HPP
