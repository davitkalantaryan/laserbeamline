#ifndef doocsDebug_h
#define doocsDebug_h

#ifdef DEBUG
#define IFDEBUG(x) if(testSettings.value() & x) {
#define ENDDEBUG }
#else
#define IFDEBUG(x)
#define ENDDEBUG
#endif	//DEBUG

#endif	//doocsDebug
