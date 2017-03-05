# mbediags
Utility to access MBE ECU settings as used in some TVRs.

This is a simple console application to interface with the TVR Cerbera MBE ECU, allowing the reading of sensor values, adaptive maps, errors and logs. 

The serial interface is provided by the wjwwood serial library: https://github.com/wjwwood/serial

Boost is required for building. The project is setup to use the env variable BOOST_1_57_INC to find the boost includes.

Solution and project files are included for VS 2015.
