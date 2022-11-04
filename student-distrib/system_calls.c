#include "lib.h"
#include "system_calls.h"

int32_t open (const uint8_t* filename){
     get_ft(filename);
     return -1;
}

int32_t write (int32_t fd, const void* buf, int32 t nbytes){

     return -1;
    /*
    * TODO: make assembly file with jmp table to jump to these c 
    * functions
    */

     //fd is an integer index into the file descriptor table
     //access the file operations table ptr within the desctable index and 
     //tells you what specific system call to execute

     if(file_desc_tb[fd].fotp == 0){
          
     }
}

int32_t read (int32_t fd, void* buf, int32 t nbytes){
     return -1;
}

int32_t close (int32_t fd){
     return -1;
}