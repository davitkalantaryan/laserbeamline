#ifndef __mtca_stdafx_h__
#define __mtca_stdafx_h__


#ifdef _WIN64
	#ifndef WIN32
		#define WIN32
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <conio.h>
#define EXIT_PROG(x) \
	{\
		printf("Press any key to exit!");\
		getch();\
		exit (x); \
	}
#else   /* #ifdef WIN32 */
#define EXIT_PROG(x) exit(x)
#endif	/* #ifdef WIN32 */


#endif/* #ifndef __mtca_stdafx_h__ */
