// simpleThreads.cpp
// 20-Aug-2002, M. Winde, DESY-Zeuthen

#include "simpleThreads.h"
#include <signal.h>

extern "C" {
	void* glue_routine(void * ptr);
}

threadM::threadM(std::string name, int priority, int stackSize, void* aClass)
	: itsName(name), itsClass(aClass)
{
	#if OSIS==VXWORKS
		itsThreadID == ERROR;
		itsThreadID = taskSpawn(name.c_str(), priority, 0, stackSize,
			 (FUNCPTR)glue_routine, (int)this, 0,0,0,0,0, 0,0,0,0);
		//cerr << "<-- created tread(" << name  << ") itsThreadID = " << itsThreadID << endl;	
	#else // assume POSIX
		err = -1;
		err = pthread_create(&itsThreadID, NULL, glue_routine, this);
		allThreads.push_back(this);
	#endif
}

threadM::~threadM() {
	#if OSIS==VXWORKS
		taskDelete(itsThreadID);
	#else // assume POSIX
		int i;
                //std::vector<threadM*>::ite
                for(i = 0; i<allThreads.size();++i) {
                        if( allThreads[i]->itsThreadID  == pthread_self() ) {
                                allThreads.erase(allThreads.begin()+i);
			}
		}
		pthread_kill( itsThreadID, SIGKILL);
	#endif
}

void* glue_routine(void * ptr)	{
        ((threadM*)ptr)->waitForStart();
	return NULL;
}

void threadM::waitForStart() {
	//cerr << "thread::waitForStart() itsName = " << itsName << endl;
	for(;;) {
		state = pauseDemanded;
		wait();
		run();
	}
}

int threadM::start() {
	if(state == paused) {
		itsSema.post();
		return 0;
	}
	return 1;
}

int threadM::pause() {
	if(state == running) {
		state = pauseDemanded;
		return 0;
	}
	if(state == paused) return 1;
	return -1;
}

int threadM::wait() {
	if(state == pauseDemanded) {
		state = paused;
		//cerr << "thread::wait() itsName = " << itsName << endl;
		itsSema.wait();
		//cerr << "thread::start() itsName = " << itsName << endl;
		state = running;
		return 1;
	}
	return 0;
}

std::string threadM::getName() {
	return itsName;
}

std::string threadM::getNameSelf() {
	#if OSIS==VXWORKS
		return string( taskName(taskIdSelf()) );	
	#else // assume POSIX
		int i;
                for(i = 0;  i<allThreads.size();i++) {
                        if( allThreads[i]->itsThreadID  == pthread_self() )
                                return allThreads[i]->itsName;
		}
	#endif

                return "";
}

bool threadM::isOK() {
	#if OSIS==VXWORKS
		return itsThreadID != ERROR;
	#else // assume POSIX
		return err == 0;
	#endif
}
		
#if OSIS!=VXWORKS // assume POSIX
        //int threadM::nbOfThreads = 0;
        std::vector<threadM*> threadM::allThreads;
#endif
