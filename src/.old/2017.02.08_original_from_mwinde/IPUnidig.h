// IPUnidig.h
// M. Winde, DESY Zeuthen, 14.12.01
// - 25.07.03 	- use IPCarrierName an offset instaed of absolute VME-address
//				- allow the use of different IP-Unidig modules

#ifndef __INCIPUnidigh
#define __INCIPUnidigh

#include "DigIO.h"
#include "IPModule.h"

// this provides only the MINIMAL set of functions to work with SBS Greenspring's
// Ip-Unidig modules
// 14. 12. 01 M. Winde, DESY Zeuthen

struct ipUnidig_regs          {

	volatile uint16_t   output;		/* Write: Output line  1-16, Read: Read back lines  1-16 */
	volatile uint16_t   outputH;	/* Write: Output line 17-24, Read: Read back lines 17-24 */
	volatile uint16_t   input;		/* Read: Direct read lines  1-16 */
	volatile uint16_t   inputH;		/* Read: Direct read lines 17-24 */

	volatile uint16_t	dum1[2];
	volatile uint16_t   CR;			/* Read/Write: Control Register */
	volatile uint16_t	dum2;
	volatile uint16_t   IRVector;	/* Read/Write: Interrupt Vector Register */
	
	volatile uint16_t   IREnable;	/* Read/Write: Interrupt Vector Register lines  1-16 */
	volatile uint16_t   IREnableH;	/* Read/Write: Interrupt Vector Register lines 17-24 */
	
	volatile uint16_t   IRPolarity;	/* Read/Write: Interrupt Polarity Register lines  1-16 */
	volatile uint16_t   IRPolarityH;/* Read/Write: Interrupt Polarity Register lines 17-24 */
	
	volatile uint16_t   LAM;		/* Write: IR clear Register  1-16, Read: IR Pending Register lines  1-16 */
	volatile uint16_t   LAMH;		/* Write: IR clear Register 17-24, Read: IR Pending Register lines 17-24 */
};

typedef struct ipUnidig_regs ipUnidig_regs;

class IPUnidig : public DigIO, public IPModule
{
  
   public:
	// --- creator
	IPUnidig(const char* ipCarrierName, off_t addr_offset, int modspec);
	// ipCarrierName: as in $VME_CONF/VME.<host>.conf
	// addr_offset: address offset of the IP module in the carrier
	// modspec: 0 - IP-UNIDIG
	//          1 - IP-UNIDIG-I
	//          2 - IP-UNIDIG-D	
	
	// --- interface
	void Write(uint32_t out); 	// write to Output line 1-24
	uint32_t ReadBack();		// read back lines  1-24
	uint32_t Read();			// read from Direct read lines  1-24
	
   private:
	ipUnidig_regs * mp;	// base address
	
};

#endif // __INCIPUnidigh
