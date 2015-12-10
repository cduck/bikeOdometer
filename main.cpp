#include "mbed.h"
#include "SDFileSystem.h"
#include "AutoSDLogger.h"
#include "LPS.h"
#include "L3G.h"
#include "LSM303.h"
#include "SparkFunBLEMate2.h"
#include "WString.h"
#include "itoas.h"
#include "MTSSerialFlowControl.h"

// Helpful Things
Serial pc(USBTX, USBRX);
Timer t;
DigitalOut rled(LED1);
DigitalOut gled(LED2);
DigitalOut bled(LED3);
DigitalIn button(PTC6); //SW2


// SD Card
SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
AutoSDLogger autoSD;
FILE *fp; //SD card file writing

// BLE
#ifdef USE_FLOW_CONTROL
  mts::MTSSerialFlowControl bleUart(PTC17, PTC16, PTA1, PTB9);
#else
  Serial bleUart(PTC17, PTC16);
#endif
BLEMate2 BTModu(&bleUart);

// Pololu Sensor and Motion Data Variables
I2C i2c(PTC11, PTC10); //SDA (PTC11), SCL (PTC10)
LPS ps(&i2c);
L3G gyr(&i2c, &pc);
LSM303 acc(&i2c, &pc);
float altitude = 0.0;
float altitude_prev = 0.0;
float incline = 0.0;
float tot_dist = 0.0;
float speed = 0.0;
float tot_dist_prev = 0.0; //placeholder
float speed_prev = 0.0;
int r_altitude = 0;
int r_incline = 0;
int r_dist = 0;
int r_speed = 0;

// Helper Variables
int timeLastPoll = 0;
int iter = 0;

// Constants
float MBED_POLLING_PERIOD_MS =  1/25.0*1000; // units of ms 
float MBED_POLLING_PERIOD_S =  1/25.0; // units of s 
float R = 0.2794; //11 inches in meters


// Helper Functions
void setup(); //device hardware set up
void setupSensor(); //Sensor set up
void setupBLE(); //BLE set up
void calcMotionData(); //computes motion data
int roundData(float data); //rounds data to prepare for sending over BLE
String packageData2String(int data, char* datastring);
void saveLastData(); //saves t-1 data
uint32_t millis(); //millis implementation on mbed
void setupPeripheralExample(); //BLE set up for peripheral

int main() {
    pc.printf("Starting \r\n");

    setup(); //initializes sensors

    t.start();
    while(button){
        timeLastPoll = t.read_ms();
        altitude = ps.pressureToAltitudeMeters(ps.readPressureMillibars());
        acc.readFIFO();
        gyr.readFIFO();

        // pc.printf("%d Att: %2.2f \tGyr: %2.2f %2.2f %2.2f \tAcc: %2.2f %2.2f %2.2f \tT: %d\r\n",
        //     iter,
        //     altitude,
        //     gyr.g.x,gyr.g.y,gyr.g.z,
        //     acc.a.x,acc.a.y,acc.a.z,
        //     t.read_ms()-timeLastPoll);

        calcMotionData();

        r_altitude = roundData(altitude);
        r_incline = roundData(incline);
        r_dist = roundData(tot_dist);
        r_speed = roundData(speed);

        pc.printf("%d \t%d \t%d \t%d\r\n",r_altitude,r_incline,r_dist,r_speed);

        fprintf(fp, "%d, %f, %f, %f, %f, %f, %f, %f, %d, %d, %d \r\n",
            (t.read_ms() - timeLastPoll),
            altitude,
            gyr.g.x,gyr.g.y,gyr.g.z,
            acc.a.x,acc.a.y,acc.a.z,
            r_incline,
            r_dist,
            r_speed);

        // static String fullBuffer = "";
        // static String saveBuffer = "";
        static unsigned long lastSendTime = millis();
        // // Send Stuff over BLE
        // if (fullBuffer != ""){
        //     pc.printf("sending \r\n");
        //     pc.printf("%s\r\n", fullBuffer.c_str());
        //     BTModu.sendData(fullBuffer);
        //     saveBuffer = String(fullBuffer);
        //     fullBuffer = "";
        // }
        // else {
            if (lastSendTime + 250 < millis()) {
                // static int iii = 0;
                // static char iiistring[20];
                static char datastring[20];
                // BTModu.sendData(String(", i=") + itoa(iii, iiistring, 10));
                // iii++;
                // BTModu.sendData(String("x") + itoa(roundData(speed), iiistring, 10) +
                // itoa(roundData(altitude), iiistring, 16) +
                // itoa(roundData(incline), iiistring, 10) +
                // itoa(roundData(tot_dist), iiistring, 16) );                
                BTModu.sendData(String("x")+packageData2String(r_altitude,datastring)+
                    packageData2String(r_incline,datastring)+
                    packageData2String(r_dist,datastring)+
                    packageData2String(r_speed,datastring) );
                lastSendTime += 250;
            }
        // }
        // static String inputBuffer;
        // while (bleUart.readable() > 0)
        // {
        //   pc.printf("bytes: %d\r\n",bleUart.readable());
        //   inputBuffer.concat((char)bleUart.getc());
        // }

        // // Lines that end with \n\r- but also start with "RCV=" are the ones we keep
        // if (inputBuffer.endsWith("\n\r"))
        // {
        //   if (inputBuffer.startsWith("RCV="))
        //   {
        //     inputBuffer.trim(); // Remove \n\r from end.
        //     inputBuffer.remove(0,4); // Remove RCV= from front.
        //     fullBuffer += inputBuffer;
        //     inputBuffer = "";
        //     pc.printf("processed read bytes \r\n");
        //   }
        //   else
        //   {
        //     inputBuffer = "";
        //   }
        // }

        saveLastData();            
        while( (t.read_ms() - timeLastPoll) < MBED_POLLING_PERIOD_MS){
            gled = 0;
        }
        // pc.printf("LT: %d\r\n",t.read_ms()-timeLastPoll);
        gled = 1; // iter++; 
    }
    fclose(fp);
    pc.printf("File successfully written! \r\n");
    printf("End of Program. \r\n");
}

void calcMotionData(){
    // Computes the relevant motion data that we are interested in
    incline = asin((altitude-altitude_prev)/(speed_prev*MBED_POLLING_PERIOD_S));
    if isnan(incline){ // if incline is indeterminate
        incline = 0.0;
    }
    tot_dist = tot_dist_prev + speed_prev*MBED_POLLING_PERIOD_S;
    speed = gyr.g.z*R;
}

int roundData(float data){
    int roundedData = round(data*10);
    return roundedData;
}

void saveLastData(){
    // Saves the last time step worth of data for Motion Data Calculation
    altitude_prev = altitude;
    tot_dist_prev = tot_dist;
    speed_prev = speed;
}

String packageData2String(int datum, char* datastring){
    itoa(datum,datastring,16);
    int dataLength = strlen(datastring);
    if (datum >= 0){
        String padded = "x000";
        if (dataLength < 4){ // not a length 4 string, gotta pad it with zeros
            padded.concat(datastring);
            padded.remove(0,dataLength);
        }
        return padded;
    }
    else{ 
        //by default, it appends many 0xFF to the datastring, gotta remove them
        String trimmed = String(datastring);
        if (dataLength > 4){        
            trimmed.remove(0,dataLength-4);
        }
        return trimmed;
    }
}

void setup(){
    setupBLE();
    setupSensor();
    //successful start up LED sequence
    gled = 1; bled = 0;
    wait(0.5);
    gled = 0; bled = 1;
    wait(0.5);
    gled = 1; bled = 0;
    wait(0.5);
    gled = 1; bled = 1;
}

void setupBLE(){
    // BLE Set Up
    // Regarding function return values: most functions that interact with the
    //  BC118 will return BLEMate2::opResult values. The possible values here
    //  are:
    //  REMOTE_ERROR - No remote devices exist.
    //  INVALID_PARAM - You've called the function with an invalid parameter.
    //  TIMEOUT_ERROR - The BC118 failed to respond to the command in a timely
    //                   manner; timely is redefined for each command.
    //  MODULE_ERROR - The BC118 didn't like the command string it received.
    //                  This will probably only occur when you attempt to send
    //                  commands and parameters outside the built-ins. 
    //  SUCCESS - What it says.

    #ifdef SETUP_NEW_BLEMATE
    // Reset is a blocking function which gives the BC118 a few seconds to reset.
    //  After a reset, the module will return to whatever settings are in
    //  non-volatile memory. One other *super* important thing it does is issue
    //  the "SCN OFF" command after the reset is completed. Why is this important?
    //  Because if the device is in central mode, it *will* be scanning on reset.
    //  No way to change that. The text traffic generated by the scanning will
    //  interfere with the firmware on the Arduino properly identifying response
    //  strings from the BC118.
    if (BTModu.reset() != BLEMate2::SUCCESS)
    {
        pc.puts("Module reset error!\n");
        while (1);
    }

    // restore() resets the module to factory defaults; you'll need to perform
    //  a writeConfig() and reset() to make those settings take effect. We don't
    //  do that automatically because there may be things the user wants to
    //  change before committing the settings to non-volatile memory and
    //  resetting.
    if (BTModu.restore() != BLEMate2::SUCCESS)
    {
        pc.puts("Module restore error!\n");
        while (1);
    }
    // writeConfig() stores the current settings in non-volatile memory, so they
    //  will be in place on the next reboot of the module. Note that some, but
    //  not all, settings changes require a reboot. It's probably in general best
    //  to write/reset when changing anything.
    if (BTModu.writeConfig() != BLEMate2::SUCCESS)
    {
        pc.puts("Module write config error!\n");
        while (1);
    }
    // One more reset, to make the changes take effect.
    if (BTModu.reset() != BLEMate2::SUCCESS)
    {
        pc.puts("Second module reset error!\n");
        while (1);
    }
    #endif

    bleUart.baud(9600);

      bool inCentralMode = false;
  // A word here on amCentral: amCentral's parameter is passed by reference, so
  //  the answer to the question "am I in central mode" is handed back as the
  //  value in the boolean passed to it when it is called. The reason for this
  //  is the allow the user to check the return value and determine if a module
  //  error occurred: should I trust the answer or is there something larger
  //  wrong than merely being in the wrong mode?
  BTModu.amCentral(inCentralMode); 
  if (inCentralMode)
  {
    BTModu.BLEPeripheral();
    BTModu.BLEAdvertise();
  }

  // There are a few more advance settings we'll probably, but not definitely,
  //  want to tweak before we reset the device.

  // The CCON parameter will enable advertising immediately after a disconnect.
  BTModu.stdSetParam("CCON", "ON");
  // The ADVP parameter controls the advertising rate. Can be FAST or SLOW.
  BTModu.stdSetParam("ADVP", "FAST");
  // The ADVT parameter controls the timeout before advertising stops. Can be
  //  0 (for never) to 4260 (71min); integer value, in seconds.
  BTModu.stdSetParam("ADVT", "0");
  // The ADDR parameter controls the devices we'll allow to connect to us.
  //  All zeroes is "anyone".
  BTModu.stdSetParam("ADDR", "000000000000");

  BTModu.writeConfig();
  BTModu.reset();

  pc.printf("BLE ready to receive connections\r\n");
  // We're set up to allow anything to connect to us now.

}

void setupSensor(){
    // Sensor Set Up
    rled = 1; gled = 1; bled = 1;

    autoSD.ready_datalogger(); // Prepares a datalog filename according to index.txt
    pc.printf("Index: %d\r\n",autoSD.curr_index());         
    fp = fopen(autoSD.filepath, "w");
    if (fp == NULL) {
        pc.printf("Unable to write the file \r\n");
        while(1){ bled = !bled; wait_ms(100);}
    }

    if(!ps.init()){ //enable pressure sensor
        pc.printf("Unable to talk to Barometer\r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }
    else{
        ps.enableFIFO();
    }

    if(!gyr.init()){
        pc.printf("Unable to talk to Gyroscope\r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }
    else{
        gyr.enableFIFO();
    }

    if(!acc.init()){
        pc.printf("Unable to talk to Accelerometer\r\n");
        while(1){ rled = !rled; wait_ms(100);}
    }
    else{
        acc.enableFIFO();
    }


    pc.printf("Sensor set up successfully\r\n");

}

uint32_t millis() {
  return us_ticker_read() / 1000;
}
