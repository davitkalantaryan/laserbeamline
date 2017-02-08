//RS485_Controller.h

#ifndef __INCRS485_Controllerh
#define __INCRS485_Controllerh

class RS485_Controller
// virtual class
{
public:
	// --- creator
	RS485_Controller() {
	    endOfInfo = -1;	// set the default to no test
	}

	virtual int Write(const char * tele, int len) = 0; // write len chars, return number of chars written

	virtual int Read(char * tele, int len) = 0;	// read 0..len chars, return number of chars read
	
	virtual void SetEndOfInfo(int aChar) { endOfInfo = aChar;}

        virtual const char * GetVersion() { return "";}
	virtual int State() { return 0;}	// return 0 if all is OK, a negativ value for an error state

   protected:
	int endOfInfo;

};

#endif // __INCRS485_Controllerh
