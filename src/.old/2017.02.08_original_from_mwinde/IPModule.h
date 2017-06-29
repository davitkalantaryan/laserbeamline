// IPModule.h
// M. Winde, DESY Zeuthen, 14.12.01

#ifndef __INCIPModuleh
#define __INCIPModuleh

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#ifndef uchar_t
#define uchar_t unsigned char
#endif

class IPModule
// a class common for IP-modules on SBS Greenspring VIPC616
// Intel-Solaris

{
  
   public:
	// --- creator
	IPModule(const char* ipCarrierName, off_t addr_offset);
	// ipCarrierName: as in $VME_CONF/VME.<host>.conf
	// addr_offset: address offset of the IP module in the carrier
	
	// --- interface
        virtual const char * GetVersion() {return "V1.0 IPModule";}
	bool ModuleExists() {return (base != (uint16_t *)-1);}
	int ModuleError() {return moduleErr;}

   protected:
   	bool IDPromOK(uchar_t* IdProm, int len = 12);
	// IdProm: ID PROM data awaited for that IP type, len (= 12) words will be compared

   private:
	int fileDescriptor;
	int moduleErr;	// 0: module OK
					// -1: can't open VME
					// -2: can't map VME memory
					// -3: not the awaited IP module at that VME address
	
   protected:
	uint16_t * base;	// base address
	
};

#endif // __INCIPModuleh
