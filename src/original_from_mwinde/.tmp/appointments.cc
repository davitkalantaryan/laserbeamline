// appointments.cpp
// M. Winde, DESY-Zeuthen, 26.06.2002

#include "appointments.h"

const appPointer appointmentList::wait() {
	timePoint now;
	appPointer nextApp = front();
	if(!nextApp->active) {
		// no active appointment, wait 1 sec then return
		timeInterval(1.0).wait();
		return NULL;
	} else {
		if(nextApp->itsTimePoint > now) {
			timeInterval(nextApp->itsTimePoint - now).wait();
		}
	}
	return nextApp;
}

void appointmentList::insertBasedOnTime(appPointer anApp) {
    //??
    //iterator p = find_first_of( begin(), end(), &anApp, (&anApp)+1, isLater() );
    //insert(p, anApp);
}

appointment::appointment(appointmentList* theList)
	: 	active(false)
	,	itsList(theList)
{
	theList->push_back(this);
}

appointment::appointment(appointmentList* theList, const timePoint& tp)
	:	active(true)
	,	itsTimePoint(tp)
	,	itsList(theList)
{	
	theList->insertBasedOnTime(this);
}

appointment::appointment(appointmentList* theList, const timeInterval& ti)
	:	active(true)
	,	itsTimePoint(ti + timePoint())
	,	itsList(theList)
{	
	theList->insertBasedOnTime(this);
}

appointment::~appointment() {
	itsList->remove(this);
}

void appointment::make(const timePoint& tp) {
// set the time of an inactive appointment, make the appointment activ;
// or change the time of an active  appointment -- at a point in time
	itsList->remove(this);
	itsTimePoint = tp;
	active = true;
	itsList->insertBasedOnTime(this);
}

void appointment::make(const timeInterval& ti) {
// set the time of an inactive appointment, make the appointment activ;
// or change the time of an active  appointment -- after an interval in time
	itsList->remove(this);
	itsTimePoint = ti + timePoint();
	active = true;
	itsList->insertBasedOnTime(this);
}

bool appointment::reached() {
// return: active and time of appointment is reached
	return (active && (timePoint() >= itsTimePoint));
}

void appointment::done() {
// mark the appointment as inactive
	active = false; 
	itsList->remove(this);
	itsList->push_back(this);
}
