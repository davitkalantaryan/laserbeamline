//IPUnidig.cpp


#include "IPUnidig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define OS_Solaris
#ifdef OS_Solaris

// --- creator
IPUnidig::IPUnidig(const char* ipCarrierName, off_t addr_offset, int modspec) : IPModule(ipCarrierName, addr_offset)
{
	// ipCarrierName: as in $VME_CONF/VME.<host>.conf
	// addr_offset: address offset of the IP module in the carrier
	// modspec: 0 - IP-UNIDIG
	//          1 - IP-UNIDIG-I
	//          2 - IP-UNIDIG-D	

	// to test the ip-module-type in the prom
	uchar_t ip_prom[] = {
		'I', 'P', 'A', 'C',
		0xF0, 0x61, 0xA1, 0x00, 0x00, 0x00,
		0x0C
	};
	switch(modspec) {
		case 0: break;
		case 1: ip_prom[5] = 0x68;
			break;
		case 2: ip_prom[5] = 0x69;
		default:;
	}
	if( IDPromOK(ip_prom, sizeof(ip_prom)) ) {
		// cerr << "IPUnidig::IPUnidig mapped to " << hex << base << dec << endl;
		// all OK, so let's assign the pointer to the base address
		mp = (ipUnidig_regs *)base;
	
		// -- init the module
		// disable interrupts
		mp->IREnable = 0;
		mp->IREnableH = 0;
		// disable double buffering
		mp->CR = 0;
	}
}

void IPUnidig::Write(uint32_t out) {
 	// write to Output line 1-24

	if(!ModuleExists()) return; // --- error
	
	mp->output = (uint16_t)(out & 0xFFFF);
	mp->outputH = (uint16_t)((out >> 16) & 0xFFFF);
}

uint32_t IPUnidig::Read() {
	// read from Direct read lines  1-24
	
	if(!ModuleExists()) return 0; // --- error
	
	uint32_t buf = mp->inputH;
	buf = (buf << 16) | mp->input;
	return buf;
}

uint32_t IPUnidig::ReadBack() {
	// read back output lines  1-24
	
	if(!ModuleExists()) return 0; // --- error
	
	uint32_t buf = mp->outputH;
	buf = (buf << 16) | mp->output;
	return buf;
}

#else	// simul input and output

#endif	// simulHardware
