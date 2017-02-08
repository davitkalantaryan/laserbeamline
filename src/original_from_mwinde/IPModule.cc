// IPModule.cpp
// M. Winde 14.12.01
// -- 18.07.03 - map 0x1000 memory bytes


#include "IPModule.h"
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "VME.h"

#define OS_Solaris
#ifdef OS_Solaris

using namespace std;


#include <errno.h>
int errno;

//?static uint16_t* mpIP = 0;
static VME* vme = 0;     

// --- creator
IPModule::IPModule(const char* ipCarrierName, off_t addr_offset) {
// addr_offset: VME base address of the IP module
// ident: ID PROM data awaited for that IP type, len words will be compared

	base = (uint16_t *)-1;	// -1 means: nothing assigned
	moduleErr = -1;
//? /*??	
	if(vme == 0) vme = new VME;
	caddr_t vme_a16=vme->map_device(ipCarrierName);
	if(vme_a16 == (caddr_t) -1) {
                std::cerr << "IPModule::IPModule can't map " << ipCarrierName << endl;
		return;
	}
	base = (uint16_t*)(vme_a16 + (addr_offset & 0x300));
	//cerr << "IPModule::IPModule " << ipCarrierName << " mapped to " << hex << base << dec <<endl; 
//? */
/*?	 
	if(mpIP == 0) {
		fileDescriptor = open("/dev/vme16d16", O_RDWR);
		if (fileDescriptor == -1) return;	//  --- can't open VME 
	
		moduleErr = -2;
		int offset = (addr_offset & 0xF00)/2;	// offset from last 4k boundary [16 bit units]
		mpIP = (uint16_t *)mmap ((caddr_t)0, 0x10000, PROT_READ | PROT_WRITE,
						MAP_SHARED, fileDescriptor, 0x0);
		if(mpIP == (uint16_t *)-1) {
			perror("IPModule::IPModule can't map");
			cerr << "errno = " << errno << hex << ", addr_offset = " << addr_offset << dec << endl;
			return;	// can't map
		}
	cerr << "IPModule::IPModule VME A16D16 mapped" << endl;
	}
	// all OK, so let's assign the pointer to the base address
	moduleErr = 0;
	base = mpIP+(addr_offset & 0xFF00)/2;
*/	
}

bool IPModule::IDPromOK(uchar_t* IdProm, int len) {
// test the ip-module-type in the ID prom
// IdProm: ID PROM data awaited for that IP type, len words will be compared

	if(base == (uint16_t *)-1) return false;
	
	bool isOK = true;
	uchar_t* ip_promP = IdProm;
	uint16_t* prom= base+0x40;
	uchar_t promRead[0x40];
	int i;
	for (i = 0; i<len; i++) {
		if ((promRead[i] = *prom++ & 0xFF) != ip_promP[i]) {
			isOK = false;	// --- no such IP module at this address
		} 
	}
	if(!isOK) {  // --- no such IP module at this address
		moduleErr = -3;
		base = (uint16_t *)-1;	// -1 means: nothing assigned
		cerr << "// --- no such IP module at this address," << endl << "wanted:" << hex;
		for(i=0; i<len; i++) {
			cerr << uint16_t(ip_promP[i]) << ",";
		}
		cerr << endl << " found:";
		for(i=0; i<len; i++) {
			cerr << uint16_t(promRead[i]) << ",";
		}	
		cerr << dec << endl;
	}
	return isOK;
}

#else	// simul input and output

#endif	// simulHardware
