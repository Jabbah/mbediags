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

#ifndef COMINTERFACE_H
#define COMINTERFACE_H

#include <vector>

class COMInterface
{
public:
	virtual bool writeBytes(std::vector<unsigned char> bytes) = 0;
	virtual std::vector<unsigned char> readBytes() = 0;
	virtual std::vector<unsigned char> readBytes(unsigned int nBytes) = 0;
};

#endif // COMINTERFACE_H
