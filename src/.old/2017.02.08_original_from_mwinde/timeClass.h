// timeClass.h
// M. Winde, DESY-Zeuthen, 17.08.01
// 
// Two classes to perform calculations with points in time (class timePoint) and 
// time intervals (class timeInterval)

// 25.02.2003 - return value (instead of reference) from operators +, - ...

#ifndef TIMECLASS_H
#define TIMECLASS_H

#include "definePosixSource.h"

#include <time.h>

/** \defgroup timeClass timeClass classes
	Two classes to perform calculations with points in time (class timePoint) and 
	time intervals (class timeInterval).
	Operators that combine objects of both classes are provided. 
	
	Struct timespec is used to store any time values, clock_gettime() is used to get the current time 
	(see "man clock_gettime"). That allows to deal with time values with the (theoretical) accuracy of 1 ns.

	Why two classes?

	There obviously is a difference between a point in time and a time interval. E. g. it makes sense to add
	two time intervals, while it makes no sense to add two points in time. On the other hand a timePoint minus a timePoint gives
	a timeInterval. Also it is meaningfull to multiply a time interval by a factor, while that seems useless
	for a point in time. If the programmer distingishes between the two types at the declaration of an object,
	the compiler is able to detect errors in the combination of points in time and time intervals.
	(By the way: that same difference exists between points in space and vectors.)

	\{
*/
class timeInterval : public timespec
{
public:
	/// make an interval of sec seconds plus nsec nanoseconds
	timeInterval(time_t sec = 0, long nsec = 0);

	/// make an interval of t seconds
	timeInterval(double t);
	
	/// set to sec seconds plus nsec nanoseconds
	void set(time_t sec = 0, long nsec = 0);
	
	/// set to t seconds
	void set(double t);
	
	/// return seconds as double
	double get() const;	
	
	/// return number of whole seconds as long
	long wholeSeconds() const {return tv_sec;}
	
	/// return number of whole milliseconds as long
	long wholeMilliSeconds() const {return tv_sec*1000 + tv_nsec/1000000;}
	
	/// wait (pause the thread) for the timeInterval
	void wait();			// wait interval duration

	/// timeInterval += timeInterval
	timeInterval &operator+=( const timeInterval &t );

	/// timeInterval -= timeInterval
	timeInterval &operator-=( const timeInterval &t );
	
	friend bool operator==( const timeInterval &t1, const timeInterval &t2);
	friend bool operator>( const timeInterval &t1, const timeInterval &t2);
	friend bool operator<( const timeInterval &t1, const timeInterval &t2);
	
};


	/// timeInterval = timeInterval - timeInterval
	timeInterval operator-( const timeInterval &t1, const timeInterval &t2);

	/// timeInterval = timeInterval + timeInterval
	timeInterval operator+( const timeInterval &t1, const timeInterval &t2);

	/// timeInterval = timeInterval * factor
	timeInterval operator*( const timeInterval& t, double factor);

	/// timeInterval = factor * timeInterval
	timeInterval operator*( double factor, const timeInterval& t);

	/// timeInterval = timeInterval / divisor
	timeInterval operator/( const timeInterval& t, double quotient);

	/// timeInterval == timeInterval
	bool operator==( const timeInterval &t1, const timeInterval &t2);

	/// timeInterval > timeInterval
	bool operator>( const timeInterval &t1, const timeInterval &t2);

	/// timeInterval >= timeInterval
	bool operator>=( const timeInterval &t1, const timeInterval &t2);

	/// timeInterval < timeInterval
	bool operator<( const timeInterval &t1, const timeInterval &t2);

	/// timeInterval <= timeInterval
	bool operator<=( const timeInterval &t1, const timeInterval &t2);

/*-- not compatible with gcc v2.7.2, use max<timeInterval> instead

	/// return the maximum of two time intervals
	timeInterval max( const timeInterval &t1, const timeInterval &t2);

	/// return the minimum of two time intervals
	timeInterval min( const timeInterval &t1, const timeInterval &t2);
*/

class timePoint : public timespec
{
public:
	/// make a point in time, set it to current time
	timePoint();

	/// make a point in time, set it to current time if setNow==true
	timePoint(bool setNow);
	
	/// make a point in time, set it to t (copy constructor)
	timePoint(const timePoint& t);
	
	~timePoint();
	
	/// assign operator
	timePoint& operator=( const timePoint &t );

	/// set to current time
	void now();
	
	/// wait until this point in time (pause the thread)
	void wait();
	
	/// return the UNIX time (seconds since 01.01.1970 00:00:00 UT)
	time_t unixTime() const {return tv_sec;}


	/// timePoint += timeInterval
	timePoint &operator+=( const timeInterval &t );
	
	/// timePoint -= timeInterval
	timePoint &operator-=( const timeInterval &t );
	
	friend timeInterval operator-( const timePoint &t1, const timePoint &t2);
	friend bool operator==( const timePoint &t1, const timePoint &t2);
	friend bool operator>( const timePoint &t1, const timePoint &t2);
	friend bool operator<( const timePoint &t1, const timePoint &t2);

	/// return the (date and) time as a string
	/** \param opt specifies the format of the string. May be empty or any combination of timePoint::ms | 
		timePoint::date | timePoint::once | timePoint::space.
		\return const pointer to the string.
		\code Example:
		timePoint().str();									// returns "14:27:00 " - note the trayling space
		timePoint().str(timePoint::ms | timePoint::date);	// returns "2002/07/26  14:27:00.371 "
	*/
	const char* str(int opt = 0);
	
	/// predicate for parameter opt of method str(opt)
	/** add milliseconds */
	const static int ms = 0x1;
	
	/// predicate for parameter opt of method str(opt)
	/** add date */
	const static int date = 0x2;
	
	/// predicate for parameter opt of method str(opt)
	/** return the string only once until the timePoint was assigned a new value, else return a null-string.
		This is usefull when you want to generate variable text elements, allocated to the same
		timePoint.
		\code Example:
		for(;;) {
			timePoint happendedAt();
			if(event1) cout << happendedAt.str(timePoint::once) << "event1 happened" << endl;
			if(event2) cout << happendedAt.str(timePoint::once) << "event2 happened" << endl;
			if(event3) cout << happendedAt.str(timePoint::once) << "event3 happened" << endl;
			...
		}
		\endcode
		With event1==false, event2==true, event3==true the output will be something like:
		\code
		14:27:00 event2 happened
		event3 happened
		\endcode
		With event1==event2==event3==false no output will be generated at all.
		 */
	const static int once = 0x4;
	
	/// predicate for parameter opt of method str(opt)
	/** usefull in combination with timePoint::once only: returns spaces instead of a null-string.
			\code Example:
		for(;;) {
			timePoint happendedAt();
			if(event1) cout << happendedAt.str(timePoint::once | timePoint::space) << "event1 happened" << endl;
			if(event2) cout << happendedAt.str(timePoint::once | timePoint::space) << "event2 happened" << endl;
			if(event3) cout << happendedAt.str(timePoint::once | timePoint::space) << "event3 happened" << endl;
			...
		}
		\endcode
		With event1==false, event2==true, event3==true the output will be something like:
		\code
		14:27:00 event2 happened
		         event3 happened
		\endcode
		With event1==event2==event3==false no output will be generated at all.
	 */
	const static int space = 0x8;

private:
	const static int charsForDate = 11;
	const static int charsForTime = 9;
	const static int charsForMs = 4;
	char firstMilliChar;
	char* strBuf;
	enum {notSet, justSet, oncePrinted} strMode;
};


	/// timeInterval = timePoint - timePoint
	timeInterval operator-( const timePoint &t1, const timePoint &t2);

	/// timePoint = timePoint - timeInterval
	timePoint operator-( const timePoint &t1, const timeInterval &t2);

	/// timePoint = timePoint + timeInterval
	timePoint operator+( const timePoint &t1, const timeInterval &t2);

	/// timePoint = timeInterval + timePoint
	timePoint operator+( const timeInterval &t1, const timePoint &t2);

	/// timePoint == timePoint
	bool operator==( const timePoint &t1, const timePoint &t2);

	/// timePoint > timePoint (later)
	bool operator>( const timePoint &t1, const timePoint &t2);

	/// timePoint >= timePoint (later or same)
	bool operator>=( const timePoint &t1, const timePoint &t2);

	/// timePoint < timePoint (earlyer)
	bool operator<( const timePoint &t1, const timePoint &t2);

	/// timePoint <= timePoint (earlyer or same)
	bool operator<=( const timePoint &t1, const timePoint &t2);

/*-- not compatible to gcc v2.7.2, use max<timePoint> instead

	/// return the later one of two timePoints
	timePoint max( const timePoint &t1, const timePoint &t2);

	/// return the earlyer one of two timePoints
	timePoint min( const timePoint &t1, const timePoint &t2);
*/

/** \} */ // end of group timeClass

#endif // TIMECLASS_H
