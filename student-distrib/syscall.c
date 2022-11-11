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


/* Local variables */
struct fap fap_func_arr[3];
int8_t check_exe[4] = {0x7f, 0x45, 0x4c, 0x46};  // first 4 bytes identifying an executable
pcb_t* curr_pcb[MAX_PROCESSES] = {(pcb_t*)(_8MB - 2 * _8KB),
								  (pcb_t*)(_8MB - 3 * _8KB),
								  (pcb_t*)(_8MB - 4 * _8KB),
								  (pcb_t*)(_8MB - 5 * _8KB),
								  (pcb_t*)(_8MB - 6 * _8KB),
								  (pcb_t*)(_8MB - 7 * _8KB)};  // array of pointers to pcb's
struct fap terminal_op_table = {terminal_read, terminal_write, terminal_open, terminal_close};
struct fap rtc_op_table = {rtc_read, rtc_write, rtc_open, rtc_close};
struct fap dir_op_table = {dir_read, dir_write, dir_open, dir_close};
struct fap file_op_table = {file_read, file_write, file_open, file_close};


/* Local functions */

/*
 * get_PCB
 * DESCRIPTION: initializes PCB
 * INPUTS: NONE
 * OUTPUTS: intialized PCB  
 */
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


/* defines the file descriptor array, only to check 
compile errors for sys calls, comment out later once
PCB is implemented*/
void set_fda(){ 
	// unsigned i;
	// for(i = 0; i < 8; i++){
	// 	file_desc_tb[i].f_op->read = NULL;
	// 	file_desc_tb[i].f_op->write = NULL;
	// 	file_desc_tb[i].f_op->open = NULL;
	// 	file_desc_tb[i].f_op->close = NULL;
	// 	file_desc_tb[i].flag = 0;
	// 	file_desc_tb[i].file_position = 0;
	// }

	return;
}


/* System call functions */

/*
 * system_halt
 * DESCRIPTION: terminates process as well as returns specified value to its parent process
 * INPUTS: status
 * OUTPUTS: status 
 */
int32_t sys_halt(uint8_t status) {
    union dirEntry d;
    pcb_t * pcb = get_pcb();
    union dirEntry d;

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
        :"r"(pcb->saved_esp), "r"(pcb->saved_ebp)
        :"%esp", "%ebp"
    );

    // restore parent's paging      // TODO: check if this is correct. math was done in a hurry
    uint32_t parent_pid = pcb->parent_pid;
    d.val = 7;		//sets P and RW bits
	d.whole.add_22_31 = (_8MB + _4MB * parent_pid) >> 22;
	chgDir(32, d);	//32 = 128 / 4 (all programs are in the 128-132 MiB vmem page
    flushTLB();

    // TODO reset tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (parent_pid + 1) * _8KB;
    // TODO jump to end of execute asm code (need to add the label); TODO add a leave and ret to the end
    asm volatile(
        "movl %%ebx, %%esp;"
        "movl %%edx, %%ebp;"
        "movl %%ecx, %%eax;"
        "jmp execute_ret;"
        :
        :"b"(pcb->saved_esp), "d"(pcb->saved_ebp), "c"(status)
    );
    return status;  // TODO check if this is correct, or if we need to return 0/-1
}

/*
 * system_execute
 * DESCRIPTION: loads and executes a new program
 * INPUTS: command (space separated squence of words)
 * OUTPUTS: returns 0 if successful, returns -1 if program isn't executable
 */
int32_t sys_execute(const uint8_t * command) {
    uint8_t command_name[32] = {0};     // first word of the command
    uint32_t args[128] = {0}; //
    int8_t exe[40] = {0};  // header occupies first 40 bytes of the file
    struct dentry command_dentry;
    uint32_t command_inode;
    void * entry_point;                 // entry point of the executable
	union dirEntry d;

    // copy command into a buffer until /0 or /n is reached
    int i = 0;
    while (command[i] != '\n' && command[i] != ' ' && i < 32) {    //only want first word
        command_name[i] = command[i];
        i++;
    }
    // check if command was an enter press
    if (command_name[0] == 0) return 0;

    // check if command is quit terminal
    if (strncmp((int8_t*)command_name, "quit", 4) == 0) {
        return sys_halt(0);
    }
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
    if (command_dentry.ft != 2)
        return -1; // not a file
    
    command_inode = command_dentry.ind;

    // check ELF header to see if it is a executable (read_data first 4 bytes)
    read_data(command_inode, 0, (uint8_t *)exe, 40);
    if (strncmp(exe, check_exe, 4))
        return -1; // not an executable
    
    entry_point = (void *)(((uint32_t)(exe[24])) + (((uint32_t)(exe[25])) << 8) + (((uint32_t)(exe[26])) << 16) + (((uint32_t)(exe[27])) << 24));
    
    // find first active pcb
	int pcb_index = 0;
	while (pcb_index < MAX_PROCESSES && curr_pcb[pcb_index]->active) pcb_index++;
	if (pcb_index == MAX_PROCESSES) return -1;  // no available pcb's

    // set up paging for the program (flush TLB)
	d.val = 7;		//sets P, RW, and US bits 0b111
    d.whole.ps = 1;
	d.whole.add_22_31 = (_8MB + _4MB * pcb_index) >> 22; 
	chgDir(32, d);	//32 = 128 / 4 (all programs are in the 128-132 MiB vmem page)
	//flushTLB();

    // put arguments in pcb
    strcpy((int8_t *)curr_pcb[pcb_index]->args, (const int8_t *)args);

    // load in data
	read_data(command_inode, 0, (uint8_t *)(_128MB + PROC_OFFSET), KERNEL_STACK_BOTTOM);

    // set up and load pcb (setup fd[0] and fd[1])
    curr_pcb[pcb_index]->pid = pcb_index;
    curr_pcb[pcb_index]->active = 1;
    curr_pcb[pcb_index]->parent_pid = curr_pcb[pcb_index]->pid;     // might need to change this to a pointer to the parent pcb

    curr_pcb[pcb_index]->file_desc_tb[0].flag = 1;
    curr_pcb[pcb_index]->file_desc_tb[0].f_op = &terminal_op_table;
    curr_pcb[pcb_index]->file_desc_tb[1].flag = 1;
    curr_pcb[pcb_index]->file_desc_tb[1].f_op = &terminal_op_table;
    for (i = 2; i < MAX_PROCESSES; i++) {
        curr_pcb[pcb_index]->file_desc_tb[i].flag = 0;
    }

    // TODO check if this is correct/needed
    asm volatile(
        "movl %%esp, %%eax;"
        "movl %%ebp, %%ebx;"
        : "=a" (curr_pcb[pcb_index]->saved_esp), "=b" (curr_pcb[pcb_index]->saved_ebp)
    );
    // 0x083FFFFC
    uint32_t user_sp = 

    // set up tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (pcb_index + 1) * _8KB - 4;

    // context switch
    asm volatile(
        "pushl %0;"     // push kernel ds
        "pushl %1;"     // push esp
        "pushfl;"       // push eflags
        "popl %%eax;"    // pop eflags
        "orl $0x200, %%eax;"     // set IF bit
        "pushl %%eax;"   // push back
        "pushl %2;"     // push cs
        "pushl %3;"     // push eip
        "iret;"
        "execute_ret:"
        "leave;"
        "ret;"
        :
        : "r" (USER_DS), "r" (user_sp), "r" (USER_CS), "r" (entry_point)
        : "eax"
    );

    return 0;
}


int32_t get_args(uint8_t * buf, int32_t nbytes) {
    pcb_t * pcb = get_pcb();
    if (buf == NULL || nbytes < 0) return -1;
    if (nbytes > 128) nbytes = 128;
    strncpy((int8_t *)buf, (const int8_t *)pcb->args, nbytes);
    return 0;
}


/*int32_t sys_open(const uint8_t* filename)
* DESCRIPTION: Open systemcall that initializes the file descriptor table entry
* INPUTS: uint8_t filename
* OUTPUTS: -1 upon failure, 0 upon success
* 
 */
int32_t sys_open (const uint8_t* filename){
    pcb_t * pcb = get_pcb();
    struct dentry dentry;
    int file_type = get_filetype(filename);
    int found_open_fd=0;
    int i;

    if(filename == NULL || *filename == '\0' || read_dentry_by_name(filename, &dentry) == -1 || file_type == -1){
        return -1;
    }

    //return -1 if array is full
    if (file_type == 0){
        //set the f_op fields to RTC

        for (i = 2; i < MAX_FILES; i++){ // for the length of the file array, excluding stdin/stdout
            if(pcb->file_desc_tb[i].flag == 0){  // if entry dne
                found_open_fd=1;
                pcb->file_desc_tb[i].f_op = &rtc_op_table;
                pcb->file_desc_tb[i].flag=1;
                pcb->file_desc_tb[i].file_position=0;
                break;
            }
        }
        if(found_open_fd ==0){
            return -1;
        }
    }
    else if(file_type == 1){
        //set the f_op fields to directory
        for (i=2; i<8; i++){ //for entire file array
            if(pcb->file_desc_tb[i].flag == 0){  //if entry dne
                found_open_fd=1;
                pcb->file_desc_tb[i].f_op = &dir_op_table;
                pcb->file_desc_tb[i].flag=1;
                pcb->file_desc_tb[i].file_position=0;
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
            if(pcb->file_desc_tb[i].flag == 0){  //if entry dne
            //HOW WOULD I WRITE TO TERMINAL??
                found_open_fd=1;
                pcb->file_desc_tb[i].f_op = &file_op_table;
                pcb->file_desc_tb[i].flag=1;
                pcb->file_desc_tb[i].file_position=0;
                pcb->file_desc_tb[i].inode=get_inode(filename);
                break;
            }
        }
        if(found_open_fd ==0){ //if table is full
            return -1;
        }
    }
    return 0;
}

/*
 * sys_write
 * DESCRIPTION: writes data to the terminal or to a device (RTC)
 * INPUTS: file descriptor, buffer, number of bytes
 * OUTPUTS: returns number of bytes written , returns -1 if its invalid 
 */
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    //fd is an integer index into the file descriptor table
    //access the file operations table ptr within the desctable index and 
    //tells you what specific system call to execute

    // if(file_desc_tb[fd].fotp == 0){
        
    // }

    pcb_t * pcb = get_pcb();

    if(fd > 1){
        int32_t valid = (pcb->file_desc_tb[fd].f_op)->write(fd, buf, nbytes);
        if(valid != -1){
            return nbytes;
        }
        return -1;
    }
    return -1;
}

/*
 * sys_read
 * DESCRIPTION: reads data from the keyboard, a file, device (RTC), or directory
 *              calls upon specific read funciton based on file descriptor (fd)
 * INPUTS: file descriptor, buffer, number of bytes
 * OUTPUTS: returns number of bytes read and updates file position, returns -1
 *          if its invalid 
 */
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
    pcb_t * pcb = get_pcb();

    if(fd > 1){
        int32_t valid = (pcb->file_desc_tb[fd].f_op)->read(fd, buf, nbytes);
        if(valid != -1){
        pcb->file_desc_tb[fd].file_position += nbytes;
        return valid;
        }
        return -1;
    }
    return -1;
}

/*
 * sys_close
 * DESCRIPTION: closes and makes specific file descriptor available 
 * INPUTS: the file descriptor
 * OUTPUTS: sets all elements in file descriptor to uninitialized valuesand  returns 0
 */
int32_t sys_close (int32_t fd){
    pcb_t * pcb = get_pcb();

    if(fd == 1 || fd == 0)
        return -1;

    pcb->file_desc_tb[fd].f_op->read = NULL;
	pcb->file_desc_tb[fd].f_op->write = NULL;
	pcb->file_desc_tb[fd].f_op->open = NULL;
	pcb->file_desc_tb[fd].f_op->close = NULL;
    pcb->file_desc_tb[fd].inode = 0;
    pcb->file_desc_tb[fd].file_position = 0;
    pcb->file_desc_tb[fd].flag = 0; 

    return 0;
}
