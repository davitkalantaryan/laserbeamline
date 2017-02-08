// simpleThreads.h
// 20-Aug-2002, M. Winde, DESY-Zeuthen

#ifndef __INCsimpleThreadsh
#define __INCsimpleThreadsh

#include "OSis.h"
#if OSIS==WINDOWS
	#include <windows.h>
	#include <process.h>
#elif OSIS==VXWORKS
	#include <taskLib.h>
#else // assume POSIX
	#include <pthread.h>
#endif

#include "sema.h"
#include <string>
#include <vector>

class threadM {

public:
	/// create the thread, pause it until start() will be called
	/** \param name can be used in Run() as debugging aid output
		\param priority thread prioryty (OS dependent)
		\param stackSize thread's stack size (bytes)
		\param aClass pointer to a class, can be used inside Run()
	*/
        threadM(std::string name, int priority, int stackSize, void* aClass);
	
	/// destruct the thread
        virtual ~threadM();
	
	/// (re-)start the thread
	/** called from any other task: Start the thread, if it is in paused-state; do nothing
		if it is in running-state.
		\return 0 thread state changed from paused to running
				1 thread state was running
		*/
	int start();
	
	/// method called after first start()
	virtual int run() {return -1;}
	
	/// pause the thread
	/** called from any other task: Send a signal to the thread, that it should wait until
		the next start().
		\return 0 thread state changed from running to pause-demanded
				1 thread state was paused
			   -1 thread state was pauseDemanded
	*/
	int pause();
	
	/// wait for start()
	/** called by the thread itself: If a pause-signal was sent to the thread, wait until
		an other start(). Else simply continue. 
		\return 0: no pause
				>0: there was a pause (? effective thread wait time [ms] ?)
	*/
	int wait();
	
	/// endless loop that waits for start(), then calls run()
	/** used internally only */
	void waitForStart();

        std::string getName();
	
        static std::string getNameSelf();
	
	bool isOK();
	
protected:
        const std::string itsName;
	void* itsClass;

private:
	
	sema itsSema;
	enum stateEnum {running, paused, pauseDemanded} state;
	
	#if OSIS==VXWORKS
		int itsThreadID;
	#else
		pthread_t itsThreadID;
		int err;
                static std::vector<threadM*> allThreads;
	#endif
	
};

#endif
