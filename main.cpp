#include "mbed.h"
#include "SDFileSystem.h"
#include "AutoSDLogger.h"

SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
AutoSDLogger autoSD;
Serial pc(USBTX, USBRX);
AnalogIn sensor(PTB2);
//DigitalOut bled(LED3);
FILE *fp;

int main() {
    pc.printf("Starting \r\n");

    autoSD.ready_datalogger(); // Prepares a datalog filename according to index.txt
    pc.printf("Index: %d",autoSD.curr_index());
    
    printf("\nWriting data to the sd card \r\n");
    fp = fopen(autoSD.filepath, "w");
    if (fp == NULL) {
        pc.printf("Unable to write the file \r\n");
    }
    for ( int i = 0; i < 100; i++){        
        fprintf(fp, "%f, %f, %f \r\n",sensor.read(),sensor.read()+.1,sensor.read()+.05);
    }
    fclose(fp);
    
    pc.printf("File successfully written! \r\n");
    printf("End of Program. \r\n");
}
