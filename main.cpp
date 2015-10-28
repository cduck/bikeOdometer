#include "mbed.h"
#include "SDFileSystem.h"

SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
Serial pc(USBTX, USBRX);
FILE *fp;

int file_copy(const char *src, const char *dst)
{
    int retval = 0;
    int ch;
 
    FILE *fpsrc = fopen(src, "r");   // src file
    FILE *fpdst = fopen(dst, "w");   // dest file
    
    while (1) {                  // Copy src to dest
        ch = fgetc(fpsrc);       // until src EOF read.
        if (ch == EOF) break;
        fputc(ch, fpdst);
    }
    fclose(fpsrc);
    fclose(fpdst);
  
    fpdst = fopen(dst, "r");     // Reopen dest to insure
    if (fpdst == NULL) {          // that it was created.
        retval = -1;           // Return error.
    } else {
        fclose(fpdst);
        retval = 0;              // Return success.
    }
    return retval;
}

uint32_t do_list(const char *fsrc)
{
    DIR *d = opendir(fsrc);
    struct dirent *p;
    uint32_t counter = 0;

    while ((p = readdir(d)) != NULL) {
        counter++;
        printf("%s\n", p->d_name);
    }
    closedir(d);
    return counter;
}

// bool is_folder(const char *fdir)
// {
//     DIR *dir = opendir(fdir);
//     if (dir) {
//         closedir(dir);
//     }
//     return (dir != NULL);

// }

// bool is_file(const char *ffile)
// {
//     FILE *fp = fopen(ffile, "r");
//     if (fp) {
//         fclose(fp);
//     }
//     return (fp != NULL);
// }

void do_remove(const char *fsrc)
{
    DIR *d = opendir(fsrc);
    struct dirent *p;
    char path[30] = {0};
    while((p = readdir(d)) != NULL) {
        strcpy(path, fsrc);
        strcat(path, "/");
        strcat(path, p->d_name);
        remove(path);
    }
    closedir(d);
    remove(fsrc);
}

int main()
{
    pc.printf("Initializing \n");
    wait(2);

    do_remove("/sd/test1"); /* clean up from the previous Lab 5 if was executed */
    if (do_list("/sd") == 0) {
        printf("No files/directories on the sd card.");
    }

    printf("\nCreating two folders. \n");
    mkdir("/sd/test1", 0777);
    mkdir("/sd/test2", 0777);

    fp = fopen("/sd/test1/1.txt", "w");
    if (fp == NULL) {
        pc.printf("Unable to write the file \n");
    } else {
        fprintf(fp, "1.txt in test 1");
        fclose(fp);
    }

    fp = fopen("/sd/test2/2.txt", "w");
    if (fp == NULL) {
        pc.printf("Unable to write the file \n");
    } else {
        fprintf(fp, "2.txt in test 2");
        fclose(fp);
    }

    printf("\nList all directories/files /sd.\n");
    do_list("/sd");

    printf("\nList all files within /sd/test1.\n");
    do_list("/sd/test1");


    printf("\nList all files within /sd/test2.\n");
    do_list("/sd/test2");

    int status = file_copy("/sd/test2/2.txt", "/sd/test1/2_copy.txt");
    if (status == -1) {
        printf("Error, file was not copied.\n");
    }
    printf("Removing test2 folder and 2.txt file inside.");
    remove("/sd/test2/2.txt");
    remove("/sd/test2");

    printf("\nList all directories/files /sd.\n");
    do_list("/sd");

    printf("\nList all files within /sd/test1.\n");
    do_list("/sd/test1");

    printf("\nEnd of complete Lab 5. \n");
}
