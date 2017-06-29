// stopwatch.h
// M. Winde, DESY-Zeuthen, 17.08.01
// 

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "timeClass.h"

/// A class to perform various time meassurements.    
class stopwatch
{
public:
	stopwatch();

	/// start a new time meassurement
	/** Reset all, then run the timer.
		\return always true (stopwatch was started just now) */
	bool Start();
							
	/// stop (pause) the timer
	/** meassurement may be continued by Continue() later.
		\return true if the stopwatch was stopped just now, false if it was
		not running before */
	bool Stop();
							
	/// continue a meassurement paused by  Stop().
	/** \return true if the stopwatch was really continued, false if it was
		running before */
	bool Continue();
	
	/// time since Start() or last Continue() 
	/** \return the time since Start() or last Continue() 
		[whatever came later] until last Stop(),
		or until now if the meassurement is in progress */
	timeInterval lastTime();
									 
	/// sum of all meassured time intervals since Start()
	/** \return the sum of all meassured time intervals since Start()
		until last Stop(), or until now if the meassurement is in progress.
		This excludes all time intervals when the stopwatch was stopped. */
	timeInterval time();
									
	/// time since Start()
	/** \return the time since Start()	until last Stop(),
		or until now if the meassurement is in progress. This includes times with
		the stopwatch having been stopped. */
	timeInterval timeSinceStart();
									
	/// seconds since last Start() or last Continue()
	/** \return the number of seconds since Start() or last Continue() 
		[whatever came later] until last Stop(),
		or until now if the meassurement is in progress */
	double lastSeconds()	{	return lastTime().get();}
	
	/// sum of all meassured time intervals since Start()
	/** \return the sum (in seconds) of all meassured time intervals since Start()
		until last Stop(), or until now if the meassurement is in progress.
		This excludes all time intervals when the stopwatch was stopped. */
	double seconds() {return time().get();}
	
	/// seconds since Start()
	/** \return the number of seconds since Start()	until last Stop(),
		or until now if the meassurement is in progress. This includes times with
		the stopwatch having been stopped. */
	double secondsSinceStart() { return timeSinceStart().get(); }

private:
	bool meassuring;	// meassurement is running (not paused)
	timePoint startTime;
	timePoint continueTime;
	timePoint stopTime;
	
	timeInterval accumulatedTime;
};

#endif // STOPWATCH_H
