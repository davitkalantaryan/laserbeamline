/*
 *	File      : pitz_poluxclient_application.cpp
 *
 *	Created on: 22 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *
 */

#define     ANY_COMMAND_PROP_NAME  "ANY.COMMAND"

#include "pitz_poluxclient_application.hpp"
#include <stdio.h>
#include <stdarg.h>

int g_nDebugApplication = 1;
typedef void* TypeVoidPointer__;
static pitz::PoluxClient::Application* s_pApplication = NULL;


void* pitz::PoluxClient::GetFunctionPointer(int a_first,...)
{
    void* pReturn;
    va_list pArguments;
    va_start( pArguments, a_first );
    pReturn = va_arg(pArguments,TypeVoidPointer__);
    va_end(pArguments);
    return pReturn;
}

pitz::PoluxClient::Application* pitz::PoluxClient::GetCurentApplication()
{
    pitz::PoluxClient::Application* pApp =
            (pitz::PoluxClient::Application*)QCoreApplication::instance();
    return pApp ? pApp : s_pApplication;
}


int pitz::PoluxClient::SetNewTaskGlb(TypeCallback1 a_fpClb,void* a_owner,SCallArgsAll a_args)
{
    pitz::PoluxClient::Application* pApp = GetCurentApplication();

    if(!pApp){return -1;}
    pApp->SetNewTask(a_fpClb,a_owner,a_args);
    return 0;
}


/* ////////////////////////////////////////////////////////////////////////////// */
pitz::PoluxClient::Application::Application(int& a_argc, char* a_argv[])
    :
      QApplication(a_argc,a_argv),
      m_strFacility("PITZ.RPI"),
      m_strDevice("STEPPER_MOTOR.RPI02"),
      m_strLocation("STEPPER1")
{
    s_pApplication = this;
    connect(this,SIGNAL(TaskDoneSig1(pitz::PoluxClient::STaskDoneStruct)),
            this,SLOT(TaskDoneSlot1(pitz::PoluxClient::STaskDoneStruct)));
    m_doocsThread = std::thread(&pitz::PoluxClient::Application::DoocsThreadFunction,this);
}


pitz::PoluxClient::Application::~Application()
{
    disconnect(this,SIGNAL(TaskDoneSig1(pitz::PoluxClient::STaskDoneStruct)),
               this,SLOT(TaskDoneSlot1(pitz::PoluxClient::STaskDoneStruct)));
    m_nWork = 0;
    m_semaphore.post();
    m_doocsThread.join();
}


void pitz::PoluxClient::Application::DoocsThreadFunction()
{
    EqData  eqDataIn, eqDataOut;
    EqCall  eqCall;
    EqAdr   doocsAdr;

    int64_t llnReturn;
    STaskStruct aTaskStruct;
    STaskDoneStruct aTaskDoneStruct1;

    std::string propertyName;
    int nType = DATA_STRING;

    m_nWork = 1;

    while(m_nWork==1)
    {
        propertyName = m_strFacility + "/" + m_strDevice + "/" + m_strLocation +
                "/" ANY_COMMAND_PROP_NAME ;
        m_semaphore.wait();

        while(m_taskFifo.GetFirstData(&aTaskStruct))
        {
            // Do the job

            switch(aTaskStruct.args.type)
            {
            case TaskType::AnyCommand:
                eqDataIn.init();
                eqDataOut.init();
                doocsAdr.adr(propertyName.c_str());

                eqDataIn.set_type(DATA_STRING);
                eqDataIn.set(aTaskStruct.args.args.input.c_str());
                llnReturn = eqCall.set(&doocsAdr,&eqDataIn,&eqDataOut);
                //printf("!!!!!!!!!!!!!line:%d\n",__LINE__);
                break;
            case TaskType::CheckDoocsAdr:
                propertyName = m_strFacility + "/" + m_strDevice + "/" + m_strLocation +
                        "/" ANY_COMMAND_PROP_NAME ;
                eqDataIn.init();
                eqDataOut.init();
                doocsAdr.adr(propertyName.c_str());

                llnReturn = eqCall.get(&doocsAdr,&eqDataIn,&eqDataOut);
                nType = eqDataOut.type();
                break;
            default:
                llnReturn = -1;
                break;
            }

            __DEBUG__APP__(0,"prop=%s, str=%s, ret=%d, %d ~ %d",
                           propertyName.c_str(),
                           aTaskStruct.args.args.input.c_str(),
                           (int)llnReturn,nType,DATA_STRING);
            aTaskDoneStruct1.task = aTaskStruct;
            aTaskDoneStruct1.err = llnReturn;
            emit TaskDoneSig1(aTaskDoneStruct1);

        } // while(m_taskFifo.GetFirstData(&aTaskStruct))

    } // while(m_nWork==1)
}


//#define CallingArguments1  void* a_clb_data,const std::string& a_input
// struct task_struct{TypeCallback1 fpClbk;void* owner;void* clb_data;std::string input;};

void pitz::PoluxClient::Application::SetNewTask(TypeCallback1 a_fpClb,void* a_owner,SCallArgsAll a_args)
{
    STaskStruct aTaskStruct;

    aTaskStruct.fpClbk = a_fpClb;
    aTaskStruct.owner = a_owner;
    aTaskStruct.args = a_args;

    m_taskFifo.AddNewData(aTaskStruct);
    m_semaphore.post();
}

//#define CallingArguments1  void* a_clb_data,const std::string& a_input
//#define CallbackArguments1  CallingArguments1, int64_t a_err

void pitz::PoluxClient::Application::TaskDoneSlot1(pitz::PoluxClient::STaskDoneStruct a_td)
{
    (*a_td.task.fpClbk)(a_td.task.owner,a_td.task.args,a_td.err);
}
