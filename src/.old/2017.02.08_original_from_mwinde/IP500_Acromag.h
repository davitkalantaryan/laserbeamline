//IP500_Acromag.h

#ifndef __INCIP500_Acromagh
#define __INCIP500_Acromagh

#include "IPModule.h"
#include "RS485_Controller.h"

struct ip501ac_regs          {
	union  {
		volatile uint16_t   receiveBuf;		/* READ */
		volatile uint16_t   transmitter;		/* WRITE */
		volatile uint16_t   divisorLatchLSB;	/* R/W, LCR bit 7==1 */
	} r0;
	union  {
		volatile uint16_t   interruptEnable;	/* R/W */
		volatile uint16_t   divisorLatchMSB;	/* R/W, LCR bit 7==1 */
	} r2;
	union {
		volatile uint16_t   interruptIdent;	/* READ */
		volatile uint16_t   fifoControl;		/* WRITE */
	} r4;
	volatile uint16_t   LCR;		/* R/W  line control register */
	volatile uint16_t   modemControl;/* R/W */
	volatile uint16_t   lineStatus;	/* R/W */
	volatile uint16_t   modemStatus;	/* R/W */
	volatile uint16_t   IRVector;	/* R/W */
};

typedef struct ip501ac_regs ip501ac_regs;

class IP500_Acromag : public RS485_Controller, public IPModule
{
  
   public:
	// --- creator
	IP500_Acromag(const char* ipCarrierName, off_t addr_offset, int channel=0);
	// ipCarrierName: as in $VME_CONF/VME.<host>.conf
	// addr_offset: address offset of the IP module in the carrier
	// channel: RS485 channel number (0..3)
		
	// --- interface
	int Write(const char * tele, int len); // write len chars, return number of chars written
	int Read(char * tele, int len);	// read 0..len chars, return number of chars read
		
        const char * GetVersion();

   private:
	int fileDescriptor;
	ip501ac_regs * mp;		// base address of the};

};

#endif // __INCIP500_Acromagh
