# mbediags
Utility to access MBE ECU settings as used in some TVRs.

MBE_ConsoleDiags is a simple console application to interface with the TVR Cerbera MBE ECU, allowing the reading of sensor values, adaptive maps, errors and logs. 

The serial interface is provided by the wjwwood serial library: https://github.com/wjwwood/serial

Boost is required for building. The project is setup to use the env variable BOOST_1_57_INC to find the boost includes.

Solution and project files are included for VS 2015.



MBE_Emulator is another console app providing a test harness for the console diags. It is not guaranteed to be the same as connecting to an ECU but does provide similar communication to the ECU in my Cerbera as far as I can tell and it does work with the official TVR software. To provide a virtual COM port link use com0com or similar http://com0com.sourceforge.net/ The Emulator is hard coded to use COM5 currently. 

There are two sample adaptive map files provided with the emulator to provide adaptive maps to the diags. These should be put in the same dir as the executable.
