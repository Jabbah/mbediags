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

#include "MBEDiagComms.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

MBEDiagComms::MBEDiagComms()
{
}

MBEDiagComms::~MBEDiagComms()
{
}


void MBEDiagComms::setCOMInterface(shared_ptr<COMInterface> interface)
{
	comInterface = interface;
}

string MBEDiagComms::getECUId()
{
	vector<unsigned char> bytes;
	
	bytes.push_back(0x0D); 
	
	comInterface->writeBytes(bytes);
	bytes = comInterface->readBytes();
	
	return string(bytes.begin(), bytes.end());	
}

bool MBEDiagComms::resetFaultCodes()
{
	vector<unsigned char> bytes;

	bytes.push_back(0x03);

	comInterface->writeBytes(bytes);
	bytes = comInterface->readBytes();

	if (bytes.size())
		std::cout << "0x" << std::hex << bytes[0] << std::endl;

	return true;
}

bool MBEDiagComms::resetAdaptiveMaps()
{
	vector<unsigned char> bytes;

	bytes.push_back(0x05);

	comInterface->writeBytes(bytes);
	bytes = comInterface->readBytes();

	if (bytes.size())
		std::cout << "0x" << std::hex << bytes[0] << std::endl;

	return true;
}

bool MBEDiagComms::resetLogHistory()
{
	vector<unsigned char> bytes;

	bytes.push_back(0x09);

	comInterface->writeBytes(bytes);
	bytes = comInterface->readBytes();

	if (bytes.size())
		std::cout << "0x" << std::hex << bytes[0] << std::endl;

	return true;
}

bool MBEDiagComms::resetThrottle()
{
	vector<unsigned char> bytes;

	bytes.push_back(0x0C);

	comInterface->writeBytes(bytes);
	bytes = comInterface->readBytes();

	if(bytes.size())
		std::cout << "0x" << std::hex << bytes[0] << std::endl;

	return true;
}

vector<double> MBEDiagComms::decodeAdaptives(const vector<unsigned char> &data)
{
	vector<double> adaptives;
	
	vector<unsigned char>::const_iterator it = data.begin();
	for(int lineNum = 0; lineNum < 40 && it < data.end(); it+=61, lineNum++){
		
		if(lineNum == 40)
			continue;
			
		vector<unsigned char> line(it, it+61);
				
		for(int i = 9; i < 57; i+=4){
			stringstream byteStream;
			byteStream << line[i+2] << line[i+3] << line[i] << line[i+1];
			string byteStr = byteStream.str();
			int byteVal = strtol(byteStr.c_str(),0,16);
			double val = shortToDouble(byteVal, -100.0, 100.0);
			adaptives.push_back(val);
		}
		
	}
		
	return adaptives;
}

bool MBEDiagComms::validateAdaptives(const vector<unsigned char> &data, int startCheckValue)
{
	if(data.size() != 2453)
		return false;
	
	vector<unsigned char>::const_iterator it = data.begin();
	for(int lineNum = 0; lineNum < 40 && it < data.end(); it+=61, lineNum++){
		
		if(lineNum == 40)
			continue;
			
		vector<unsigned char> line(it, it+61);
				
		stringstream checksumStream;
		checksumStream << line[57] << line[58];
		string checkStr = checksumStream.str();
		int checkVal = strtol(checkStr.c_str(),0,16);
		
		stringstream addressStream;
		addressStream << line[4];
		string addressStr = addressStream.str();
		int addressVal = strtol(addressStr.c_str(),0,16);
		
		int checkSum = (startCheckValue - (0x18 * lineNum) - addressVal) & 0xFF;
		
		for(int i = 9; i < 57; i+=4){
			stringstream byteStream;
			byteStream << line[i+2] << line[i+3] << line[i] << line[i+1];
			string byteStr = byteStream.str();
			int byteVal = strtol(byteStr.c_str(),0,16);
			
			if(byteVal > 0x8000){
				checkSum = (checkSum - (((byteVal - 0x8000) & 0xFF) + (((byteVal - 0x8000) >> 8) & 0xFF))) & 0xFF;
			}else if(byteVal < 0x8000){
				checkSum = (checkSum + ((0x7FFF - byteVal) & 0xFF) + (((0x7FFF - byteVal) >> 8) & 0xFF) + 2) & 0xFF;
			}
		}
		
		if(checkVal != checkSum)
			return false;

	}
		
	return true;
}

vector<unsigned char> MBEDiagComms::getAdaptives1()
{	
	unsigned char request1[] = {0x06};
	unsigned char request2[] = {0x02};
	unsigned char request3[] = {0x07, 0x53, 0x45, 0x30, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x33, 0x42, 0x46, 0x0D};
	
	vector<unsigned char> bytes1 (request1, request1 + sizeof(request1) / sizeof(unsigned char) );
	vector<unsigned char> bytes2 (request2, request2 + sizeof(request2) / sizeof(unsigned char) );
	vector<unsigned char> bytes3 (request3, request3 + sizeof(request3) / sizeof(unsigned char) );
	
	comInterface->writeBytes(bytes1);
	bytes1 = comInterface->readBytes();
	cout << "First response: ";
	for(unsigned int i = 0; i < bytes1.size(); ++i)
		cout << "0x" << hex << (int)bytes1[i] << " ";
	
	cout << "\nSecond response: ";
	comInterface->writeBytes(bytes2);
	bytes2 = comInterface->readBytes();
	for(unsigned int i = 0; i < bytes2.size(); ++i)
		cout << "0x" << setfill ('0') << setw (2) << hex << (int)(bytes2[i]) << " ";		

	ofstream outfile;
	outfile.open("adaptives1Output.txt");

	cout << "\nThrid response: ";
	comInterface->writeBytes(bytes3);
	bytes3 = comInterface->readBytes();
	for(unsigned int i = 0; i < bytes3.size(); ++i)
		outfile << "0x" << setfill ('0') << setw (2) << hex << (int)(bytes3[i]) << " ";		

	outfile.close();
	
	bool validData = false;
	if(bytes3.size() > 0){
		validData = validateAdaptives(bytes3, 0x68);
		if(!validData){
			cout << "Failed checksum" << endl;
		}
	}
	
	return bytes3;	
}

vector<unsigned char> MBEDiagComms::getAdaptives2()
{
	unsigned char request1[] = {0x06};
	unsigned char request2[] = {0x02};
	unsigned char request3[] = {0x07, 0x53, 0x45, 0x30, 0x38, 0x34, 0x30, 0x30, 0x30, 0x38, 0x37, 0x42, 0x46, 0x0D};
	//unsigned char request3[] = {0x07, 0x53, 0x45, 0x30, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x33, 0x42, 0x46, 0x0D};
	
	vector<unsigned char> bytes1 (request1, request1 + sizeof(request1) / sizeof(unsigned char) );
	vector<unsigned char> bytes2 (request2, request2 + sizeof(request2) / sizeof(unsigned char) );
	vector<unsigned char> bytes3 (request3, request3 + sizeof(request3) / sizeof(unsigned char) );
	
	comInterface->writeBytes(bytes1);
	bytes1 = comInterface->readBytes();
	cout << "First response: ";
	for(unsigned int i = 0; i < bytes1.size(); ++i)
		cout << "0x" << hex << (int)bytes1[i] << " ";
	
	cout << "\nSecond response: ";
	comInterface->writeBytes(bytes2);
	bytes2 = comInterface->readBytes();
	for(unsigned int i = 0; i < bytes2.size(); ++i)
		cout << "0x" << setfill ('0') << setw (2) << hex << (int)(bytes2[i]) << " ";		

	ofstream outfile;
	outfile.open("adaptives2Output.txt");

	cout << "\nThrid response: ";
	comInterface->writeBytes(bytes3);
	bytes3 = comInterface->readBytes();
	for(unsigned int i = 0; i < bytes3.size(); ++i)
		outfile << "0x" << setfill ('0') << setw (2) << hex << (int)(bytes3[i]) << " ";		

	outfile.close();
	
	bool validData = false;
	if(bytes3.size() > 0){
		validData = validateAdaptives(bytes3, 0x68);
		if(!validData){
			cout << "Failed checksum" << endl;
		}
	}
	
	return bytes3;	
}

map<RTValue, boost::variant<int, double> > MBEDiagComms::getRealTimeValues(const list<RTValue> &items)
{
	map<RTValue, boost::variant<int, double> > values;
	
	vector<unsigned char> byteCodes;
	for(RTValue item : items)
	{
		vector<unsigned char> byteCode = getByteCode(item);
		byteCodes.insert(byteCodes.end(), byteCode.begin(), byteCode.end());
	}
	
	comInterface->writeBytes(byteCodes);
	
	for(RTValue item : items)
	{
		int numBytes = getByteCode(item).size();
		vector<unsigned char> byteValue = comInterface->readBytes(numBytes);
		values[item] = decodeValue(item, byteValue);
	}
	
	return values;
}

map<RTStatus, bool>  MBEDiagComms::getRealTimeStatus(const list<RTStatus> &items)
{
	map<RTStatus, bool> values;
	
	vector<unsigned char> byteCodes;
	for(RTStatus item : items)
	{
		vector<unsigned char> byteCode = getByteCode(item);
		byteCodes.push_back(byteCode[0]);
	}
	
	comInterface->writeBytes(byteCodes);
	
	for(RTStatus item : items)
	{
		vector<unsigned char> byteValue = comInterface->readBytes(1);
		values[item] = decodeStatus(item, byteValue);
	}
	
	return values;
}

std::map<LogItem, bool >  MBEDiagComms::getLoggedItems(const std::list<LogItem> &items)
{
	map<LogItem, bool> values;

	vector<unsigned char> byteCodes;
	for (LogItem item : items)
	{
		vector<unsigned char> byteCode = getByteCode(item);
		byteCodes.push_back(byteCode[0]);
	}

	comInterface->writeBytes(byteCodes);

	for (LogItem item : items)
	{
		vector<unsigned char> byteValue = comInterface->readBytes(1);
		values[item] = decodeLogItem(item, byteValue);
	}

	return values;
}

bool MBEDiagComms::decodeLogItem(LogItem item, const vector<unsigned char> &bytes)
{
	if (bytes.size() == 0)
		return false;

	bool status;

	switch (item) {
	case LogItem::BaroSensor:
	{
		status = (bytes[0] & 0x01) != 0;
		break;
	}
	case LogItem::BattVoltage:
	{
		status = (bytes[0] & 0x02) != 0;
		break;
	}
	case LogItem::WaterTempSensor:
	{
		status = (bytes[0] & 0x04) != 0;
		break;
	}
	case LogItem::ThrottlePot2:
	{
		status = (bytes[0] & 0x08) != 0;
		break;
	}
	case LogItem::LambdaSensor1:
	{
		status = (bytes[0] & 0x10) != 0;
		break;
	}
	case LogItem::ThrottlePot1:
	{
		status = (bytes[0] & 0x20) != 0;
		break;
	}
	case LogItem::LambdaSensor2:
	{
		status = (bytes[0] & 0x40) != 0;
		break;
	}
	case LogItem::AirTempSensor:
	{
		status = (bytes[0] & 0x80) != 0;
		break;
	}
	default:
		throw std::invalid_argument("LogItem type not found: " + std::to_string(static_cast<int>(item)));
		break;
	}

	return status;
}

bool MBEDiagComms::decodeStatus(RTStatus item, const vector<unsigned char> &bytes)
{
	if (bytes.size() == 0)
		return false;

	bool status;
	switch(item){
		case RTStatus::FuelPump:
		{
			status = (bytes[0] & 0x01) != 0;
			break;
		}
		case RTStatus::Fan1:
		{
			status = (bytes[0] & 0x80) != 0;
			break;
		}
		case RTStatus::Fan2:
		{
			status = (bytes[0] & 0x02) != 0;
			break;
		}
		case RTStatus::Lambda1:
		{
			status = (bytes[0] & 0x01) == 0;
			break;
		}
		case RTStatus::Lambda2:
		{
			status = (bytes[0] & 0x02) == 0;
			break;
		}
		case RTStatus::FuelCut:
		{
			status = (bytes[0] & 0x04) == 0;
			break;
		}
		case RTStatus::WaterTempCtrl:
		{
			status = (bytes[0] & 0x08) == 0;
			break;
		}
		case RTStatus::DThrottle:
		{
			status = (bytes[0] & 0x10) == 0;
			break;
		}
		case RTStatus::DSpeed:
		{
			status = (bytes[0] & 0x20) == 0;
			break;
		}
		case RTStatus::Warmup:
		{
			status = (bytes[0] & 0x40) == 0;
			break;
		}
		case RTStatus::Lambda:
		{
			status = (bytes[0] & 0x80) == 0;
			break;
		}
		case RTStatus::Baro:
		{
			status = (bytes[0] & 0x01) != 0;
			break;
		}
		case RTStatus::BattVoltage:
		{
			status = (bytes[0] & 0x02) != 0;
			break;
		}
		case RTStatus::WaterTemp:
		{
			status = (bytes[0] & 0x04) != 0;
			break;
		}
		case RTStatus::Throttle2:
		{
			status = (bytes[0] & 0x08) != 0;
			break;
		}
		case RTStatus::Lambda1Sensor:
		{
			status = (bytes[0] & 0x10) != 0;
			break;
		}
		case RTStatus::Throttle1:
		{
			status = (bytes[0] & 0x20) != 0;
			break;
		}
		case RTStatus::Lambda2Sensor:
		{
			status = (bytes[0] & 0x40) != 0;
			break;
		}
		case RTStatus::AirTemp:
		{
			status = (bytes[0] & 0x80) != 0;
			break;
		}
		case RTStatus::CrankSensor:
		{
			status = (bytes[0] & 0x02) != 0;
			break;
		}
		case RTStatus::AFR1:
		{
			status = (bytes[0] & 0x04) != 0;
			break;
		}
		case RTStatus::AFR2:
		{
			status = (bytes[0] & 0x08) != 0;
			break;
		}
		case RTStatus::CrankWiring:
		{
			status = (bytes[0] & 0x10) != 0;
			break;
		}
		default:
			throw std::invalid_argument("RTStatus type not found: " + std::to_string(static_cast<int>(item)));
			break;
	}
	return status;
}

boost::variant<int, double> MBEDiagComms::decodeValue(RTValue item, const vector<unsigned char> &bytes)
{
	boost::variant<int, double> value;

	if (bytes.size() == 0) {
		value = 0.0;
		return value;
	}

	switch(item){
		case RTValue::Speed:
		{
			int rpmMinor = decodeInt(bytes[0], 0, 249);
			int rpmMajor = bytes[1] * 250;
			value = rpmMajor + rpmMinor;
			break;
		}
		case RTValue::Inj1357:
		{
			double injMinor = decodeDouble(bytes[0], 0, 0.511);
			double injMajor = (double)bytes[1] * 0.512;
			value = injMajor + injMinor;
			break;
		}
		case RTValue::Inj2468:
		{
			double injMinor = decodeDouble(bytes[0], 0, 0.511);
			double injMajor = (double)bytes[1] * 0.512;
			value = injMajor + injMinor;
			break;
		}
		case RTValue::Lambda1:
		{
			value = decodeDouble(bytes[0], 0, 2.5);
			break;
		}
		case RTValue::Lambda2:
		{
			value = decodeDouble(bytes[0], 0, 2.5);
			break;
		}
		case RTValue::Throttle1:
		{
			value = decodeDouble(bytes[0], 9.48, 100);
			break;
		}
		case RTValue::Throttle2:
		{
			value = decodeDouble(bytes[0], 9.48, 100);
			break;
		}
		case RTValue::Adaptive1:
		{
			value = decodeDouble(bytes[0], -100, 100);
			break;
		}
		case RTValue::Adaptive2:
		{
			value = decodeDouble(bytes[0], -100, 100);
			break;
		}
		case RTValue::Battery:
		{
			value = decodeDouble(bytes[0], 0, 20);
			break;
		}
		case RTValue::Ignition:
		{
			value = decodeDouble(bytes[0], -60, 0);
			break;
		}
		case RTValue::Baro:
		{
			value = decodeInt(bytes[0], 0, 1063);
			break;
		}
		case RTValue::AirTemp:
		{
			value = decodeDouble(bytes[0], -30, 130);
			break;
		}
		case RTValue::WaterTemp:
		{
			value = decodeDouble(bytes[0], -30, 130);
			break;
		}
		default:
			throw std::invalid_argument("RTValue type not found: " + std::to_string(static_cast<int>(item)));
			break;
	}
	return value;
}

vector<unsigned char> MBEDiagComms::getByteCode(RTValue item)
{
	vector<unsigned char> byteCode;
	switch(item){
		case RTValue::Speed:
			byteCode.push_back(0x7C);
			byteCode.push_back(0xC3);
			break;
		case RTValue::Inj1357:
			byteCode.push_back(0x7E);
			byteCode.push_back(0xC3);
			break;
		case RTValue::Inj2468:
			byteCode.push_back(0x76);
			byteCode.push_back(0xC3);
			break;
		case RTValue::Lambda1:
			byteCode.push_back(0x49);
			break;
		case RTValue::Lambda2:
			byteCode.push_back(0x4A);
			break;
		case RTValue::Throttle1:
			byteCode.push_back(0x9C);
			break;
		case RTValue::Throttle2:
			byteCode.push_back(0x94);
			break;
		case RTValue::Adaptive1:
			byteCode.push_back(0xEF);
			break;
		case RTValue::Adaptive2:
			byteCode.push_back(0xF1);
			break;
		case RTValue::Battery:
			byteCode.push_back(0xA7);
			break;
		case RTValue::Baro:
			byteCode.push_back(0x50);
			break;
		case RTValue::Ignition:
			byteCode.push_back(0x87);
			break;
		case RTValue::AirTemp:
			byteCode.push_back(0xBF);
			break;
		case RTValue::WaterTemp:
			byteCode.push_back(0xC0);
			break;
		default:
			break;
	}
	
	return byteCode;
}

vector<unsigned char> MBEDiagComms::getByteCode(RTStatus item)
{
	vector<unsigned char> byteCode;
	switch(item){
		case RTStatus::FuelPump:
		case RTStatus::Fan1:
		case RTStatus::Fan2:
			byteCode.push_back(0x58);
			break;
		case RTStatus::Lambda1:
		case RTStatus::Lambda2:
		case RTStatus::FuelCut:
		case RTStatus::WaterTempCtrl:
		case RTStatus::DThrottle:
		case RTStatus::DSpeed:
		case RTStatus::Warmup:
		case RTStatus::Lambda:
			byteCode.push_back(0xCA);
			break;
		case RTStatus::Baro:
		case RTStatus::BattVoltage:
		case RTStatus::WaterTemp:
		case RTStatus::Throttle1:
		case RTStatus::Throttle2:
		case RTStatus::Lambda1Sensor:
		case RTStatus::Lambda2Sensor:
		case RTStatus::AirTemp:
			byteCode.push_back(0xD3);
			break;
		case RTStatus::CrankSensor:
		case RTStatus::AFR1:
		case RTStatus::AFR2:
		case RTStatus::CrankWiring:
			byteCode.push_back(0xFE);
			break;
		default:
			break;
	}
	
	return byteCode;
}

vector<unsigned char> MBEDiagComms::getByteCode(LogItem item)
{
	vector<unsigned char> byteCode;

	byteCode.push_back(0xFF);

	return byteCode;
}
	
double MBEDiagComms::decodeDouble(unsigned char byte, double min, double max)
{
	double steps = (double)byte;
	
	double range = max - min;
	
	double unscaled = (steps * range);
	
	double scaled = unscaled / 255.0;
	
	return min + scaled;
}

double MBEDiagComms::shortToDouble(unsigned short value, double min, double max)
{
	double steps = (double)value;
	
	double range = max - min;
	
	double unscaled = (steps * range);
	
	double scaled = unscaled / 65535.0;
	
	return min + scaled;
}

int MBEDiagComms::decodeInt(unsigned char byte, int min, int max)
{
	int steps = (int)byte;
	
	int range = max - min;
	
	int unscaled = (steps * range);
	
	double scaled = (double)unscaled / 255.0;
	double roundedUp = scaled + 0.5;
	
	return min + (int)roundedUp;
}
