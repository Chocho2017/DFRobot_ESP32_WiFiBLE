# DFRobot_ESP32_WiFiBLE
This library is based on ESP32 master BLE applications. It can connect 4 slave devices at the same time, and can connect different Bluetooth through setting different service ID. At the same time, an example of combining WiFi and BLE is also given.

## Installation
Download the directory "DFRobot_ESP32_WiFiBLE » and move it into the "libraries" directory inside your sketchbook directory, then restart the Arduino IDE. You will then see it listed under File->Examples->DFRobot_ESP32_WiFiBLE.

## Usage
The library is instantiated as an object with BLE provided to read and write data from BLE. 
  #include "DFRobot_ESP32_BLE.h"
  DFRobot_ESP32_BLE ble;
  
## void setService(uint16_t ser)
   Setting up peripheral service ID.

Example:

   ble.setService(0xdfb0);

0xdfb0 is the serive ID for Bluno, so you can connect Bluno.

## void setCharacteristic(uint16_t cha)
Setting up peripheral Characteristic.

Example:

   ble.setCharacteristic(0xdfb1);

0xdfb0 is the Characteristic number for Bluno, so you can connect Bluno.

## void setconnummax(uint8_t max)
Sets the number of connections from the machine

Example:

   ble.setconnummax(2);

The max conncect number is 2.

## void setconnectname0(String str)
Setting up peripheral's name,which serive will connected.

Example:

   ble.setconnectname0("Bluno0");


## void init()
Initalize ESP32 BLE device.

Example:

   ble.init();
   
## void begin()
Start running Bluetooth device

Example:

   ble.begin();

## String readdata(char *buf)
Read data from BLE.

Example:

   char dataName[15] = {'\0'};
   String bledata = ble.readdata(dataName);



		
