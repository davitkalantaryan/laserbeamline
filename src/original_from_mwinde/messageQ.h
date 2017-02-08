//messageQ.h
#ifndef __INCmessageQh
#define __INCmessageQh

#include "sema.h"
#include <queue>

/** \defgroup messageQ messageQ
	template class for semaphore-guarded queues in a multi-thread program.
	
	A semaphore is used to block a thread from access to the queue while any other thread is
	just modifying it. An other semaphore is used to
	block a thread that waits for a message.

	\{
*/

/// template class for semaphore-guarded queues.
/** provides for the safe handling of a message queue in a multi-thread program.
	A semaphore is used to block a thread from access to the queue while any other thread is
	just modifying it. An other semaphore is used to
	block a thread that waits for a message.
*/    
template<class T> class messageQ {

public:
	/// create a message queue
	messageQ();			// constructor
	
	~messageQ();
	
	/// put a message to the queue
	/** \param toSend	put this object to the end of the queue
	*/
	void send(T toSend);
	
	/// get a message from the queue
	/** 
	- if the queue is empty: wait until at least one message is in the queue
- get the first (oldest) message in the queue, delete it from the queue
		\return the message
	*/
	T receive();
	
	/// get a message without waiting
	/** 
	- if the queue is empty: return noWaitValue
- get the first (oldest) message in the queue, delete it from the queue
		\param noWaitValue	the message to return in case the queue is empty
		\return the message
	*/
	T receiveNowait(const T& noWaitValue);

private:
	sema sem_Access;	// synch internal access to the itsQ structure
	sema sem_Q;		// synch (external) send and receive operations
	std::queue<T> itsQ;
};

/** \} */ // end of group messageQ

#endif
