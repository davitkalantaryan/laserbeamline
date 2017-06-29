// IPUnidig_server.h

// abstract class for bitwise digital I/O
// M. Winde, DESY Zeuthen, 16.06.2004

#ifndef __INCIPUnidig_serverh
#define __INCIPUnidig_serverh

#include "DigIO.h"
#include "doocsAPI.h"

class IPUnidig_server : public DigIO {
   public:
	// --- creator
	IPUnidig_server(const char* the_sc_name) 
	    :	DigIO(),
		standardErrReact(),
		errReactGroup1(&standardErrReact),
		IP_NIBBLE(the_sc_name, &errReactGroup1)
{}
	
	// --- interface
	virtual void Write(uint32_t out) {IP_NIBBLE = out;} 	// write to Output lines
	virtual uint32_t Read() {return IP_NIBBLE;}		// read from read lines
    private:
	doocsErrorReaction standardErrReact;
 	doocsErrReactGroup errReactGroup1;
	doocs<int> IP_NIBBLE;
};

#endif // __INCIPUnidig_serverh
