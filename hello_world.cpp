#include "mbed.h"
#include "SDFileSystem.h"
#include "AutoSDLogger.h"
#include "LPS.h"
#include "L3G.h"
#include "LSM303.h"


// Sensor and Serial objects
SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
AutoSDLogger autoSD;
Serial pc(USBTX, USBRX);
DigitalOut rled(LED1);
DigitalOut gled(LED2);
DigitalOut bled(LED3);
DigitalIn button(PTC6);
FILE *fp;
Timer t;

//SDA (PTC11), SCL (PTC10)
I2C i2c(PTC11, PTC10);
LPS ps(&i2c);
L3G gyr(&i2c);
LSM303 acc(&i2c);

// Helper Variables
int timeLastPoll = 0;
int iter = 0;
float altitude = 0.0;
#define MBED_POLLING_PERIOD 1/25.0 // units of s 

// Helper Functions
void setup();


int main() {
    pc.printf("Starting \r\n");

    setup(); //initializes sensors

    t.start();
    timeLastPoll = t.read_ms();
    while(button){

        altitude = ps.pressureToAltitudeMeters(ps.readPressureMillibars());
        gyr.read();
        acc.read();

        fprintf(fp, "%f, %d, %d, %d \r\n",
            altitude,gyr.g.x,gyr.g.y,gyr.g.z);

        pc.printf("%d Att: %2.2f \tGyr: %d %d %d \tAcc: %d %d %d \tT: %d\r\n",
            iter,
            altitude,
            gyr.g.x,gyr.g.y,gyr.g.z,
            acc.a.x,acc.a.y,acc.a.z,
            t.read_ms()-timeLastPoll);

        while( (t.read_ms() - timeLastPoll) < MBED_POLLING_PERIOD){
        }
        // pc.printf("Loop Time: %d",t.read_ms()-timeLastPoll);
        timeLastPoll = t.read_ms();
        iter++;
    }
    fclose(fp);
    pc.printf("File successfully written! \r\n");
    printf("End of Program. \r\n");
}

void setup(){

    rled = 1; gled = 1; bled = 1;

    autoSD.ready_datalogger(); // Prepares a datalog filename according to index.txt
    pc.printf("Index: %d\r\n",autoSD.curr_index());         

    if(!ps.init()){ //enable pressure sensor
        pc.printf("Unable to talk to Barometer\r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }
    else{
        ps.enableDefault();
    }

    if(!gyr.init()){
        pc.printf("Unable to talk to Gyroscope\r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }
    else{
        gyr.enableDefault();
    }

    if(!acc.init()){
        pc.printf("Unable to talk to Accelerometer\r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }
    else{
        acc.enableDefault();
    }

    fp = fopen(autoSD.filepath, "w");
    if (fp == NULL) {
        pc.printf("Unable to write the file \r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }

    pc.printf("Success in Set Up\r\n");

}
