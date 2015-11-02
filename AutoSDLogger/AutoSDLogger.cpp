#include "AutoSDLogger.h"

AutoSDLogger::AutoSDLogger(){
    sprintf(index_path,"%s","/sd/index.txt");
    sprintf(filename,"%s","wowData");
}

int AutoSDLogger::ready_datalogger(){
    // Checks SD card if index.txt exists
    // If it doesn't, initialize it with a 0
    FILE *fp;
    int tries = 0;
    fp = fopen(index_path, "r");
    while (fp == 0){
        if ( (fp = fopen(index_path,"r")) == 0 ){ // Try opening file a couple of times as system initializes
            tries += 1;
            wait(0.20);
        }
        if (tries == 25){ //If tried opening many times, just create the file
            fp = fopen(index_path,"w");
            fprintf(fp,"%d",0);
            fclose(fp);
            break;
        }
    }
    increment_index_file();
    return 0;
}

int AutoSDLogger::curr_index(){
    // Returns the current index in index.txt
    return index;
}

int AutoSDLogger::increment_index_file(){
    // Increments the number in index.txt by one
    // Saves the index number to the AutoSDLogger object
    FILE *fp;
    int ind = 0;
    fp = fopen(index_path, "r");
    rewind(fp);
    fscanf(fp,"%d",&ind); //Reads the number in file
    index = ind;
    fclose(fp);
    remove(index_path);
    fp = fopen(index_path, "w"); //Recreate the file while incrementing it by one
    fprintf(fp,"%d\n",ind+1); 
    fclose(fp);  

    sprintf(filepath,"/sd/%s__%d.txt",filename,ind); //Saves the new filename to use when we log a new data file
    return 0;
}