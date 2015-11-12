#include "mbed.h"
#include "SDFileSystem.h"
#include "AutoSDLogger.h"
#include "LPS.h"

SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
AutoSDLogger autoSD;
Serial pc(USBTX, USBRX);
AnalogIn sensor(PTB2);
//DigitalOut bled(LED3);
FILE *fp;

I2C i2c(PTE25, PTE24);
LPS ps(&i2c);

int main() {
    pc.printf("Starting \r\n");

    // Sample Code to Write Data
    // autoSD.ready_datalogger(); // Prepares a datalog filename according to index.txt
    // pc.printf("Index: %d",autoSD.curr_index());
    
    // printf("\nWriting data to the sd card \r\n");
    // fp = fopen(autoSD.filepath, "w");
    // if (fp == NULL) {
    //     pc.printf("Unable to write the file \r\n");
    // }
    // for ( int i = 0; i < 100; i++){        
    //     fprintf(fp, "%f, %f, %f \r\n",sensor.read(),sensor.read()+.1,sensor.read()+.05);
    // }
    // fclose(fp);
    
    // pc.printf("File successfully written! \r\n");

    // Sample Code to Test Barometer
    if(!ps.init()){
        pc.printf("Unable to talk to Barometer\r\n");
        while(1);
    }
    else{
        pc.printf("Success in talking to Barometer! \r\n");
        ps.enableDefault();
    }

    while(1){
        float pressure = ps.readPressureMillibars();
        float altitude = ps.pressureToAltitudeMeters(pressure);
        pc.printf("Pressure: %f mBar Altitude: %f m\r\n",pressure,altitude);
    }
    printf("End of Program. \r\n");
}
