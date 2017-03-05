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

#ifndef MBEDIAGCOMMS_H
#define MBEDIAGCOMMS_H

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "COMInterface.h"
#include "boost/variant.hpp"

enum class RTValue {
	Speed, Baro, Lambda1, Lambda2, Battery, Inj1357, Inj2468, Ignition,
	Throttle1, Throttle2, WaterTemp, AirTemp, Adaptive1, Adaptive2,
	Last
};
enum class RTStatus {
	FuelPump, Fan1, Fan2, BattVoltage, WaterTemp, AirTemp,
	Throttle1, Throttle2, Baro, Lambda1, Lambda2,
	CrankWiring, AFR1, AFR2, CrankSensor, FuelCut, WaterTempCtrl,
	Lambda1Sensor, Lambda2Sensor, DThrottle, DSpeed, Warmup, Lambda,
	Last

};
enum class LogItem {
	WaterTempSensor, AirTempSensor, BattVoltage, BaroSensor,
	ThrottlePot1, ThrottlePot2, LambdaSensor1, LambdaSensor2,
	Last
};

class MBEDiagComms
{
public:
	MBEDiagComms();
	~MBEDiagComms();

	void setCOMInterface(std::shared_ptr<COMInterface> interface);
	
	std::string getECUId();
	std::vector<double> decodeAdaptives(const std::vector<unsigned char> &data);
	std::vector<unsigned char> getAdaptives1();
	std::vector<unsigned char> getAdaptives2();
	std::map<RTValue, boost::variant<int, double> > getRealTimeValues(const std::list<RTValue> &items);
	std::map<RTStatus, bool >  getRealTimeStatus(const std::list<RTStatus> &items);
	std::map<LogItem, bool >  getLoggedItems(const std::list<LogItem> &items);

	bool resetFaultCodes();
	bool resetAdaptiveMaps();
	bool resetLogHistory();
	bool resetThrottle();
	
private:
	std::vector<unsigned char> getByteCode(RTValue item);
	std::vector<unsigned char> getByteCode(RTStatus item);
	std::vector<unsigned char> getByteCode(LogItem item);
	boost::variant<int, double> decodeValue(RTValue item, const std::vector<unsigned char> &bytes);
	bool decodeStatus(RTStatus item, const std::vector<unsigned char> &bytes);
	bool decodeLogItem(LogItem item, const std::vector<unsigned char> &bytes);
	bool validateAdaptives(const std::vector<unsigned char> &data, int startCheckValue);
	double decodeDouble(unsigned char byte, double min, double max);
	double shortToDouble(unsigned short value, double min, double max);
	int decodeInt(unsigned char byte, int min, int max);
	std::shared_ptr<COMInterface> comInterface;
};

#endif // MBEDIAGCOMMS_H
