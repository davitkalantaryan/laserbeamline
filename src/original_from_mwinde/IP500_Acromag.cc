//IP500_Acromag.cpp


#include "IP500_Acromag.h"
#include "timeClass.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define OS_Solaris
#ifdef OS_Solaris

// --- creator
IP500_Acromag::IP500_Acromag(const char* ipCarrierName, off_t addr_offset, int channel)  
        : RS485_Controller(),IPModule(ipCarrierName, addr_offset)
{
	// ipCarrierName: as in $VME_CONF/VME.<host>.conf
	// addr_offset: address offset of the IP module in the carrier
	// channel: channel number of the used serial channel (0..3)
	
	// to test the ip-module-type in the prom
	uchar_t ip_prom[] = {
		'I', 'P', 'A', 'C',
		0xA3, 0x04, 0x00, 0x00, 0x00, 0x00,
		0x0C, 0x86
	};
	if( IDPromOK(ip_prom, sizeof(ip_prom)) ) {

		// all OK, so let's assign the pointer to the base address of the selected channel
		mp = (ip501ac_regs *)(base+ channel*0x8);
		
		// init the channel
		mp->r2.interruptEnable = 0;		/* disable interrupts */
		// set baudrate
			int divisor = 26; 	/* 19200 baud */
			mp->LCR = 0x80;	/* set LCR bit 7 high */
			timeInterval(0.001).wait();	// waist some time
			mp->r0.divisorLatchLSB = divisor & 0xFF;
			timeInterval(0.001).wait();	// waist some time
			mp->r2.divisorLatchMSB = (divisor >> 8) & 0xFF;
			timeInterval(0.001).wait();	// waist some time
		mp->LCR = 0x3; 	/* 8 data bits, 1 stop bits, no parity; set LCR bit 7 low */
			timeInterval(0.001).wait();	// waist some time
		mp->modemControl = 0x1; 	/* DTR activ, RTS inactiv, external serial chan disabled, loop disabled */
			timeInterval(0.001).wait();	// waist some time
		mp->r4.fifoControl = 0x7;/* enable FIFO, clear both FIFOs, set Rx-FIFO trigger level=1 byte (d.c.)*/
			timeInterval(0.001).wait();	// waist some time
		//mp->r4.fifoControl = 0x0;/* disable FIFO */
		
		// permanently enable transmitter
		//?? a semaphore should be used to synchronise write access
		/*int buf = *(base+0x20);
		*(base+0x20) = buf | (0x1 << channel);
			toWait.wait(0.001);*/	// waist some time
			
	}
}

int IP500_Acromag::Write(const char * tele, int len) {

	if(!ModuleExists()) return -1; // --- error
	
	// at that time the INPUT Fifo should be empty
	mp->r4.fifoControl = 0x3;/* enable FIFO, clear Rx-FIFO, set Rx-FIFO trigger level=1 byte (d.c.)*/
	
	
	// now we are going to output the command
	int spaceInFifo = 0;
	int i;
	for (i=0; i<len; i++) {
		int count = -5;
		if(spaceInFifo <= 0) { 
			while((mp->lineStatus & 0x20) == 0) {
				if(count++ >= 0) return i; // --- timeout (>20 ms)
				// wait until the FIFO is empty
				timeInterval(0.004).wait();	// wait 4 ms
			}
			spaceInFifo = 16;
		}
		mp->r0.transmitter = *tele++;
		spaceInFifo--;
	}
	return len; // --- OK
}

int IP500_Acromag::Read(char * tele, int len) {

    if(!ModuleExists()) return -1; // --- error

    int i = 0;	// chars read so far
    for(; i<len; i++) {
	if((mp->lineStatus& 0x1) == 0) return i; // --- no further chars yet
	*tele = mp->r0.receiveBuf & 0xFF;
	if(endOfInfo >=0) {
	    if(*tele++ == endOfInfo) return i+1; // --- endOfInfo char found
	} else {
	    tele++;
	}
    }
    return len;	// --- max. wanted length reached
}

const char * IP500_Acromag::GetVersion() {
	return "via IP500_Acromag";
}

#else	// simul input and output

#endif	// simulHardware
