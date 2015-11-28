/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "FXOS8700Q.h"
#include "mbed.h"
#define UINT14_MAX        16383

uint8_t SensorBuffer[12];
int     MagReadStatus;

FXOS8700Q::FXOS8700Q(I2C* i2c){

    _i2c = i2c;
    _addr = FXOS8700CQ_SLAVE_ADDR1;
}

FXOS8700Q::~FXOS8700Q() { }

bool FXOS8700Q::init(void){
    if (whoAmI() == FXOS8700Q_WHOAMI_VAL){
        return true;
    }
    else{
        return false;
    }
}

void FXOS8700Q::enableDefault(void) {
    // activate the peripheral
    // uint8_t data[2] = {FXOS8700Q_CTRL_REG1, 0x00};
    // writeRegs(data, 2);
    // data[0] = FXOS8700Q_M_CTRL_REG1;
    // data[1] = 0x1F;
    // writeRegs(data, 2);
    // data[0] = FXOS8700Q_M_CTRL_REG2;
    // data[1] = 0x20;
    // writeRegs(data, 2);
    // data[0] = FXOS8700Q_XYZ_DATA_CFG;
    // data[1] = 0x00;
    // writeRegs(data, 2);
    // data[0] = FXOS8700Q_CTRL_REG1;
    // data[1] = 0x1C;
    // writeRegs(data, 2);
    // MagReadStatus = 0;

    // uint8_t data[2];
    // readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    // data[1] |= 0x01;
    // data[0] = FXOS8700Q_CTRL_REG1;
    // writeRegs(data, 2);
    // MagReadStatus = 0;

    // Activate the peripheral
    writeRegs(FXOS8700Q_CTRL_REG1, 0x00);
    writeRegs(FXOS8700Q_M_CTRL_REG1, 0x1F);
    // 0x20 = 0b00100000, doesn't seem to do anything
    writeRegs(FXOS8700Q_M_CTRL_REG2, 0x20);
    // 0b00000000, select +/-2g range, and no HPFilter
    writeRegs(FXOS8700Q_XYZ_DATA_CFG, 0x00);
    writeRegs(FXOS8700Q_CTRL_REG1, 0x1C);

    // 0x15 = 0b00010101, ODR = 200 Hz, Active Mode, Normal-read mode, Reduced Noise mode
    writeRegs(FXOS8700Q_CTRL_REG1, 0x15);
    // writeRegs(FXOS8700Q_CTRL_REG1, 0x1D);

    MagReadStatus = 0;

}

void FXOS8700Q::disable(void) {
    // uint8_t data[2];
    // readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    // data[1] &= 0xFE;
    // data[0] = FXOS8700Q_CTRL_REG1;
    // writeRegs(data, 2);
    // MagReadStatus = 0;

    // Turns off accelerometer
    writeRegs(FXOS8700Q_CTRL_REG1, 0x00);
}



uint32_t FXOS8700Q::whoAmI() {
    uint8_t who_am_i = readRegs(FXOS8700Q_WHOAMI);
    return (uint32_t) who_am_i;
}

void FXOS8700Q::read(){
    char c = FXOS8700Q_OUT_X_MSB;
    _i2c->write(_addr,&c,1,true);

    uint8_t reading[6];
    _i2c->read(_addr,(char*)reading, 6);

    int16_t acc, t[3];
    acc = (reading[0] << 6) | (reading[1] >> 2);
    if (acc > UINT14_MAX/2){
        acc -= UINT14_MAX;        
    }
    t[0] = acc;
    acc = (reading[2] << 6) | (reading[3] >> 2);
    if (acc > UINT14_MAX/2){
        acc -= UINT14_MAX;
    }
    t[1] = acc;
    acc = (reading[4] << 6) | (reading[5] >> 2);
    if (acc > UINT14_MAX/2){
        acc -= UINT14_MAX;
    } 
    t[2] = acc;   
    a.x = ((float)t[0])/4096.0f;
    a.y = ((float)t[1])/4096.0f;
    a.z = ((float)t[2])/4096.0f;
}


// int16_t FXOS8700Q::getAccAxis(uint8_t addr) {
//     int16_t acc;
//     uint8_t res[2];
//     readRegs(addr, res, 2);

//     acc = (res[0] << 6) | (res[1] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;

//     return acc;
// }

// uint32_t FXOS8700Q::dataReady(void) {
//     uint8_t stat = 0;
//     readRegs(FXOS8700Q_STATUS, &stat, 1);
//     return (uint32_t) stat;
// }

// uint32_t FXOS8700Q::sampleRate(uint32_t f) {
//     return(50); // for now sample rate is fixed at 50Hz
// }

// void FXOS8700Q::getX(float * x) {
//     *x = (float(getAccAxis(FXOS8700Q_OUT_X_MSB))/4096.0f);
// }

// void FXOS8700Q::getY(float * y) {
//     *y = (float(getAccAxis(FXOS8700Q_OUT_Y_MSB))/4096.0f);
// }

// void FXOS8700Q::getZ(float * z) {
//     *z = (float(getAccAxis(FXOS8700Q_OUT_Z_MSB))/4096.0f);
// }

// void FXOS8700Q::getX(int16_t * d) {
//     *d = getAccAxis(FXOS8700Q_OUT_X_MSB);
// }

// void FXOS8700Q::getY(int16_t * d) {
//     *d = getAccAxis(FXOS8700Q_OUT_Y_MSB);
// }

// void FXOS8700Q::getZ(int16_t * d) {
//     *d = getAccAxis(FXOS8700Q_OUT_Z_MSB);
// }


// void FXOS8700Q::getAxis(MotionSensorDataUnits &data) {
//     int16_t acc, t[3];

//    readRegs(FXOS8700Q_OUT_X_MSB, SensorBuffer, 12);

//     acc = (SensorBuffer[0] << 6) | (SensorBuffer[1] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;
//     t[0] = acc;
//     acc = (SensorBuffer[2] << 6) | (SensorBuffer[3] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;
//     t[1] = acc;
//     acc = (SensorBuffer[4] << 6) | (SensorBuffer[5] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;
//     t[2] = acc;
//     data.x = ((float) t[0]) / 4096.0f;
//     data.y = ((float) t[1]) / 4096.0f;
//     data.z = ((float) t[2]) / 4096.0f;
//     MagReadStatus = 1;
// }


// void FXOS8700Q::getAxis(MotionSensorDataCounts &data) {
//     int16_t acc;
//     readRegs(FXOS8700Q_OUT_X_MSB, SensorBuffer, 12);

//     acc = (SensorBuffer[0] << 6) | (SensorBuffer[1] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;
//     data.x = acc;
//     acc = (SensorBuffer[2] << 6) | (SensorBuffer[3] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;
//     data.y = acc;
//     acc = (SensorBuffer[4] << 6) | (SensorBuffer[5] >> 2);
//     if (acc > UINT14_MAX/2)
//         acc -= UINT14_MAX;
//     data.z = acc;
//     MagReadStatus = 1;
// }

char FXOS8700Q::readRegs(char reg) {
    char value;

    _i2c->write(_addr, &reg, 1, true);
    _i2c->read(_addr, &value, 1);

    return value;
}

void FXOS8700Q::writeRegs(char reg, char value) {
    char cmd[2];
    cmd[0] = reg;
    cmd[1] = value;
    _i2c->write(_addr,cmd,2);    
}


// FXOS8700Q_mag::FXOS8700Q_mag(PinName sda, PinName scl, int addr) : m_i2c(sda, scl), m_addr(addr) {
//     // activate the peripheral
//     uint8_t data[2] = {FXOS8700Q_CTRL_REG1, 0x00};
//     m_i2c.frequency(400000);
//     writeRegs(data, 2);
//     data[0] = FXOS8700Q_M_CTRL_REG1;
//     data[1] = 0x1F;
//     writeRegs(data, 2);
//     data[0] = FXOS8700Q_M_CTRL_REG2;
//     data[1] = 0x20;
//     writeRegs(data, 2);
//     data[0] = FXOS8700Q_XYZ_DATA_CFG;
//     data[1] = 0x00;
//     writeRegs(data, 2);
//     data[0] = FXOS8700Q_CTRL_REG1;
//     data[1] = 0x18;//0x1D;
//     writeRegs(data, 2);
//     MagReadStatus = 0;
// }

// FXOS8700Q_mag::~FXOS8700Q_mag() { }

// void FXOS8700Q_mag::enable(void) {
//     uint8_t data[2];
//     readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
//     data[1] |= 0x01;
//     data[0] = FXOS8700Q_CTRL_REG1;
//     writeRegs(data, 2);
//     MagReadStatus = 0;
// }

// void FXOS8700Q_mag::disable(void) {
//     uint8_t data[2];
//     readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
//     data[1] &= 0xFE;
//     data[0] = FXOS8700Q_CTRL_REG1;
//     writeRegs(data, 2);
//     MagReadStatus = 0;
// }



// uint32_t FXOS8700Q_mag::whoAmI() {
//     uint8_t who_am_i = 0;
//     readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);
//     return (uint32_t) who_am_i;
// }

// uint32_t FXOS8700Q_mag::dataReady(void) {
//     uint8_t stat = 0;
//     readRegs(FXOS8700Q_STATUS, &stat, 1);
//     return (uint32_t) stat;
// }

// uint32_t FXOS8700Q_mag::sampleRate(uint32_t f) {
//     return(50); // for now sample rate is fixed at 50Hz
// }

// void FXOS8700Q_mag::getX(float * x) {
//     *x = (float(getAccAxis(FXOS8700Q_M_OUT_X_MSB)) * 0.1f);
// }

// void FXOS8700Q_mag::getY(float * y) {
//     *y = (float(getAccAxis(FXOS8700Q_M_OUT_Y_MSB)) * 0.1f);
// }

// void FXOS8700Q_mag::getZ(float * z) {
//     *z = (float(getAccAxis(FXOS8700Q_M_OUT_Z_MSB)) * 0.1f);
// }

// void FXOS8700Q_mag::getX(int16_t * d) {
//     *d = getAccAxis(FXOS8700Q_M_OUT_X_MSB);
// }

// void FXOS8700Q_mag::getY(int16_t * d) {
//     *d = getAccAxis(FXOS8700Q_M_OUT_Y_MSB);
// }

// void FXOS8700Q_mag::getZ(int16_t * d) {
//     *d = getAccAxis(FXOS8700Q_M_OUT_Z_MSB);
// }


// void FXOS8700Q_mag::getAxis(MotionSensorDataUnits &data) {
//     int16_t t[3];
//     uint8_t res[6];
    
//    if(MagReadStatus) {
//         t[0] = (SensorBuffer[6] << 8) | SensorBuffer[7];
//         t[1] = (SensorBuffer[8] << 8) | SensorBuffer[9];
//         t[2] = (SensorBuffer[10] << 8) | SensorBuffer[11];        
//         } else {
//         readRegs(FXOS8700Q_M_OUT_X_MSB, res, 6);
//         t[0] = (res[0] << 8) | res[1];
//         t[1] = (res[2] << 8) | res[3];
//         t[2] = (res[4] << 8) | res[5];
//         }
//     data.x = ((float) t[0]) * 0.1f;
//     data.y = ((float) t[1]) * 0.1f;
//     data.z = ((float) t[2]) * 0.1f;
// }


// void FXOS8700Q_mag::getAxis(MotionSensorDataCounts &data) {

//     uint8_t res[6];
//     readRegs(FXOS8700Q_M_OUT_X_MSB, res, 6);

//     data.x = (res[0] << 8) | res[1];
//     data.y = (res[2] << 8) | res[3];
//     data.z = (res[4] << 8) | res[5];
// }

// void FXOS8700Q_mag::readRegs(int addr, uint8_t * data, int len) {
//     char t[1] = {addr};
//     m_i2c.write(m_addr, t, 1, true);
//     m_i2c.read(m_addr, (char *)data, len);
// }

// void FXOS8700Q_mag::writeRegs(uint8_t * data, int len) {
//     m_i2c.write(m_addr, (char *)data, len);
// }


// int16_t FXOS8700Q_mag::getAccAxis(uint8_t addr) {
//     int16_t acc;
//     uint8_t res[2];
//     readRegs(addr, res, 2);

//     acc = (res[0] << 8) | res[1];

//     return acc;
// }