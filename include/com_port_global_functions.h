/*
 *	File: com_port_global_functions.h
 *
 *	Created on: 29 Jun, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */
#ifndef __com_port_global_functions_h__
#define __com_port_global_functions_h__

#include "pitz_rpi_tools_serial.hpp"
#include "common_serial_comport.hpp"

typedef int SerialParityT;
namespace SerialParity{
enum {
	None,
	Odd,
	Even,
	Mark,
	Space,
};
}

typedef int SerialStopBitsT;
namespace SerialStopBits{
enum {
	One,
	OnePointFive,
	Two,
};
}

int MakeErrorReport(void);
int PrepareSerial(pitz::rpi::tools::Serial* a_pSerial, const char* a_portName);
int PrepareSerial2(common::serial::ComPort* a_pSerial, const char* a_portName);


#endif  // #ifndef __com_port_global_functions_h__
