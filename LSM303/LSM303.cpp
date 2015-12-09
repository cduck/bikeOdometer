#include <LSM303.h>
#include <mbed.h> // for i2c
#include <math.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define D_SA0_HIGH_ADDRESS                0b00111010
#define D_SA0_LOW_ADDRESS                 0b00111100
#define DLHC_DLM_DLH_MAG_ADDRESS          0b00111100
#define DLHC_DLM_DLH_ACC_SA0_HIGH_ADDRESS 0b00110010
#define DLM_DLH_ACC_SA0_LOW_ADDRESS       0b00110000

#define TEST_REG_ERROR -1

#define D_WHO_ID    0x49
#define DLM_WHO_ID  0x3C

// Constructors ////////////////////////////////////////////////////////////////

LSM303::LSM303(I2C *i2c, Serial *pc)
{
  /*
  These values lead to an assumed magnetometer bias of 0.
  Use the Calibrate example program to determine appropriate values
  for your particular unit. The Heading example demonstrates how to
  adjust these values in your own sketch.
  */
  m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};

  _device = device_auto;

  io_timeout = 0;  // 0 = no timeout
  did_timeout = false;
  _i2c = i2c;
  _pc = pc;
}

// Public Methods //////////////////////////////////////////////////////////////

// Did a timeout occur in readAcc(), readMag(), or read() since the last call to timeoutOccurred()?
bool LSM303::timeoutOccurred()
{
  bool tmp = did_timeout;
  did_timeout = false;
  return tmp;
}

void LSM303::setTimeout(unsigned int timeout)
{
  io_timeout = timeout;
}

unsigned int LSM303::getTimeout()
{
  return io_timeout;
}

bool LSM303::init(deviceType device, sa0State sa0)
{
  // perform auto-detection unless device type and SA0 state were both specified
  if (device == device_auto || sa0 == sa0_auto)
  {
    // check for LSM303D if device is unidentified or was specified to be this type
    if (device == device_auto || device == device_D)
    {
      // check SA0 high address unless SA0 was specified to be low
      if (sa0 != sa0_low && testReg(D_SA0_HIGH_ADDRESS, WHO_AM_I) == D_WHO_ID)
      {
        // device responds to address 0011101 with D ID; it's a D with SA0 high
        device = device_D;
        sa0 = sa0_high;
      }
      // check SA0 low address unless SA0 was specified to be high
      else if (sa0 != sa0_high && testReg(D_SA0_LOW_ADDRESS, WHO_AM_I) == D_WHO_ID)
      {
        // device responds to address 0011110 with D ID; it's a D with SA0 low
        device = device_D;
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


  acc_address = mag_address = (sa0 == sa0_high) ? D_SA0_HIGH_ADDRESS : D_SA0_LOW_ADDRESS;
  translated_regs[-OUT_X_L_M] = D_OUT_X_L_M;
  translated_regs[-OUT_X_H_M] = D_OUT_X_H_M;
  translated_regs[-OUT_Y_L_M] = D_OUT_Y_L_M;
  translated_regs[-OUT_Y_H_M] = D_OUT_Y_H_M;
  translated_regs[-OUT_Z_L_M] = D_OUT_Z_L_M;
  translated_regs[-OUT_Z_H_M] = D_OUT_Z_H_M;

  
  return true;
}

/*
Enables the LSM303's accelerometer and magnetometer. Also:
- Sets sensor full scales (gain) to default power-on values, which are
  +/- 2 g for accelerometer and +/- 1.3 gauss for magnetometer
  (+/- 4 gauss on LSM303D).
- Selects 50 Hz ODR (output data rate) for accelerometer and 7.5 Hz
  ODR for magnetometer (6.25 Hz on LSM303D). (These are the ODR
  settings for which the electrical characteristics are specified in
  the datasheets.)
- Enables high resolution modes (if available).
Note that this function will also reset other settings controlled by
the registers it writes to.
*/
void LSM303::enableDefault(void)
{
  // Accelerometer
  // 0x57 = 0b01010111
  // AODR = 0101 (200 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
  writeReg(CTRL1, 0x77);

  // 0x00 = 0b00000000
  // AFS = 0 (+/- 2 g full scale)
  writeReg(CTRL2, 0x00);

  // // Magnetometer, which is not used

  // // 0x64 = 0b01100100
  // // M_RES = 11 (high resolution mode); M_ODR = 001 (6.25 Hz ODR)
  // writeReg(CTRL5, 0x64);

  // // 0x20 = 0b00100000
  // // MFS = 01 (+/- 4 gauss full scale)
  // writeReg(CTRL6, 0x20);

  // // 0x00 = 0b00000000
  // // MLP = 0 (low power mode off); MD = 00 (continuous-conversion mode)
  // writeReg(CTRL7, 0x00);
}

void LSM303::enableFIFO(void) //only FIFO for accelerometer now
{
  // Accelerometer
  // 0x57 = 0b01110111
  // AODR = 0101 (200 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
  writeReg(CTRL1, 0x77);

  // 0x00 = 0b00000000
  // AFS = 0 (+/- 2 g full scale)
  writeReg(CTRL2, 0x00);

  // 0x40 = 0b01000000
  // FIFO_EN = 1 (enable FIFO)
  writeReg(CTRL0, 0x40);

  // 0x40 = 0b01000000
  // FM = 010 (STREAM mode)
  writeReg(FIFO_CTRL, 0x40);  
}

char LSM303::readReg(char reg)
{
  char value;

  _i2c->write(acc_address, &reg, 1, true);
  _i2c->read(acc_address, &value, 1);

  return value;
}

void LSM303::writeReg(uint8_t reg, uint8_t value)
{
  char cmd[2] = {reg,value};
  _i2c->write(acc_address,cmd,2);
}

// Reads the 3 accelerometer channels and stores them in vector a
void LSM303::readAcc(void)
{
  char cmd[1];
  // assert the MSB of the address to get the accelerometer
  // to do slave-transmit subaddress updating.
  cmd[0] = (OUT_X_L_A | (1 << 7));
  _i2c->write(acc_address,cmd,1);
  
  char reading[6];
  _i2c->read(acc_address,reading,6);

  a.x = (int16_t)(reading[1] << 8 | reading[0]);
  a.y = (int16_t)(reading[3] << 8 | reading[2]);
  a.z = (int16_t)(reading[5] << 8 | reading[4]); 
}

// Reads the 3 accelerometer channels stored in FIFO and stores them in vector a
void LSM303::readAccFIFO(void)
{
  char FIFO_SRC_status = readReg(FIFO_SRC);
  int FSS = (FIFO_SRC_status & 0x1F); //number of elements in FIFO stack, max(FSS) = 31
  
  // assert the MSB of the address to get the accelerometer to do slave-transmit subaddress updating.
  char cmd = (OUT_X_L_A | (1 << 7));
  _i2c->write(acc_address,&cmd,1);
  
  char reading[6*(FSS+1)];
  _i2c->read(acc_address,reading,6*(FSS+1));

  // combine high and low bytes and take an average
  a.x = 0.0; a.y = 0.0; a.z = 0.0;
  for (int i=0; i <= FSS; i++){
    a.x += (int16_t)((int16_t)reading[6*i+1] << 8 | reading[6*i]);
    a.y += (int16_t)((int16_t)reading[6*i+3] << 8 | reading[6*i+2]);
    a.z += (int16_t)((int16_t)reading[6*i+5] << 8 | reading[6*i+4]);
  }
  a.x = a.x/(float)(FSS+1);
  a.y = a.y/(float)(FSS+1);
  a.z = a.z/(float)(FSS+1);
}

// Reads the 3 magnetometer channels and stores them in vector m
void LSM303::readMag(void)
{
  char cmd[1];
  cmd[0] = (_device == device_D) ? translated_regs[-OUT_X_L_M] | (1 << 7) : translated_regs[-OUT_X_H_M];
  _i2c->write(mag_address,cmd,1);

  char reading[6];
  _i2c->read(mag_address, reading, 6); 

  m.x = (int16_t)(reading[1] << 8 | reading[0]);
  m.y = (int16_t)(reading[3] << 8 | reading[2]);
  m.z = (int16_t)(reading[5] << 8 | reading[4]); 
}

// Reads all 6 channels of the LSM303 and stores them in the object variables
void LSM303::read(void)
{
  readAcc();
  // readMag(); //Don't really need this
}

// Reads all 6 channels of the LSM303 from FIFO and stores them in the object variables
void LSM303::readFIFO(void)
{
  readAccFIFO();
  convertReadings();
  // readMagFIFO(); //Don't really need this
}

void LSM303::convertReadings(void)
{
  //returns readings in Gs
  float LSB_2_G = .061/1000.0;
  a.x = a.x*LSB_2_G;
  a.y = a.y*LSB_2_G;
  a.z = a.z*LSB_2_G;
}

/*
Returns the angular difference in the horizontal plane between a
default vector and north, in degrees.

The default vector here is chosen to point along the surface of the
PCB, in the direction of the top of the text on the silkscreen.
This is the +X axis on the Pololu LSM303D carrier and the -Y axis on
the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH carriers.
*/
float LSM303::heading(void)
{
  if (_device == device_D)
  {
    return heading((vector<int>){1, 0, 0});
  }
  else
  {
    return heading((vector<int>){0, -1, 0});
  }
}

void LSM303::vector_normalize(vector<float> *a)
{
  float mag = sqrt(vector_dot(a, a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}

// Private Methods //////////////////////////////////////////////////////////////

int LSM303::testReg(int address, regAddr reg)
{

  char cmd[1];
  cmd[0] = reg;
  char reading[4];
  if( _i2c->write(address,cmd,1) != 0){
    return TEST_REG_ERROR;
  }
  if( _i2c->read(address,reading,4) == 0){
    return (reading[3] | reading[2] | reading[1] | reading[0]);
  }
  else {
    return TEST_REG_ERROR;
  }
}