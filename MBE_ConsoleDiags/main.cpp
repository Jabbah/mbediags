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

#include <stdio.h>
#include <memory>
#include <iostream>
#include <iomanip>
#include <string>

#include "RS232COMPort.h"
#include "MBEDiagComms.h"

#ifdef _WIN32
#include <Windows.h>
void cls(  )
{
   HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
   COORD coordScreen = { 0, 0 };    // home for the cursor 
   DWORD cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi; 
   DWORD dwConSize;

	// Get the number of character cells in the current buffer. 
   if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
   {
      return;
   }

   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   // Fill the entire screen with blanks.
   if( !FillConsoleOutputCharacter( hConsole,        // Handle to console screen buffer 
                                    (TCHAR) ' ',     // Character to write to the buffer
                                    dwConSize,       // Number of cells to write 
                                    coordScreen,     // Coordinates of first cell 
                                    &cCharsWritten ))// Receive number of characters written
   {
      return;
   }

   // Get the current text attribute.
   if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
   {
      return;
   }

   // Set the buffer's attributes accordingly.
   if( !FillConsoleOutputAttribute( hConsole,         // Handle to console screen buffer 
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute 
                                    coordScreen,      // Coordinates of first cell 
                                    &cCharsWritten )) // Receive number of characters written
   {
      return;
   }

   // Put the cursor at its home coordinates.
   SetConsoleCursorPosition( hConsole, coordScreen );
}
#else
#include <unistd.h>
void cls(  )
{
}
#endif

int main(int argc, char **argv)
{
	RS232COMPort::enumerate_ports();
	std::string comPortName;
	std::cout << "Enter COM port number:";
	std::cin >> comPortName;
	std::shared_ptr<RS232COMPort> comPort = std::shared_ptr<RS232COMPort>(new RS232COMPort(comPortName, 4800));
	if(!comPort->connect())
	{
		std::cout << "Couldn't connect to COM port. Exiting..." << std::endl;
		return 0;
	}
	
	MBEDiagComms diags;
	diags.setCOMInterface(comPort);
	
	std::cout << "ECU ID: " << diags.getECUId() << std::endl;
	bool exit = false;
	
	while(!exit)
	{
		std::cout << "data: (r)eal time, (s)tatus flags, (l)ogged items, (a)daptives1, a(d)aptives2" << std::endl;
		std::cout << "reset: (f)ault codes, adaptive (m)aps, log (h)istory, (t)hrottle" << std::endl;
		std::cout << "e(x)it" << std::endl;
		
		std::string input;
		std::cin >> input;
		
		if(input == "r"){
			while(GetKeyState(VK_ESCAPE) >= 0)
			{
			#ifdef _WIN32
				Sleep(100);
			#else
				usleep(1000000);  /* sleep for 10 milliSeconds */
			#endif
			
				std::list<RTValue> items;
				for (int i = 0; i < static_cast<int>(RTValue::Last); ++i)
					items.push_back(static_cast<RTValue>(i));
				
				std::map<RTValue, boost::variant<int, double> > values = diags.getRealTimeValues(items);
				
				cls();
				std::cout << " RPM=\t\t" << std::setfill (' ') << std::setw (4) << std::fixed << std::dec << values[RTValue::Speed];
				std::cout << " \t\tBatt=\t\t" << std::setfill (' ') << std::setw (6) << std::fixed << std::setprecision(2) << values[RTValue::Battery];
				std::cout << std::endl;
				std::cout << " Adaptive1=\t" << std::setfill (' ') << std::setw (5) << std::fixed << std::setprecision(1) << values[RTValue::Adaptive1];
				std::cout << " \t\tAdaptive2=\t" << std::setfill (' ') << std::setw (5) << std::fixed << std::setprecision(1) << values[RTValue::Adaptive2];
				std::cout << std::endl;
				std::cout << " InjBank1=\t" << std::setfill (' ') << std::setw (7) << std::fixed << std::setprecision(3) << values[RTValue::Inj1357];
				std::cout << " \tInjBank2=\t" << std::setfill (' ') << std::setw (7) << std::fixed << std::setprecision(3) << values[RTValue::Inj2468];
				std::cout << std::endl;
				std::cout << " Lambda1=\t" << std::setfill (' ') << std::setw (6) << std::fixed << std::setprecision(2) << values[RTValue::Lambda1];
				std::cout << " \t\tLambda2=\t" << std::setfill (' ') << std::setw (6) << std::fixed << std::setprecision(2) << values[RTValue::Lambda2];
				std::cout << std::endl;
				std::cout << " Throttle1=\t" << std::setfill (' ') << std::setw (5) << std::fixed << std::setprecision(1) << values[RTValue::Throttle1];
				std::cout << " \t\tThrottle2=\t" << std::setfill (' ') << std::setw (5) << std::fixed << std::setprecision(1) << values[RTValue::Throttle2];
				std::cout << std::endl;
				std::cout << " Baro=\t\t" << std::setfill (' ') << std::setw (4) << std::fixed << std::setprecision(1) << values[RTValue::Baro];
				std::cout << " \t\tIgnition=\t" << std::setfill (' ') << std::setw (4) << std::fixed << std::setprecision(1) << values[RTValue::Ignition];
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}else if(input == "s"){
			while(GetKeyState(VK_ESCAPE) >= 0)
			{
			#ifdef _WIN32
				Sleep(100);
			#else
				usleep(1000000);  /* sleep for 10 milliSeconds */
			#endif
			
				std::list<RTStatus> items;
				for (int i = 0; i < static_cast<int>(RTStatus::Last); ++i)
					items.push_back(static_cast<RTStatus>(i));

				std::map<RTStatus, bool > values = diags.getRealTimeStatus(items);
				
				cls();
				std::cout << " Fan1=" << std::boolalpha << values[RTStatus::Fan1] << std::endl;
				std::cout << " Fan2=" << std::boolalpha << values[RTStatus::Fan2] << std::endl;
				std::cout << " FuelPump=" << std::boolalpha << values[RTStatus::FuelPump] << std::endl;
				std::cout << " FuelCut=" << std::boolalpha << values[RTStatus::FuelCut] << std::endl;
				std::cout << " DThrottleStatus=" << std::boolalpha << values[RTStatus::DThrottle] << std::endl;
				std::cout << " DSpeedStatus=" << std::boolalpha << values[RTStatus::DSpeed] << std::endl;
				std::cout << " WaterTempCtrl=" << std::boolalpha << values[RTStatus::WaterTempCtrl] << std::endl;
				std::cout << " WarmupStatus=" << std::boolalpha << values[RTStatus::Warmup] << std::endl;
				std::cout << " LambdaStatus=" << std::boolalpha << values[RTStatus::Lambda] << std::endl;
				std::cout << " Lambda1Status=" << std::boolalpha << values[RTStatus::Lambda1] << std::endl;
				std::cout << " Lambda2Status=" << std::boolalpha << values[RTStatus::Lambda2] << std::endl;
				std::cout << std::endl;
				std::cout << " BattVoltage=" << std::boolalpha << values[RTStatus::BattVoltage] << std::endl;
				std::cout << " BaroStatus=" << std::boolalpha << values[RTStatus::Baro] << std::endl;
				std::cout << " AirTempStatus=" << std::boolalpha << values[RTStatus::AirTemp] << std::endl;
				std::cout << " WaterTempStatus=" << std::boolalpha << values[RTStatus::WaterTemp] << std::endl;
				std::cout << " Throttle1Status=" << std::boolalpha << values[RTStatus::Throttle1] << std::endl;
				std::cout << " Throttle2Status=" << std::boolalpha << values[RTStatus::Throttle2] << std::endl;
				std::cout << " CrankWiring=" << std::boolalpha << values[RTStatus::CrankWiring] << std::endl;
				std::cout << " CrankSensor=" << std::boolalpha << values[RTStatus::CrankSensor] << std::endl;
				std::cout << " AFR1=" << std::boolalpha << values[RTStatus::AFR1] << std::endl;
				std::cout << " AFR2=" << std::boolalpha << values[RTStatus::AFR2] << std::endl;
				std::cout << " Lambda1SensorStatus=" << std::boolalpha << values[RTStatus::Lambda1Sensor] << std::endl;
				std::cout << " Lambda2SensorStatus=" << std::boolalpha << values[RTStatus::Lambda2Sensor] << std::endl;
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		else if (input == "l") {
			while (GetKeyState(VK_ESCAPE) >= 0)
			{
#ifdef _WIN32
				Sleep(100);
#else
				usleep(1000000);  /* sleep for 10 milliSeconds */
#endif

				std::list<LogItem> items;
				for (int i = 0; i < static_cast<int>(LogItem::Last); ++i)
					items.push_back(static_cast<LogItem>(i));

				std::map<LogItem, bool > values = diags.getLoggedItems(items);

				cls();
				std::cout << " AirTempSensor=" << std::boolalpha << values[LogItem::AirTempSensor] << std::endl;
				std::cout << " WaterTempSensor=" << std::boolalpha << values[LogItem::WaterTempSensor] << std::endl;
				std::cout << " BaroSensor=" << std::boolalpha << values[LogItem::BaroSensor] << std::endl;
				std::cout << " BattVoltage=" << std::boolalpha << values[LogItem::BattVoltage] << std::endl;
				std::cout << " LambdaSensor1=" << std::boolalpha << values[LogItem::LambdaSensor1] << std::endl;
				std::cout << " LambdaSensor2=" << std::boolalpha << values[LogItem::LambdaSensor2] << std::endl;
				std::cout << " ThrottlePot1=" << std::boolalpha << values[LogItem::ThrottlePot1] << std::endl;
				std::cout << " ThrottlePot2=" << std::boolalpha << values[LogItem::ThrottlePot2] << std::endl;
				std::cout << std::endl;
			}
			std::cout << std::endl;

		}else if(input == "a"){
			std::cout << "Adaptives1:" << std::endl;
			
			std::vector<unsigned char> bytes = diags.getAdaptives1();
			std::vector<double> adaptives = diags.decodeAdaptives(bytes);
			int j = 0;
			for(std::vector<double>::const_iterator i = adaptives.begin(); i != adaptives.end(); ++i, ++j){
				if(j % 16 == 0)
					std::cout << std::endl;
				std::cout << std::fixed << std::setprecision(1) << *i << ' ';
			}
				
			std::cout << std::endl;
			
		}
		else if (input == "d") {
			std::cout << "Adaptives2:" << std::endl;

			std::vector<unsigned char> bytes = diags.getAdaptives2();
			std::vector<double> adaptives = diags.decodeAdaptives(bytes);
			int j = 0;
			for (std::vector<double>::const_iterator i = adaptives.begin(); i != adaptives.end(); ++i, ++j) {
				if (j % 16 == 0)
					std::cout << std::endl;
				std::cout << std::fixed << std::setprecision(1) << *i << ' ';
			}

			std::cout << std::endl;
		}
		else if (input == "f") {
			diags.resetFaultCodes();
			std::cout << "Reset fault codes" << std::endl;
		}
		else if (input == "m") {
			diags.resetAdaptiveMaps();
			std::cout << "Reset fault codes" << std::endl;
		}
		else if (input == "h") {
			diags.resetLogHistory();
			std::cout << "Reset fault codes" << std::endl;
		}
		else if (input == "t") {
			diags.resetThrottle();
			std::cout << "Reset fault codes" << std::endl;
		}
		else if(input == "x"){
			exit = true;
		}
	}
	return 0;
}
