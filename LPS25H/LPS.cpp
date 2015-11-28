#include <LPS.h>
#include <mbed.h> //to use mbed i2c library

// Defines ///////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
// MBED uses an 8bit address, so I left shifted these 7 bit addresses 
#define SA0_LOW_ADDRESS  0b10111000
#define SA0_HIGH_ADDRESS 0b10111010

#define TEST_REG_NACK -1

#define LPS331AP_WHO_ID 0xBB
#define LPS25H_WHO_ID   0xBD

// Constructors //////////////////////////////////////////////////////

LPS::LPS(I2C *i2c)
{
  _device = device_auto;
  
  // Pololu board pulls SA0 high, so default assumption is that it is
  // high
  address = SA0_HIGH_ADDRESS;
  _i2c = i2c;
}

// Public Methods ////////////////////////////////////////////////////

// sets or detects device type and slave address; returns bool indicating success
bool LPS::init(deviceType device, uint32_t sa0)
{
  if (!detectDeviceAndAddress(device, (sa0State)sa0))
    return false;
    
  switch (_device)
  {
    case device_25H:
      translated_regs[-INTERRUPT_CFG] = LPS25H_INTERRUPT_CFG;
      translated_regs[-INT_SOURCE]    = LPS25H_INT_SOURCE;
      translated_regs[-THS_P_L]       = LPS25H_THS_P_L;
      translated_regs[-THS_P_H]       = LPS25H_THS_P_H;
      return true;
      break;
      
    case device_331AP:
      translated_regs[-INTERRUPT_CFG] = LPS331AP_INTERRUPT_CFG;
      translated_regs[-INT_SOURCE]    = LPS331AP_INT_SOURCE;
      translated_regs[-THS_P_L]       = LPS331AP_THS_P_L;
      translated_regs[-THS_P_H]       = LPS331AP_THS_P_H;
      return true;
      break;

     default:
      return false;
      break; 
  }
}

// turns on sensor and enables continuous output
void LPS::enableDefault(void)
{
  if (_device == device_25H)
  {
    // // 0xB0 = 0b10110000
    // // PD = 1 (active mode);  ODR = 011 (12.5 Hz pressure & temperature output data rate)
    // writeReg(CTRL_REG1, 0xB0);
    
    // 0xC0 = 0b11000000
    // PD = 1 (active mode);  ODR = 100 (25 Hz pressure & temperature output data rate)
    writeReg(CTRL_REG1, 0xC0);  
    
  }
  else if (_device == device_331AP)
  {
    // 0xE0 = 0b11100000
    // PD = 1 (active mode);  ODR = 110 (12.5 Hz pressure & temperature output data rate)
    writeReg(CTRL_REG1, 0xE0);
  }
}

// turns on sensor and enables FIFO output
void LPS::enableFIFO(void)
{
  if (_device == device_25H)
  {    
    // 0xC0 = 0b11000000
    // PD = 1 (active mode);  ODR = 100 (25 Hz pressure & temperature output data rate)
    writeReg(CTRL_REG1, 0xC0);  
    
    // 0x40 = 0b01000000
    // FIFO_EN = 1 (enable FIFO)
    writeReg(CTRL_REG2, 0xC0); 

    // 0x40 = 0b01000000
    // F_MODE = 010 (STREAM mode)
    writeReg(FIFO_CTRL, 0x40);    
  }
  else if (_device == device_331AP)
  {
    // 0xE0 = 0b11100000
    // PD = 1 (active mode);  ODR = 110 (12.5 Hz pressure & temperature output data rate)
    writeReg(CTRL_REG1, 0xE0);
  }
}

// writes register
void LPS::writeReg(int reg, uint8_t value)
{
  // if dummy register address, look up actual translated address (based on device type)
  if (reg < 0)
  {
    reg = translated_regs[-reg];
  }
  char cmd[2];  
  cmd[0] = reg;
  cmd[1] = value;
  _i2c->write(address,cmd,2);
}

// reads pressure in millibars (mbar)/hectopascals (hPa)
float LPS::readPressureMillibars(void)
{
  return (float)readPressureRaw() / 4096;
}

// reads pressure in inches of mercury (inHg)
float LPS::readPressureInchesHg(void)
{
  return (float)readPressureRaw() / 138706.5;
}

// reads pressure and returns raw 24-bit sensor output
int32_t LPS::readPressureRaw(void)
{
  // char array is filled:
  // LSB first .... MSB last
  char cmd[1];
  // assert MSB to enable register address auto-increment
  cmd[0] = (PRESS_OUT_XL | (1 << 7));
  int32_t readingInt[1];
  char *reading = (char*)readingInt;
  _i2c->write(address, cmd, 1);
  _i2c->read(address, reading, 3);

  //carry the sign
  reading[3] = (reading[2] & 0x80) ? 0xFF : 0x00;
  return readingInt[0];
}

// reads temperature in degrees C
float LPS::readTemperatureC()
{
  return 42.5 + (float)readTemperatureRaw() / 480;
}

// reads temperature in degrees F
float LPS::readTemperatureF()
{
  return 108.5 + (float)readTemperatureRaw() / 480 * 1.8;
}

// reads temperature and returns raw 16-bit sensor output
int16_t LPS::readTemperatureRaw()
{
  // char array is filled:
  // LSB first .... MSB last

  char cmd[1];
  // assert MSB to enable register address auto-increment
  cmd[0] = (TEMP_OUT_L | (1 << 7));
  int16_t readingInt[1];
  char *reading = (char*)readingInt;
  _i2c->write(address, cmd, 1);
  _i2c->read(address, reading, 2);  
  
  return readingInt[0];  
}

// converts pressure in mbar to altitude in meters, using 1976 US
// Standard Atmosphere model (note that this formula only applies to a
// height of 11 km, or about 36000 ft)
//  If altimeter setting (QNH, barometric pressure adjusted to sea
//  level) is given, this function returns an indicated altitude
//  compensated for actual regional pressure; otherwise, it returns
//  the pressure altitude above the standard pressure level of 1013.25
//  mbar or 29.9213 inHg
float LPS::pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar)
{
  return (1 - pow(pressure_mbar / altimeter_setting_mbar, 0.190263)) * 44330.8;
}

// converts pressure in inHg to altitude in feet; see notes above
float LPS::pressureToAltitudeFeet(float pressure_inHg, float altimeter_setting_inHg)
{
  return (1 - pow(pressure_inHg / altimeter_setting_inHg, 0.190263)) * 145442;
}

// Private Methods ///////////////////////////////////////////////////

bool LPS::detectDeviceAndAddress(deviceType device, sa0State sa0)
{
  if (sa0 == sa0_auto || sa0 == sa0_high)
  {
    address = SA0_HIGH_ADDRESS;
    if (detectDevice(device)) return true;
  }
  if (sa0 == sa0_auto || sa0 == sa0_low)
  {
    address = SA0_LOW_ADDRESS;
    if (detectDevice(device)) return true;
  }

  return false;
}

bool LPS::detectDevice(deviceType device)
{
  int id = testWhoAmI(address);
  
  if ((device == device_auto || device == device_25H) && id == LPS25H_WHO_ID)
  {
    _device = device_25H;
    return true;
  }
  if ((device == device_auto || device == device_331AP) && id == LPS331AP_WHO_ID)
  {
    _device = device_331AP;
    return true;
  }

  return false;
}

int LPS::testWhoAmI(int address)
{
  char reg[1];
  reg[0] = WHO_AM_I;
  char identity[1];
  _i2c->write(address, reg, 1);  

  if (_i2c->read(address, identity, 1) == 0){
      return *identity;
  }
  else {
    return TEST_REG_NACK;
  }

}