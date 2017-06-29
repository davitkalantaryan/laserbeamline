// definePosixSource.h

#ifndef DEFINEPOSIXSOURCE_H
#define DEFINEPOSIXSOURCE_H

//#define _POSIX_C_SOURCE = 199506L
#ifndef _REENTRANT
	#define _REENTRANT
#endif

#include "OSis.h"
#if OSIS!=VXWORKS // POSIX, WINDOWS
	#define STATUS int
	#define ERROR -1
	#define OK 0
#endif


#endif
