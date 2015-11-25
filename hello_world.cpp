#include "mbed.h"
#include "SDFileSystem.h"
#include "AutoSDLogger.h"
#include "LPS.h"
#include "LSM303.h"
#include "L3G.h"

SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
AutoSDLogger autoSD;
Serial pc(USBTX, USBRX);
AnalogIn sensor(PTB2);
//DigitalOut bled(LED3);
FILE *fp;

I2C i2c(PTE25, PTE24);
LPS ps(&i2c);
LSM303 acc(&i2c);
L3G gyr(&i2c);

int main() {
    pc.printf("Starting \r\n");

    autoSD.ready_datalogger(); // Prepares a datalog filename according to index.txt
    pc.printf("Index: %d",autoSD.curr_index());         

    // Sample Code to Test 10 DOF sensor
    if(!ps.init()){
        pc.printf("Unable to talk to Barometer\r\n");
        while(1);
    }
    else{
        pc.printf("Success in talking to Barometer! \r\n");
        ps.enableDefault();
    }

    // if(!acc.init()){
    //     pc.printf("Unable to talk to Acc/Mag\r\n");
    //     while(1);
    // }
    // else{
    //     pc.printf("Success in talking to Acc/Mag! \r\n");
    //     acc.enableDefault();
    // }

    if(!gyr.init()){
        pc.printf("Unable to talk to Gyroscope\r\n");
        while(1);
    }
    else{
        pc.printf("Success in talking to Gyroscope! \r\n");
        gyr.enableDefault();
    }
    printf("\nWriting data to the SD card \r\n");
    fp = fopen(autoSD.filepath, "w");
    if (fp == NULL) {
        pc.printf("Unable to write the file \r\n");
    }
    for ( int i = 0; i < 1000; i++){

        gyr.read();
        float pressure = ps.readPressureMillibars();
        float altitude = ps.pressureToAltitudeMeters(pressure);
        pc.printf("Pressure: %f %f GYRO: %d %d %d\r\n",
            pressure,altitude,
            gyr.g.x,gyr.g.y,gyr.g.z);
        // acc.read();

        // pc.printf("Pressure: %f mBar Altitude: %f m A: %6d %6d %6d M: %6d %6d %6d \r\n",
        //     pressure,altitude,acc.a.x,acc.a.y,acc.a.z,
        //     acc.m.x,acc.m.y,acc.m.z);
        fprintf(fp, "%f, %f, %d, %d, %d\r\n",
            pressure,altitude,gyr.g.x,gyr.g.y,gyr.g.z);
        wait_ms(10);
    }
    fclose(fp);
    pc.printf("File successfully written! \r\n");
    printf("End of Program. \r\n");
}