// stopwatch.cpp
// M. Winde, DESY-Zeuthen, 17.08.01

#include "stopwatch.h"

stopwatch::stopwatch()
	: meassuring(false)
	, accumulatedTime()
	, startTime(false), continueTime(false), stopTime(false)
{}

bool stopwatch::Start() {		
	// start a new time meassurement: Reset all, then run the timer
	// a possibly running meassurement will be lost.
	startTime.now();
	continueTime = startTime;
	accumulatedTime.set();
	meassuring = true;
	return true;	// stopwatch was (always) started
}

bool stopwatch::Stop() {
	// stop (pause) the timer
	if(!meassuring) return false; // do nothing if no meassurement in progress
	meassuring = false;
	stopTime.now();
	accumulatedTime += (stopTime - continueTime);
	return true; // stopwatch was stopped just now
}

bool stopwatch::Continue() {
	// continue a meassurement paused by  Stop()
	if(meassuring) return false; // do nothing if meassurement in progress
	continueTime.now();
	meassuring = true;
	return true; // stopwatch was continued just now
}

timeInterval stopwatch::lastTime() {
	// return the time since Start() or last Continue() [whatever came later] 
	// until last Stop() or until now if the meassurement is in progress
	if(meassuring) return (timePoint(true) - continueTime);
	return (stopTime - continueTime);
}

timeInterval stopwatch::time() {
	// return the sum of all meassured time intervals since Start()
	// until last Stop() or until now if the meassurement is in progress
	if(!meassuring) return accumulatedTime;
	return accumulatedTime + lastTime();
}

timeInterval stopwatch::timeSinceStart() {
	// return the time since Start()
	// until last Stop() or until now if the meassurement is in progress
	if(!meassuring) return (stopTime - startTime);
	return (timePoint(true) - startTime);
}
