#include "SDFileSystem.h"

class AutoSDLogger {
public:
    char filepath[256];
    char index_path[256];
    char filename[256];
    int index;

    AutoSDLogger();
    
    int ready_datalogger();
    int curr_index();
    
private: 
    int increment_index_file();

};