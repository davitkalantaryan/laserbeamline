//messageQ.cpp

#include "messageQ.h"

template<class T> messageQ<T>::messageQ()
	: sem_Access(1)
	, sem_Q()
{
}

template<class T> messageQ<T>::~messageQ() {
}

template<class T> void messageQ<T>::send(T toSend) {
	sem_Access.wait();
	itsQ.push(toSend);
	sem_Access.post();
	sem_Q.post();
}

template<class T> T messageQ<T>::receive() {
	sem_Q.wait();
	sem_Access.wait();
	T received = itsQ.front();
	itsQ.pop();
	sem_Access.post();
	return received;
}

template<class T> T messageQ<T>::receiveNowait(const T& noWaitValue) {
	if(!sem_Q.trywait()) return noWaitValue;
	sem_Access.wait();
	T received = itsQ.front();
	itsQ.pop();
	sem_Access.post();
	return received;
}
