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

#ifndef RS232COMPort_H
#define RS232COMPort_H

#include <memory>
#include "COMInterface.h"

namespace serial {
    class Serial;
}

class RS232COMPort : public COMInterface
{
	RS232COMPort(){}
public:
	RS232COMPort(std::string port, int baud);
	~RS232COMPort();

	bool connect();
	void disconnect();
	
	virtual bool writeBytes(std::vector<unsigned char> bytes);
	virtual std::vector<unsigned char> readBytes();
	virtual std::vector<unsigned char> readBytes(unsigned int nBytes);
    
    static void enumerate_ports();
private:
	void flushRecvBuffer();
    std::shared_ptr<serial::Serial> _cp;
};

#endif // RS232COMPort_H
