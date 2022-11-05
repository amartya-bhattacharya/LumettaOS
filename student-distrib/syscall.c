/* syscall.c - System call functions
 * vim:ts=4 sw=4 noexpandtab
 */

#include "syscall.h"
#include "lib.h"
#include "filesystem.h"
#include "paging.h"
#include "rtc.h"


/* Local variables */
int8_t check_exe[4] = {0x7f, 0x45, 0x4c, 0x46};  // first 4 bytes identifying an executable

/* Local functions */
int32_t system_execute(const uint8_t * command) {
    uint8_t command_name[32] = {0};
    uint32_t args[128] = {0};
    int8_t exe[40] = {0};  // header occupies first 40 bytes of the file
    struct dentry command_dentry;
    uint32_t command_inode;
    void * entry_point;

    // copy command into a buffer until /0 or /n is reached
    int i = 0;
    while (command[i] != '\n' && command[i] != ' ' && i < 32) {
        command_name[i] = command[i];
        i++;
    }
    // TODO check if command was an enter press
    // TODO check if command is quit terminal

    // check rest of command for arguments
    if (command[i] == ' ') {
        i++;
        int j = 0;
        while (command[i] != '\n' && i < 128) {
            args[j] = command[i];
            i++;
            j++;
        }
    }
    
    if (read_dentry_by_name(command_name, &command_dentry))
        return -1; // read failed

    // TODO check dentry file descriptor

    command_inode = command_dentry.ind;

    // check ELF header to see if it is a executable (read_data first 4 bytes)
    read_data(command_inode, 0, (uint8_t *)exe, 40);
    if (strncmp(exe, check_exe, 4))
        return -1; // not an executable
    
    entry_point = (void *)(exe[24] + (exe[25] << 8) + (exe[26] << 16) + (exe[27] << 24));
    
    // set up paging for the program (flush TLB)
    // load in data
    // tss.esp0 = kernel stack pointer
    // set up and load pcb (setup fd[0] and fd[1])
    return 0;
}

int32_t open (const uint8_t* filename){
     // int file_type = get_filetype(filename);
     // int found_open_fd=0;
     // if(file_type == -1){
     //      return -1;
     // }
     // //return -1 if array is full
     // if (file_type == 0){
     //      //set the f_op fields to RTC
     // int i;
     // for (i=2; i<8; i++){
     //      if(file_desc_tb[i].flag == 0){  //if entry dne
     //           found_open_fd=1;
     //           file_desc_tb[i].f_op->read=rtc_read;
     //           break;
     //      }
     //  }
     //  if(found_open_fd ==0){
     //      return -1;
     //  }
     // }
     // else if(file_type ==1){
     //      //set the f_op fields to directory
     //      int i;
     //      for (i=2; i<8; i++){
     //           if(file_desc_tb[i].flag == 0){  //if entry dne
     //                found_open_fd=1;
     //                file_desc_tb[i].f_op->read=rtc_read;
     //                file_desc_tb[i].f_op->write=rtc_write;
     //                file_desc_tb[i].f_op->open=rtc_open;
     //                file_desc_tb[i].f_op->close=rtc_close;
     //                break;
     //           }
     //      }
     //      if(found_open_fd ==0){
     //           return -1;
     //      }
     // }
     // else if(file_type == 2){
     //      //set the f_op fields to regular file

     // }
 
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
