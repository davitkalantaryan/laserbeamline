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
#include "common/common_serial_comport.hpp"

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
int PrepareSerial2_old(common::serial::ComPort* a_pSerial, const char* a_portName);
int MakeStatisticForCom(common::serial::ComPort* a_pSerial);
#ifdef _USE_PITZ_RPI_SERIAL
int PrepareSerial(pitz::rpi::tools::Serial* a_pSerial, const char* a_portName);
#endif

#ifdef __cplusplus

template<typename SerialType>
int MakeStatisticForComT(SerialType* a_pSerial);
#include "impl.com_port_global_functions.h"
#endif


#endif  // #ifndef __com_port_global_functions_h__
