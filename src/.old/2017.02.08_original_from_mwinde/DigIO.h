// DigIO.h

// abstract class for bitwise digital I/O
// M. Winde, DESY Zeuthen, 16.06.2004

#ifndef __INCIDigIOh
#define __INCIDigIOh

#include <sys/types.h>
#include <stdint.h>


class DigIO {
   public:
	// --- creator
	DigIO() {}
	
	// --- interface
	virtual void Write(uint32_t out) = 0; 	// write to Output lines
	virtual uint32_t Read() = 0;		// read from read lines
};

#endif // __INCIDigIOh
