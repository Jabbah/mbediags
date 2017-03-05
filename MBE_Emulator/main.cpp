#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"

using namespace std;

int comPort;

double dRPM = 0;
double dBattVolts = 13.18;
double oilPsi = 45;
double dAirTemp = 18.3;
double dWaterTemp = 90.5;
double dBarom = 984;
double dIgn = -20.2;

double dThrottle1 = 16.9;
double dThrottle2 = 17.6;
double dLambda1 = 0.92;
double dLambda2 = 0.98;
double dAdaptive1 = 0.4;
double dAdaptive2 = 1.2;
double dInjTime1 = 2.384;
double dInjTime2 = 2.438;

bool bRadFan1 = false;
bool bRadFan2 = true;
bool bFuelPump = true;
bool bFuelCut = false;
bool bWaterTemp = false;
bool bLambda = false;
bool bDThrottle = true;
bool bDSpeed = true;
bool bWarmUp = false;
bool bLambda1Status = false;
bool bLambda2Status = true;

bool bWaterTempStatus = false;
bool bAirTempStatus = true;
bool bBaroPressStatus = false;
bool bBattVoltStatus = false;
bool bCranksSensStatus = false;
bool bCrankWiringStatus = true;
bool bAFRBank1Status = false;
bool bAFRBank2Status = true;
bool bLambda1SensorStatus = true;
bool bLambda2SensorStatus = false;
bool bThrottlePot1Status = false;
bool bThrottlePot2Status = true;


vector<unsigned char> getStatusBits1()
{
	unsigned char b = 0x10;
	if (bRadFan1) b |= 0x80;
	if (bRadFan2) b |= 0x02;
	if (bFuelPump) b |= 0x01;
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getStatusBits2()
{
	unsigned char b = 0x0;
	if (!bLambda1Status) b |= 0x01;
	if (!bLambda2Status) b |= 0x02;
	if (!bFuelCut) b |= 0x04;
	if (!bWaterTemp) b |= 0x08;
	if (!bDThrottle) b |= 0x10;
	if (!bDSpeed) b |= 0x20;
	if (!bWarmUp) b |= 0x40;
	if (!bLambda) b |= 0x80;
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getStatusBits3()
{
	unsigned char b = 0x0;
	if (bBaroPressStatus) b |= 0x01;
	if (bBattVoltStatus) b |= 0x02;
	if (bWaterTempStatus) b |= 0x04;
	if (bThrottlePot2Status) b |= 0x08;
	if (bLambda1SensorStatus) b |= 0x10;
	if (bThrottlePot1Status) b |= 0x20;
	if (bLambda2SensorStatus) b |= 0x40;
	if (bAirTempStatus) b |= 0x80;
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getStatusBits4()
{
	unsigned char b = 0x01;
	if (bCranksSensStatus) b |= 0x02;
	if (bAFRBank1Status) b |= 0x04;
	if (bAFRBank2Status) b |= 0x08;
	if (bCrankWiringStatus) b |= 0x10;
	return vector<unsigned char>(1, b);
}

unsigned char scaleToByte(double value, double min, double max){
	return (unsigned char)(((value - min) / ((max - min) / 255.0)) + 0.5);
}

vector<unsigned char> getRPM()
{
	unsigned char x250 = (unsigned char)((int)dRPM / 250);
	unsigned char x1 = scaleToByte(fmod(dRPM, 250.0), 0, 249);
	dRPM += 43.0;
	if(dRPM > 7500)
		dRPM = 0.0;
	vector<unsigned char> bytes;
	bytes.push_back(x1);
	bytes.push_back(x250);
	return bytes;
}

vector<unsigned char> getInjBank1()
{
	unsigned char hiInj = (unsigned char)(dInjTime1 / 0.512);
	unsigned char lowInj = scaleToByte(fmod(dInjTime1, 0.512), 0, 0.511);
	vector<unsigned char> bytes;
	bytes.push_back(lowInj);
	bytes.push_back(hiInj);
	return bytes;
}

vector<unsigned char> getInjBank2()
{
	unsigned char hiInj = (unsigned char)(dInjTime2 / 0.512);
	unsigned char lowInj = scaleToByte(fmod(dInjTime2, 0.512), 0, 0.511);
	vector<unsigned char> bytes;
	bytes.push_back(lowInj);
	bytes.push_back(hiInj);
	return bytes;
}

vector<unsigned char> getBaro()
{
	unsigned char b = (unsigned char)((dBarom+0.5) / (1063.0 / 255.0));
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getLambda1()
{
	unsigned char b = (unsigned char)((dLambda1 / (2.5 / 255.0)) + 0.5);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getLambda2()
{
	unsigned char b = (unsigned char)((dLambda2 / (2.5 / 255.0)) + 0.5);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getAdaptive1()
{
	unsigned char b = (unsigned char)(((dAdaptive1 + 100.0) / (200.0 / 255.0)) + 0.5);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getAdaptive2()
{
	unsigned char b = (unsigned char)(((dAdaptive2 + 100.0) / (200.0 / 255.0)) + 0.5);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getThrottle1()
{
	unsigned char b = (unsigned char)((dThrottle1 - 9.5) / ((100.0 - 9.5) / 255.0) + 0.5);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getThrottle2()
{
	unsigned char b = (unsigned char)((dThrottle2 - 9.5) / ((100.0 - 9.5) / 255.0) + 0.5);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getBattVolts()
{
	unsigned char b = scaleToByte(dBattVolts, 0.0, 20.0);
	dBattVolts += 0.1;
	if(dBattVolts > 20.05)
		dBattVolts = 0;
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getOilPsi()
{
	unsigned char b = (unsigned char)(oilPsi / (160.0 / 255.0) + 0.01);
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getAirTemp()
{
	unsigned char b = (unsigned char)((dAirTemp + 30.05) / (160.0 / 255.0));
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getWaterTemp()
{
	unsigned char b = (unsigned char)((dWaterTemp + 30.05) / (160.0 / 255.0));
	return vector<unsigned char>(1, b);
}

vector<unsigned char> getIgn()
{
	unsigned char b = (unsigned char)((dIgn + 60.1) / (60.0 / 255.0));
	return vector<unsigned char>(1, b);
}


void logData(std::string data)
{
	std::cout << data.c_str() << std::endl;
}

vector<unsigned char> getAdaptiveMap(int n);

void handleSerialData(int b)
{	
	static int lastByte = 0;
	static bool handleMapReq = false;
	static std::vector<int> mapReq;
		
	vector<unsigned char> response;
	
	std::cout << "0x" << std::hex << b << std::endl;

	if(handleMapReq){
		mapReq.push_back(b);
		
		if(b == 0x0D)
		{
			int sizeMapReq = mapReq.size();
			
			handleMapReq = false;
			
			int addrOffset = sizeMapReq - 10;
			
			if(mapReq[addrOffset] == 0x38){
				
				if(mapReq[addrOffset+1] == 0x30){//08000
					response = std::move(getAdaptiveMap(1));
				}else if(mapReq[addrOffset+1] == 0x34){//08400
					response = std::move(getAdaptiveMap(2));
				}else{
					{
						std::cout << "Unkown Map Req -";
						for(unsigned i=0; i < mapReq.size(); i++)
						{
							std::cout << " x" << std::hex << std::uppercase << (int)mapReq[i];
						}
						std::cout << std::endl;
					}
				}
			}
		}
		
		if(response.empty())
			return;
	}
		
	if(response.empty())
	switch (b)
	{
		case 0x0D:
		{
			//{ 0x23, 0x39, 0x34, 0x31, 0x33, 0x30, 0x31, 0x31, 0x36 }
			//{ 0x23, 0x39, 0x34, 0x31, 0x31, 0x30, 0x30, 0x33, 0x35 };
			//{ 0x23, 0x39, 0x34, 0x31, 0x31, 0x30, 0x30, 0x33, 0x34 };
			vector<unsigned char> ecuId{ 0x23, 0x39, 0x34, 0x31, 0x31, 0x30, 0x30, 0x33, 0x34 };
			response = std::move(ecuId);
			break;
		}
		case 0x7C:
		case 0x7E:
		case 0x76:
			lastByte = b;
			break;
		case 0xC3:
			switch (lastByte)
			{
				case 0x7C:
					response = getRPM();
					break;
				case 0x7E:
					response = getInjBank1();
					break;
				case 0x76:
					response = getInjBank2();
					break;
			}
			break;
		case 0x50:
			response = getBaro();
			break;
		case 0x49:
			response = getLambda1();
			break;
		case 0x4A:
			response = getLambda2();
			break;
		case 0xA7:
			response = getBattVolts();
			break;
		case 0x87:
			response = getIgn();
			break;
		case 0x9A:
			response = getOilPsi();
			break;
		case 0x9C:
			response = getThrottle1();
			break;
		case 0x94:
			response = getThrottle2();
			break;
		case 0xC0:
			response = getWaterTemp();
			break;
		case 0xBF:
			response = getAirTemp();
			break;
		case 0xEF:
			response = getAdaptive1();
			break;
		case 0xF1:
			response = getAdaptive2();
			break;
		case 0x58:
			response = getStatusBits1();
			break;
		case 0xCA:
			response = getStatusBits2();
			break;
		case 0xD3:
			response = getStatusBits3();
			break;
		case 0xFE:
			response = getStatusBits4();
			break;
		case 0xFF:
			//response = vector<unsigned char>{0x01}; // Baro Press
            //response = vector<unsigned char>{0x02}; // Batt voltage
            //response = vector<unsigned char>{0x04}; // Water Temp
            //response = vector<unsigned char>{0x08}; // Throttle Pot 2
            //response = vector<unsigned char>{0x10}; // Lambda 1 sensor
            //response = vector<unsigned char>{0x20}; // Throttle Pot 1
            //response = vector<unsigned char>{0x40}; // Lambda 2 sensor
            //response = vector<unsigned char>{0x80}; // Air Temp
            response = vector<unsigned char>{0xFF}; // All
			break;
		case 0x03:
			logData("Resetting Fault Codes");
			break;
		case 0x05:
			logData("Resetting Adaptive Maps");
			break;
		case 0x09:
			logData("Resetting Log History");
			break;
		case 0x0C:
			logData("Resetting Throttle");
			break;
		case 0x06:
			std::cout << "Getting map..." << std::endl;
			handleMapReq = true;
			mapReq.clear();
			response = vector<unsigned char>{ 0x42 };
			break;
		default:
			std::cout << "Unhandled - 0x" << std::hex << b << std::endl;
			break;
	}

	if (!response.empty())
	{
		if(RS232_SendBuf(comPort, response.data(), response.size()) == -1)
			std::cout << "Error writing to COM port" << std::endl;
	}
}

vector<unsigned char> getAdaptiveMap(int n)
{
	std::ifstream inputStr;
	if(n == 1)
		inputStr.open("adaptives1.txt");
	else
		inputStr.open("adaptives2.txt");
	
	std::vector<unsigned char> adapBytes;
	for(int n; inputStr >> std::hex >> n; )
		adapBytes.push_back(static_cast<unsigned char>(n));
	
	inputStr.close();
	return adapBytes;
}

int main()
{
  int i, n, bdrate=4800;
  
  comPort = 4; // 0 based so equivalent to COM5

  unsigned char buf[4096];

  char mode[]={'8','N','1',0};


  if(RS232_OpenComport(comPort, bdrate, mode))
  {
    printf("Can not open comport\n");

    return(0);
  }

  while(1)
  {
    n = RS232_PollComport(comPort, buf, 4096);
	
    if(n > 0)
    {
      for(i=0; i < n; i++)
      {
          handleSerialData(buf[i]);
      }
    }

#ifdef _WIN32
    Sleep(10);
#else
    usleep(10000);  /* sleep for 10 milliSeconds */
#endif
  }

  return(0);
}

