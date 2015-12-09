/****************************************************************
Core header file for BC118 module support

Derived from the work I did on the BC127 module, also by
BlueCreations. Of course, the BC118 is a lot simpler, but I want
the structure and concepts to remain the same.

This code is beerware; if you use it, please buy me (or any other
SparkFun employee) a cold beverage next time you run into one of
us at the local.

30 Oct 2014- Mike Hord, SparkFun Electronics

Code developed in Arduino 1.0.6, on an Arduino Pro 5V.
****************************************************************/

#ifndef BLEMate2_h
#define BLEMate2_h

#include "mbed.h"
#include <iostream>
#include <string>
#include "WString.h"


//#define USE_FLOW_CONTROL


#define byte char
#ifdef USE_FLOW_CONTROL
  #include "MTSSerialFlowControl.h"
  #define SerialType mts::MTSSerialFlowControl
#else
  #define SerialType Serial
#endif

class BLEMate2
{
  public:
    // Now, make a data type for function results.
    enum opResult {REMOTE_ERROR = -5, CONNECT_ERROR, INVALID_PARAM,
                 TIMEOUT_ERROR, MODULE_ERROR, DEFAULT_ERR, SUCCESS};
    
    BLEMate2(SerialType* sp);
    opResult reset();  
    opResult restore(); 
    opResult writeConfig(); 
    opResult connect(byte index);
    opResult connect(String address);
    opResult connectionState();
    opResult disconnect();
    opResult getAddress(byte index, String &address);
    byte     numAddresses();
    opResult sendData(String &dataBuffer);
    opResult sendData(char *dataBuffer, byte dataLen);
    opResult sendData(const char *dataBuffer);
    opResult BLECentral();
    opResult BLEPeripheral();
    opResult amCentral(bool &inCentralMode);
    opResult BLEAdvertise();
    opResult BLENoAdvertise();
    opResult BLEScan(unsigned int timeout);
    opResult setBaudRate(unsigned int newSpeed);
    opResult addressQuery(String &address);
    opResult stdGetParam(String command, String &param);
    opResult stdSetParam(String command, String param);
    opResult stdCmd(String command);
    uint32_t millis();
  private:
    BLEMate2();
    int _baudRate;
    String _addresses[5];
    byte _numAddresses;
    SerialType *_serialPort;
    opResult knownStart();
};

#undef byte

#endif


