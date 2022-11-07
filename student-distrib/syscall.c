/* syscall.c - System call functions
 * vim:ts=4 sw=4 noexpandtab
 */

#include "syscall.h"
#include "lib.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"

struct fap fap_func_arr[3] ;


/* defines the file descriptor array, only to check 
compile errors for sys calls, comment out later once
PCB is implemented*/
void set_fda(){ 
	unsigned i;
	for(i = 0; i < 8; i++){
		file_desc_tb[i].f_op->read = NULL;
		file_desc_tb[i].f_op->write = NULL;
		file_desc_tb[i].f_op->open = NULL;
		file_desc_tb[i].f_op->close = NULL;
		file_desc_tb[i].flag = 0;
		file_desc_tb[i].file_position = 0;
	}

	return;
}

/* Local variables */
int8_t check_exe[4] = {0x7f, 0x45, 0x4c, 0x46};  // first 4 bytes identifying an executable
pcb_t* curr_pcb[MAX_PROCESSES] = {(pcb_t*)(_8MB - 2 * _8KB),
								  (pcb_t*)(_8MB - 3 * _8KB),
								  (pcb_t*)(_8MB - 4 * _8KB),
								  (pcb_t*)(_8MB - 5 * _8KB),
								  (pcb_t*)(_8MB - 6 * _8KB),
								  (pcb_t*)(_8MB - 7 * _8KB)};  // array of pointers to pcb's


/* Local functions */
pcb_t * get_pcb() {
    pcb_t * pcb;
    asm volatile(
        "movl %%esp, %%eax;"
        "andl $0x7FFFF, %%eax;"     // mask out the top 9 bits
        "movl %%eax, %0;"
        :"=r"(pcb)
        :
        :"%eax"
    );
    return pcb;
}


/* System call functions */
int32_t system_halt(uint8_t status) {
    pcb_t * pcb = get_pcb();
    if (pcb->pid < 3) { // don't halt the shell or the init process
        return 0;
    }
    
    // close all files
    int i;
    for (i = 0; i < MAX_FILES; i++) curr_pcb[pcb->pid] -> file_desc_tb[0].flag = 0;
    // TODO set the process to inactive

    // restore parent's esp and ebp     // TODO check the asm error here: "register 'bp' has a fixed purpose and may not be clobbered in an asm statement"
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        :"=r"(pcb->saved_esp), "=r"(pcb->saved_ebp)
        :"%esp", "%ebp"
    );

    // restore parent's paging      // TODO: check if this is correct. math was done in a hurry
    uint32_t parent_pid = pcb->parent_pid;
    uint32_t parent_pde = _8MB - (parent_pid + 1) * _4MB;
    uint32_t parent_pte = _8MB - (parent_pid + 1) * _4KB;
    uint32_t parent_page = _8MB - (parent_pid + 1) * _4KB;
    uint32_t parent_page_dir = _8MB - (parent_pid + 1) * _4KB;
    uint32_t parent_page_table = _8MB - (parent_pid + 1) * _4KB;

    // TODO flush tlb
    flushTLB();

    // TODO reset tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (parent_pid + 1) * _8KB;
    // TODO jump to end of execute asm code (need to add the label); TODO add a leave and ret to the end
    return status;  // TODO check if this is correct, or if we need to return 0/-1
}
int32_t system_execute(const uint8_t * command) {
    uint8_t command_name[32] = {0};     // first word of the command
    uint32_t args[128] = {0};
    int8_t exe[40] = {0};  // header occupies first 40 bytes of the file
    struct dentry command_dentry;
    uint32_t command_inode;
    void * entry_point;                 // entry point of the executable
	union dirEntry d;

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
    
    // find first active pcb
	int pcb_index = 0;
	while (pcb_index < MAX_PROCESSES && curr_pcb[pcb_index]->active) pcb_index++;
	if (pcb_index == MAX_PROCESSES) return -1;  // no available pcb's

    // set up paging for the program (flush TLB)	// TODO @Vasilis
	d.val = 3;		//sets P and RW bits
	d.whole.add_22_31 = (_8MB + _4MB * pcb_index) >> 22;
	chgDir(32, d);	//32 = 128 / 4 (all programs are in the 128-132 MiB vmem page
	flushTLB();

    // put arguments in pcb
    strcpy(curr_pcb[pcb_index]->args, args);    // TODO might need to typecast to (int8_t *)

    // load in data
	read_data(command_inode, 0, (uint8_t *)(_128MB + PROC_OFFSET), KERNEL_STACK_BOTTOM);	// results in page fault for now, need to set up paging 

    // set up and load pcb (setup fd[0] and fd[1])
    curr_pcb[pcb_index]->pid = pcb_index;
    curr_pcb[pcb_index]->active = 1;
    curr_pcb[pcb_index]->parent_pid = curr_pcb[pcb_index]->pid;     // might need to change this to a pointer to the parent pcb

    curr_pcb[pcb_index]->file_desc_tb[0].flag = 1;  // TODO stdin
    curr_pcb[pcb_index]->file_desc_tb[1].flag = 1;  // TODO stdout
    for (i = 2; i < MAX_PROCESSES; i++) {
        curr_pcb[pcb_index]->file_desc_tb[i].flag = 0;
    }

    // TODO check if this is correct/needed
    // asm volatile(
    //     "movl %esp, %eax;"
    //     "movl %ebp, %ebx;"
    //     : "=a" (curr_pcb[pcb_index]->saved_esp), "=b" (curr_pcb[pcb_index]->saved_ebp)
    // );

    // set up tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (pcb_index + 1) * _8KB - 4;

    // context switch
    asm volatile(
        "pushl %0;"     // push kernel ds
        "pushl %1;"     // push esp
        "pushfl;"       // push eflags
        "popl %eax;"    // pop eflags
        "orl $0x200, %eax;"     // set IF bit
        "pushl %eax;"   // push back
        "pushl %2;"     // push cs
        "pushl %3;"     // push eip
        "iret;"
        :
        : "r" (KERNEL_DS), "r" (curr_pcb[pcb_index]->saved_esp), "r" (USER_CS), "r" (entry_point)
        : "eax"
    );

    return 0;
}

/*int32_t sys_open(const uint8_t* filename)
* DESCRIPTION: Open systemcall that initializes the file descriptor table entry
* INPUTS: uint8_t filename
* OUTPUTS: -1 upon failure, 0 upon success
* 
 */
int32_t sys_open (const uint8_t* filename){
     int file_type = get_filetype(filename);
     int found_open_fd=0;
     int i;

     if(file_type == -1){
          return -1;
     }
     //return -1 if array is full
     if (file_type == 0){
          //set the f_op fields to RTC
    
          for (i=2; i<8; i++){ //for the length of the file array, excluding stdin/stdout
               if(file_desc_tb[i].flag == 0){  //if entry dne
                    found_open_fd=1;
                    file_desc_tb[i].f_op->read=rtc_read;
                    file_desc_tb[i].f_op->write=rtc_write;
                    file_desc_tb[i].f_op->open=rtc_open;
                    file_desc_tb[i].f_op->close=rtc_close;
                    file_desc_tb[i].flag=1;
                    file_desc_tb[i].file_position=0;
                    break;
               }
          }
          if(found_open_fd ==0){
               return -1;
          }
     }
     else if(file_type ==1){
          //set the f_op fields to directory
          for (i=2; i<8; i++){ //for entire file array
               if(file_desc_tb[i].flag == 0){  //if entry dne
                    found_open_fd=1;
                    file_desc_tb[i].f_op->read=dir_read;
                    file_desc_tb[i].f_op->write=dir_write;
                    file_desc_tb[i].f_op->open=dir_open;
                    file_desc_tb[i].f_op->close=dir_close;
                    file_desc_tb[i].flag=1;
                    file_desc_tb[i].file_position=0;
                    break;
               }
          }
          if(found_open_fd ==0){ //if table is full
               return -1;
          }
     }
     else if(file_type == 2){
          //set the f_op fields to regular file
          for (i=2; i<8; i++){ //for entire file array
               if(file_desc_tb[i].flag == 0){  //if entry dne
               //HOW WOULD I WRITE TO TERMINAL??
                    found_open_fd=1;
                    file_desc_tb[i].f_op->read=file_read;
                    file_desc_tb[i].f_op->write=file_write;
                    file_desc_tb[i].f_op->open=file_open;
                    file_desc_tb[i].f_op->close=file_close;
                    file_desc_tb[i].flag=1;
                    file_desc_tb[i].file_position=0;
                    file_desc_tb[i].inode=get_inode(filename);
                    break;
               }
          }
          if(found_open_fd ==0){ //if table is full
               return -1;
          }
		}
		return 0;
}

int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    /*
    * TODO: make assembly file with jmp table to jump to these c 
    * functions
    */

     //fd is an integer index into the file descriptor table
     //access the file operations table ptr within the desctable index and 
     //tells you what specific system call to execute

     // if(file_desc_tb[fd].fotp == 0){
          
     // }

     if(fd > 1){
          int32_t valid = (file_desc_tb[fd].f_op)->write(fd, buf, nbytes);
          if(valid != -1){
            return nbytes;
          }
		  return -1;
     }
     return -1;
}

int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
     
     if(fd > 1){
          int32_t valid = (file_desc_tb[fd].f_op)->read(fd, buf, nbytes);
          if(valid != -1){
            file_desc_tb[fd].file_position += nbytes;
            return valid;
          }
		  return -1;
     }
     return -1;
}

int32_t sys_close (int32_t fd){
    if(fd == 1 || fd == 0)
        return -1;

    file_desc_tb[fd].f_op->read = NULL;
	file_desc_tb[fd].f_op->write = NULL;
	file_desc_tb[fd].f_op->open = NULL;
	file_desc_tb[fd].f_op->close = NULL;
    file_desc_tb[fd].inode = 0;
    file_desc_tb[fd].file_position = 0;
    file_desc_tb[fd].flag = 0; 

    return 0;
}
