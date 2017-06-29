// timeClass.cpp
// M. Winde, DESY-Zeuthen, 17.08.01

// 25.02.2003 - return value (instead of reference) from operators +, - ...

#include "timeClass.h"

const long nanoSecPerSec = 1000000000;	// number of ns per s

inline void nanoSecOverflow(time_t& tv_sec, long& tv_nsec) {
	if(tv_nsec >= nanoSecPerSec) {
		tv_nsec -= nanoSecPerSec;
		tv_sec++;
	}
}

inline void nanoSecUnderflow(time_t& tv_sec, long& tv_nsec) {
	if(tv_nsec < 0) {
		tv_nsec += nanoSecPerSec;
		tv_sec--;
	}
}

//---------------------------------------------------------------------------

timeInterval::timeInterval(time_t sec, long nsec) {
	set(sec, nsec);
}

timeInterval::timeInterval(double t) {
	set(t);
}

void timeInterval::set(time_t sec, long nsec) {
	tv_sec = sec;
	tv_nsec = nsec;
}

void timeInterval::set(double t) {
	if(t >= 0.) {
		tv_sec = t;
		tv_nsec = ( t - tv_sec) * nanoSecPerSec;
	} else { // negativ: avoid implementation dependencies for negative values
		double tabs = -t;
		time_t intPart = tabs;
		tv_sec = -intPart;
		if((tv_nsec = long(tabs - intPart) * nanoSecPerSec) != 0) {
			tv_sec--;
			tv_nsec = nanoSecPerSec - tv_nsec;
		};		
	}
}

double timeInterval::get() const {
	return double(tv_sec) + double(tv_nsec)/nanoSecPerSec;
}

void timeInterval::wait() {
	if (*this < timeInterval(0, 0)) return; // no wait if time is negative
	nanosleep(this, NULL);
}

timeInterval operator+( const timeInterval &t1, const timeInterval &t2) {
	timeInterval newTI = t1;
	newTI += t2;
	return newTI;
}

timeInterval operator-( const timeInterval &t1, const timeInterval &t2) {
	timeInterval newTI = t1;
	return (newTI -= t2);
}

timeInterval& timeInterval::operator+=( const timeInterval &t ) {
	tv_sec += t.tv_sec;
	tv_nsec += t.tv_nsec;
	nanoSecOverflow(tv_sec, tv_nsec);
	return *this;
}

timeInterval& timeInterval::operator-=( const timeInterval &t ) {
	tv_sec -= t.tv_sec;
	tv_nsec -= t.tv_nsec;
	nanoSecUnderflow(tv_sec, tv_nsec);
	return *this;
}

timeInterval operator*( const timeInterval& t, double factor) {
	timeInterval newTI = timeInterval(t.get()*factor);
	return newTI;
}

timeInterval operator*( double factor, const timeInterval& t) {
	return t*factor;
}

timeInterval operator/( const timeInterval& t, double quotient) {
	return t*(1.0/quotient);
}

bool operator==( const timeInterval &t1, const timeInterval &t2) {
	if (t1.tv_sec != t2.tv_sec) return false;
	return (t1.tv_nsec == t2.tv_nsec);
}

bool operator>=( const timeInterval &t1, const timeInterval &t2) {
	return (t1==t2 || t1>t2);
}

bool operator>( const timeInterval &t1, const timeInterval &t2) {
	if (t1.tv_sec < t2.tv_sec) return false;
	if (t1.tv_sec > t2.tv_sec) return true;
	return (t1.tv_nsec > t2.tv_nsec);
}

bool operator<=( const timeInterval &t1, const timeInterval &t2) {
	return (t1==t2 || t1<t2);
}

bool operator<( const timeInterval &t1, const timeInterval &t2) {
	if (t1.tv_sec > t2.tv_sec) return false;
	if (t1.tv_sec < t2.tv_sec) return true;
	return (t1.tv_nsec < t2.tv_nsec);
}

/*-- not compatible with gcc v2.7.2, use max<timeInterval> instead

timeInterval max( const timeInterval &t1, const timeInterval &t2) {
	return (t1 > t2)?t1:t2;
}

timeInterval min( const timeInterval &t1, const timeInterval &t2) {
	return (t1 < t2)?t1:t2;
}
*/

//---------------------------------------------------------------------------

timePoint::timePoint(bool setNow)
	: strBuf(NULL)
{
	strMode = notSet;
	if(setNow) now();
}

timePoint::timePoint() 
	: strBuf(NULL)
{
	now();
}

timePoint::timePoint(const timePoint& t)
	: timespec(t)
	, strBuf(NULL)
{
	strMode = (t.strMode == notSet) ? notSet : justSet;
}

timePoint& timePoint::operator=( const timePoint &t ) {
	if(this != &t) {
		tv_sec = t.tv_sec;
		tv_nsec = t.tv_nsec;
		delete[] strBuf;
		strMode = (t.strMode == notSet) ? notSet : justSet;
	}
	return *this;
}


timePoint::~timePoint() {
	delete[] strBuf;
}

void timePoint::now() {
	clock_gettime(CLOCK_REALTIME, this);
	strMode = justSet;
}

void timePoint::wait() {
	timeInterval waitTI = timeInterval(*this - timePoint(true));
	waitTI.wait();
}

static const char spaces[] = "                        ";

const char* timePoint::str(int opt) {
	struct tm timeBuffer;
	localtime_r(&tv_sec, &timeBuffer);
	
	if(strMode == notSet) return "??:??:??";	// time was not set
	if((opt & once) & (strMode == oncePrinted)) {
		// str is demanded once only, and it was already demanded before
		if(opt & space) {
			// return spaces
			//return "        ";
			const char* spaceP = spaces;
			if(!(opt & date)) spaceP += charsForDate;
			if(!(opt & ms)) spaceP += charsForMs;
			return spaceP;
		} else {
			return "";
		}
	}
	
	if(strMode == justSet) {
		if(strBuf == NULL) strBuf = new char[charsForDate+charsForTime+charsForMs+1];
		strftime(strBuf, charsForDate+charsForTime+1, "%Y/%m/%d %H:%M:%S ", &timeBuffer);
		int mSec = tv_nsec/1000000;
		char c = mSec/100;
		firstMilliChar = c + '0';
		mSec -= c*100;
		c = mSec/10;
		strBuf[charsForDate+charsForTime+1] = c + '0';
		mSec -= c*10;
		strBuf[charsForDate+charsForTime+2] = c + '0';
		strBuf[charsForDate+charsForTime+3] = ' ';
		strBuf[charsForDate+charsForTime+4] = char(0);
		strMode = oncePrinted;
	}
	if(opt & ms) {
		strBuf[charsForDate+charsForTime-1] = '.';
		strBuf[charsForDate+charsForTime] = firstMilliChar;
	} else {
		strBuf[charsForDate+charsForTime-1] = ' ';
		strBuf[charsForDate+charsForTime] = char(0);
	}
	return ( (opt & date) ? strBuf : strBuf+charsForDate );
}

timePoint operator+( const timePoint &t1, const timeInterval &t2) {
	timePoint newTI = t1;
	newTI += t2;
	return newTI;
}

timePoint operator+( const timeInterval &t1, const timePoint &t2) {
	return t2+t1;
}

timeInterval operator-( const timePoint &t1, const timePoint &t2) {
	timeInterval newTI;
	newTI.tv_sec = t1.tv_sec - t2.tv_sec;
	newTI.tv_nsec = t1.tv_nsec - t2.tv_nsec;
	nanoSecUnderflow(newTI.tv_sec, newTI.tv_nsec);
	return newTI;
}

timePoint operator-( const timePoint &t1, const timeInterval &t2) {
	timePoint newTI = t1;
	return (newTI -= t2);
}

timePoint& timePoint::operator+=( const timeInterval &t ) {
	tv_sec += t.tv_sec;
	tv_nsec += t.tv_nsec;
	nanoSecOverflow(tv_sec, tv_nsec);
	strMode = justSet;
	return *this;
}

timePoint & timePoint::operator-=( const timeInterval &t ) {
	tv_sec -= t.tv_sec;
	tv_nsec -= t.tv_nsec;
	nanoSecUnderflow(tv_sec, tv_nsec);
	strMode = justSet;
	return *this;
}

bool operator==( const timePoint &t1, const timePoint &t2) {
	if (t1.tv_sec != t2.tv_sec) return false;
	return (t1.tv_nsec == t2.tv_nsec);
}

bool operator>( const timePoint &t1, const timePoint &t2) {
	if (t1.tv_sec < t2.tv_sec) return false;
	if (t1.tv_sec > t2.tv_sec) return true;
	return (t1.tv_nsec > t2.tv_nsec);
}

bool operator>=( const timePoint &t1, const timePoint &t2) {
	return (t1==t2 || t1>t2);
}

bool operator<( const timePoint &t1, const timePoint &t2) {
	if (t1.tv_sec > t2.tv_sec) return false;
	if (t1.tv_sec < t2.tv_sec) return true;
	return (t1.tv_nsec < t2.tv_nsec);
}

bool operator<=( const timePoint &t1, const timePoint &t2) {
	return (t1==t2 || t1<t2);
}

/*-- not compatible to gcc v2.7.2, use max<timePoint> instead

timePoint max( const timePoint &t1, const timePoint &t2) {
	return (t1 > t2)?t1:t2;
}

timePoint min( const timePoint &t1, const timePoint &t2) {
	return (t1 < t2)?t1:t2;
}
*/

