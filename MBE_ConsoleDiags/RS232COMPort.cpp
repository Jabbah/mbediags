/*
***************************************************************************
*
* Author: James Lacey
*
* Copyright (C) 2017 James Lacey
*
* Email: mbediags@jameslacey.co.uk
*
***************************************************************************
*
* This file is part of MBE_Diags.
*
* MBE_Diags is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not you can find his version of GPL at
* http://www.gnu.org/licenses/
*
***************************************************************************
*/

#include "RS232COMPort.h"
//#include "rs232.h"
#include <iostream>


const char mode[] = "8N1";

#ifdef _WIN32
    #include <windows.h>
	#define SLEEP(ms) Sleep(ms)
#else
    #include <unistd.h>
	#define SLEEP(ms) usleep(ms*1000)
#endif

#include "serial/serial.h"

void RS232COMPort::enumerate_ports()
{
	std::vector<serial::PortInfo> devices_found = serial::list_ports();

	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

	while( iter != devices_found.end() )
	{
		serial::PortInfo device = *iter++;

		printf( "(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
     device.hardware_id.c_str() );
	}
}

RS232COMPort::RS232COMPort(std::string port, int baud)
{
    _cp = std::make_shared<serial::Serial>(port, baud, serial::Timeout::simpleTimeout(1000));
}

RS232COMPort::~RS232COMPort()
{
}

bool RS232COMPort::connect()
{   
    return _cp->isOpen();
}

bool RS232COMPort::writeBytes(std::vector<unsigned char> bytes)
{
    _cp->flush();
    size_t ret = _cp->write(bytes);

	if(ret >= 0)
		return true;
	return false;
}

std::vector<unsigned char> RS232COMPort::readBytes()
{
	static unsigned char buf[4096];
	
	std::vector<unsigned char> bytes;
	
	int n = 0;
	do{
		SLEEP(100);
        n = _cp->read(buf, 4095);

		for(int i = 0; i < n; ++i){
			bytes.push_back(buf[i]);
		}
	}while( n != 0 );
	
	return bytes;
}

std::vector<unsigned char> RS232COMPort::readBytes(unsigned int nBytes)
{
	static unsigned char buf[4096];
	
	std::vector<unsigned char> bytes;
	
	int n = 0, retries = 10;
	while(bytes.size() != nBytes && n < retries){
        int bytesRcvd = _cp->read(buf, nBytes - bytes.size());
		for(int i = 0; i < bytesRcvd; ++i){
			bytes.push_back(buf[i]);
		}
		
		if(bytes.size() != nBytes){
			SLEEP(20);
		}
		
		n++;
	}
	
	return bytes;
}

void RS232COMPort::flushRecvBuffer()
{
    _cp->flush();
}