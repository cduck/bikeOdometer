#include <L3G.h>
#include <mbed.h>

//#include "math.h"

//Code updated to work for mbed. Not fully working yet, need to mess around with values

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define D20_SA0_HIGH_ADDRESS      0b11010110 // also applies to D20H
#define D20_SA0_LOW_ADDRESS       0b11010100 // also applies to D20H
#define L3G4200D_SA0_HIGH_ADDRESS 0b11010010
#define L3G4200D_SA0_LOW_ADDRESS  0b11010000

#define TEST_REG_ERROR -1

#define D20H_WHO_ID     0xD7
#define D20_WHO_ID      0xD4
#define L3G4200D_WHO_ID 0xD3

// Constructors ////////////////////////////////////////////////////////////////

L3G::L3G(I2C *i2c, Serial *pc)
{
  _device = device_auto;

  _i2c = i2c;
  _pc = pc;

  io_timeout = 0;  // 0 = no timeout
  did_timeout = false;
}


// Public Methods //////////////////////////////////////////////////////////////

// Did a timeout occur in read() since the last call to timeoutOccurred()?
bool L3G::timeoutOccurred()
{
  bool tmp = did_timeout;
  did_timeout = false;
  return tmp;
}

void L3G::setTimeout(unsigned int timeout)
{
  io_timeout = timeout;
}

unsigned int L3G::getTimeout()
{
  return io_timeout;
}

bool L3G::init(deviceType device, sa0State sa0)
{
  int id;
  
  // perform auto-detection unless device type and SA0 state were both specified
  if (device == device_auto || sa0 == sa0_auto)
  {
    // check for L3GD20H, D20 if device is unidentified or was specified to be one of these types
    if (device == device_auto || device == device_D20H || device == device_D20)
    {
      // check SA0 high address unless SA0 was specified to be low
      if (sa0 != sa0_low && (id = testReg(D20_SA0_HIGH_ADDRESS, WHO_AM_I)) != TEST_REG_ERROR)
      {
        // device responds to address 1101011; it's a D20H or D20 with SA0 high     
        sa0 = sa0_high;
        if (device == device_auto)
        {
          // use ID from WHO_AM_I register to determine device type
          device = (id == D20H_WHO_ID) ? device_D20H : device_D20;
        }
      }
      // check SA0 low address unless SA0 was specified to be high
      else if (sa0 != sa0_high && (id = testReg(D20_SA0_LOW_ADDRESS, WHO_AM_I)) != TEST_REG_ERROR)
      {
        // device responds to address 1101010; it's a D20H or D20 with SA0 low      
        sa0 = sa0_low;
        if (device == device_auto)
        {
          // use ID from WHO_AM_I register to determine device type
          device = (id == D20H_WHO_ID) ? device_D20H : device_D20;
        }
      }
    }
    
    // check for L3G4200D if device is still unidentified or was specified to be this type
    if (device == device_auto || device == device_4200D)
    {
      if (sa0 != sa0_low && testReg(L3G4200D_SA0_HIGH_ADDRESS, WHO_AM_I) == L3G4200D_WHO_ID)
      {
        // device responds to address 1101001; it's a 4200D with SA0 high
        device = device_4200D;
        sa0 = sa0_high;
      }
      else if (sa0 != sa0_high && testReg(L3G4200D_SA0_LOW_ADDRESS, WHO_AM_I) == L3G4200D_WHO_ID)
      {
        // device responds to address 1101000; it's a 4200D with SA0 low
        device = device_4200D;
        sa0 = sa0_low;
      }
    }
    
    // make sure device and SA0 were successfully detected; otherwise, indicate failure
    if (device == device_auto || sa0 == sa0_auto)
    {
      return false;
    }
  }
  
  _device = device;

  // set device address
  switch (device)
  {
    case device_D20H:
    case device_D20:
      address = (sa0 == sa0_high) ? D20_SA0_HIGH_ADDRESS : D20_SA0_LOW_ADDRESS;
      break;

    case device_4200D:
      address = (sa0 == sa0_high) ? L3G4200D_SA0_HIGH_ADDRESS : L3G4200D_SA0_LOW_ADDRESS;
      break;

    default:
      break;  
  }
  
  return true;
}


//Enables the L3G's gyro
void L3G::enableDefault(void)
{
  if (_device == device_D20H)
  {
    // 0x00 = 0b00000000
    // Low_ODR = 0 (low speed ODR disabled)
    writeReg(LOW_ODR, 0x00);
  }
  
  // 0x30 = 0b00110000
  // FS = 11 (+/- 2000 dps full scale)
  writeReg(CTRL_REG4, 0x30);
  
  // 0x6F = 0b01101111
  // DR = 01 (200 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
  writeReg(CTRL_REG1, 0x6F);

  // 0x20 = 0b00100000
  // FM = 000 for bypass mode
  writeReg(FIFO_CTRL, 0x00);

}

//Enables the L3G's gyro to use the FIFO
void L3G::enableFIFO(void)
{
  enableDefault(); //flushes buffer by turning on bypass mode
  if (_device == device_D20H)
  {
    // 0x00 = 0b00000000
    // Low_ODR = 0 (low speed ODR disabled)
    writeReg(LOW_ODR, 0x00);
  }
  
  // 0x30 = 0b00110000
  // FS = 11 (+/- 2000 dps full scale)
  writeReg(CTRL_REG4, 0x30);
  
  // 0x6F = 0b01101111
  // DR = 01 (200 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
  writeReg(CTRL_REG1, 0x6F);

  // Bypass mode by default, FIFO mode to queue upto 32 data
  // 0x40 = 0b01000000
  // FIFO_EN = 1 to turn on FIFO
  writeReg(CTRL_REG5, 0x40);
  // 0xC0 = 0b11000000
  // FM = 110 for Dynamic STREAM mode
  writeReg(FIFO_CTRL, 0xC0);
}

// Writes a gyro register
void L3G::writeReg(char reg, char value)
{
  char cmd[2];
  cmd[0] = reg;
  cmd[1] = value;
  _i2c->write(address, cmd, 2);

}

// Reads a gyro register
char L3G::readReg(char reg)
{
  char value;

  _i2c->write(address, &reg, 1, true);
  _i2c->read(address, &value, 1);

  return value;
}

// Reads the 3 gyro channels and stores them in vector g
void L3G::read()
{

  char c = OUT_X_L | (1 << 7);
  _i2c->write(address, &c, 1,true);

  char reading[6];
  _i2c->read(address, reading, 6);
  
  // combine high and low bytes
  g.x = (int16_t)((int16_t)reading[1] << 8 | reading[0]);
  g.y = (int16_t)((int16_t)reading[3] << 8 | reading[2]);
  g.z = (int16_t)((int16_t)reading[5] << 8 | reading[4]);

}

// Readings X amount of data, depending on how many are in FIFO (32 elements large)
void L3G::readFIFO()
{
  char FIFO_SRC_status = readReg(FIFO_SRC);
  int FSS = (FIFO_SRC_status & 0x1F); //number of elements in FIFO stack, max(FSS) = 31

  char c = OUT_X_L | (1 << 7);
  _i2c->write(address, &c, 1,true);
  
  char reading[6*(FSS+1)]; // 6 bytes per row in FIFO, 2 bytes per axis
  _i2c->read(address, reading, 6*(FSS+1));
  
  g.x = 0.0; g.y = 0.0; g.z = 0.0;
  for (int i=0; i <= FSS; i++){
    // combine high and low bytes and take an average
    g.x += (int16_t)((int16_t)reading[6*i+1] << 8 | reading[6*i]);
    g.y += (int16_t)((int16_t)reading[6*i+3] << 8 | reading[6*i+2]);
    g.z += (int16_t)((int16_t)reading[6*i+5] << 8 | reading[6*i+4]);
  }
  g.x = g.x/(float)(FSS+1);
  g.y = g.y/(float)(FSS+1);
  g.z = g.z/(float)(FSS+1);

  convertReadings();
}

void L3G::convertReadings()
{
  //returns readings in DPS
  float DEG_2_RAD = 3.14159265358979323846/180.0;
  float LSB_2_DPS = 70/1000.0;
  g.x = g.x*LSB_2_DPS*DEG_2_RAD;
  g.y = g.y*LSB_2_DPS*DEG_2_RAD;
  g.z = g.z*LSB_2_DPS*DEG_2_RAD;
}

void L3G::vector_normalize(vector<float> *a)
{
  float mag = sqrt(vector_dot(a,a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}

// Private Methods //////////////////////////////////////////////////////////////

int L3G::testReg(char address, char reg)
{
  char d = 1;

  if (_i2c->write(address, &reg, 1,true)!= 0 ) {
      return TEST_REG_ERROR;
  }
  
  
  if (_i2c->read(address, &d, 1) == 0) {
      return d;
  } else {
      return TEST_REG_ERROR;
  }

}
