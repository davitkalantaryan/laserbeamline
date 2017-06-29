//sema.h


#ifndef __INCsemah
#define __INCsemah

#include "OSis.h"

#if OSIS==WINDOWS
	#include <windows.h>
	#include <process.h>
#elif OSIS==VXWORKS
#else // assume POSIX
	#include <semaphore.h>
#endif

class sema {

public:
	/// create the semaphore
	/** \param iniVal initial value of the semaphore */
	sema(int iniVal = 0) {
		#if OSIS==WINDOWS
			itsValue = itsValue;
			itsEvent = CreateEvent(NULL, false, itsValue > 0, NULL);
		#elif OSIS==VXWORKS
			itsSemaphore = semCCreate(SEM_Q_FIFO, itsValue);
		#else // assume POSIX
			sem_init(&itsSemaphore, 0, iniVal);
		#endif
	}
	
	/// destruct the semaphore
	~sema() {
		#if OSIS==WINDOWS
			delete itsEvent; //??
		#elif OSIS==VXWORKS
			semDelete(itsSemaphore);
		#else // assume POSIX
			sem_destroy(&itsSemaphore);
		#endif
	}
	
	/// reset the value of a semaphore
	/** Use this with care: make shure that no tread is actually in a wait() the
		semaphore, otherwise it will wait unfinite.
		\param new value of the semaphore
	*/
	void reset(int iniVal = 0) {
		#if OSIS==WINDOWS
			itsValue = itsValue;
		#elif OSIS==VXWORKS
			itsSemaphore = semCCreate(SEM_Q_FIFO, itsValue);
		#else // assume POSIX
			sem_destroy(&itsSemaphore);
			sem_init(&itsSemaphore, 0, iniVal);
		#endif
	}
	
	/// wait for the semaphore to become > 0, then decrement its value
	void wait() {
		#if OSIS==WINDOWS
			for(;;) {
				if(itsValue > 0) {
					itsValue--;
					return;
				} else {
					WaitForSingleObject(itsEvent, INFINITE);
				}
			}
		#elif OSIS==VXWORKS
			semTake( itsSemaphore, WAIT_FOREVER);
		#else // assume POSIX
			sem_wait(&itsSemaphore);
		#endif
	}
	
	/// try to decrement the value of the semaphore
	/** \return true if the value was successfully decremented
		\return false if the value was 0 */
	bool trywait() {
		#if OSIS==WINDOWS
			if(itsValue > 0) {
				itsValue--;
				return true;
			} else 
				return false;
		#elif OSIS==VXWORKS
			return (semTake( itsSemaphore, NO_WAIT) == OK);
		#else // assume POSIX
			return (sem_trywait(&itsSemaphore) == 0);
		#endif
	}
	
	/// increment the value of the semaphore
	void post() {
		#if OSIS==WINDOWS
			if(++itsValue == 1) SetEvent(itsEvent);
		#elif OSIS==VXWORKS
			semGive( itsSemaphore);
		#else // assume POSIX
			sem_post(&itsSemaphore);
		#endif
	}
	
private:
	#if OSIS==WINDOWS
		HANDLE itsEvent;
		int itsValue;
	#elif OSIS==VXWORKS
		SEM_ID itsSemaphore;
	#else // assume POSIX
		sem_t itsSemaphore;
	#endif
};
#endif
