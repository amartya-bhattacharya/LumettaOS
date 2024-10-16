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
//struct fap fap_func_arr[3];
const static int8_t check_exe[4] = {0x7f, 0x45, 0x4c, 0x46};  // first 4 bytes identifying an executable
static struct fap terminal_op_table = {.read = terminal_read, .write = terminal_write, .open = terminal_open_fail, .close = terminal_close_fail};
static struct fap rtc_op_table = {.read = rtc_read, .write = rtc_write, .open = rtc_open, .close = rtc_close};
static struct fap dir_op_table = {.read = dir_read, .write = dir_write, .open = dir_open, .close = dir_close};
static struct fap file_op_table = {.read = file_read, .write = file_write, .open = file_open, .close = file_close};


/* Local functions */

/* defines the file descriptor array, only to check 
compile errors for sys calls, comment out later once
PCB is implemented*/
/*void set_fda(){ 
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
}*/


/* System call functions */

/*
 * system_halt
 * DESCRIPTION: terminates process as well as returns specified value to its parent process
 * INPUTS: status
 * OUTPUTS: status 
 */
int32_t sys_halt(uint8_t status) {
    union dirEntry d;
    pcb_t* pcb = get_pcb();

    // close all files
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        pcb->file_desc_tb[0].flag = 0;
        pcb->active = 0;
    }

    // printf("halt: pid: %d, parent pid: %d\n", pcb->pid, pcb->parent_pid);
    if (pcb->parent_pid == -1) {
        pcb->pid--;
        clear_term();
        sys_execute((uint8_t*)"shell");
    }
    
    // restore parent's paging
    d.val = 7;		//sets P and RW bits
    d.whole.ps = 1;
	d.whole.add_22_31 = (_8MB + _4MB * pcb->parent_pid) >> 22; //bit shift by 22 to access correct address
	chgDir(32, d);	//32 = 128 / 4 (all programs are in the 128-132 MiB vmem page
    flushTLB();

    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (pcb->parent_pid) * _8KB - 4; //subtract 4 for padding

    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "movsbl %2, %%eax;"            
        "jmp execute_ret;"
        :
        :"r"(pcb->saved_esp), "r"(pcb->saved_ebp), "r"(status)
    );
    
    return status;
}


/*
 * system_execute
 * DESCRIPTION: loads and executes a new program
 * INPUTS: command (space separated squence of words)
 * OUTPUTS: returns 0 if successful, returns -1 if program isn't executable
 */
int32_t sys_execute(const uint8_t * command) {
    uint8_t command_name[MAX_CMD_LEN] = {0};     // first word of the command
    uint8_t args[128] = {0}; //args buffer is 128 in length including null char
    uint8_t exe[40] = {0};  // header occupies first 40 bytes of the file
    struct dentry command_dentry;
    uint32_t command_inode;
    uint32_t entry_point;                 // entry point of the executable
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
    if (strncmp((int8_t*)command_name, "quit", 4) == 0) { //4 bytes long string
        (void)sys_halt(0);
    }
    // check rest of command for arguments
    if (command[i] == ' ') {
        while (command[i] == ' ') i++;  // skip over spaces
        int j = 0;
        while (command[i + j] != '\n' && command[i + j] != '\0' && i + j < 128) { //for the length of the buffer
            args[j] = command[i + j];
            // printf("args: %s\n", args[0]);
            j++;
        }
    }
    
    if (read_dentry_by_name(command_name, &command_dentry))
        return -1; // read failed

    if (command_dentry.ft != 2)     //file desc type
        return -1; // not a file
    
    command_inode = command_dentry.ind;

    // check ELF header to see if it is a executable (read_data first 4 bytes)
    read_data(command_inode, 0, (uint8_t *)exe, 40);        //40 bytes
    if (strncmp((int8_t *) exe, (int8_t *) check_exe, 4))   //4 bytes
        return -1; // not an executable

    // get entry point from ELF header
    entry_point = (((uint32_t)(exe[24]) & 0xFF) + (((uint32_t)(exe[25]) & 0xFF) << 8) + (((uint32_t)(exe[26]) & 0xFF) << 16) + (((uint32_t)(exe[27]) & 0xFF) << 24)); //bits [24:27] of executable contain EIP
    // find first active pcb
	int pcb_index = 0;
	while (pcb_index < MAX_PROCESSES && curr_pcb[pcb_index]->active) pcb_index++;
	if (pcb_index == MAX_PROCESSES) return -1;  // no available pcb's

    // put arguments in pcb
    strcpy((int8_t *)curr_pcb[pcb_index]->args, (const int8_t *)args);

    // set up paging for the program (flush TLB)
	d.val = 7;		//sets P, RW, and US bits 0b111
    d.whole.ps = 1;
	d.whole.add_22_31 = (_8MB + _4MB * pcb_index) >> 22; 
	chgDir(32, d);	//32 = 128 / 4 (all programs are in the 128-132 MiB vmem page)
	flushTLB();

    // load in data
	read_data(command_inode, 0, (uint8_t *)(_128MB + PROC_OFFSET), KERNEL_STACK_BOTTOM);

    // set up and load pcb (setup fd[0] and fd[1])
    curr_pcb[pcb_index]->pid = pcb_index;
    curr_pcb[pcb_index]->active = 1;
    curr_pcb[pcb_index]->parent_pid = curr_pcb[pcb_index]->pid - 1;
    curr_pcb[pcb_index]->saved_esp = _8MB - 1;
    curr_pcb[pcb_index]->file_desc_tb[0].flag = 1;
    curr_pcb[pcb_index]->file_desc_tb[0].f_op = &terminal_op_table;
    curr_pcb[pcb_index]->file_desc_tb[1].flag = 1;
    curr_pcb[pcb_index]->file_desc_tb[1].f_op = &terminal_op_table;
    for (i = 2; i < MAX_PROCESSES; i++) {
        curr_pcb[pcb_index]->file_desc_tb[i].flag = 0;
    }

    // print pid and parent pid
    // printf("execute: pid: %d, parent pid: %d\n", curr_pcb[pcb_index]->pid, curr_pcb[pcb_index]->parent_pid);
    

    // 0x083FFFFC
    uint32_t user_sp = _132MB - 4;  //find user space

    // set up tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - curr_pcb[pcb_index]->pid * _8KB - 4;  //find EIP
	// asm volatile(
	// 	"movl %%cr3, %0;"
	// 	"movl %%esp, %1;"
	// 	: "=r" (tss.cr3), "=r" (tss.esp0)
	// );

    // asm volatile(
	// 	"movl %%cr3, %;"
	// 	: "=r" (tss.cr3)
	// );

    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r" (curr_pcb[pcb_index]->saved_esp), "=r" (curr_pcb[pcb_index]->saved_ebp)
    );
    
    // context switch
    asm volatile(
        "movw %0, %%ax;"
        "movw %%ax, %%ds;"
        "pushl %0;"     // push kernel ds  
        "pushl %1;"   // push user sp
        "pushfl;"       // push eflags
        "popl %%edx;"    // pop eflags
        "orl $0x200, %%edx;"     // set IF bit
        "pushl %%edx;"   // push back
        "pushl %2;"     // push cs
        "pushl %3;"     // push eip
        "iret;"
        "execute_ret:"
        "leave;"
        "ret;"
        :
        : "g" (USER_DS), "g" (user_sp), "g" (USER_CS), "g" (entry_point)
        : "%eax", "%edx"
    );

    // setup_context_switch(user_sp, entry_point);
	
    return 0;
}


/*int32_t get_args(uint8_t * buf, int32_t nbytes) {
    pcb_t * pcb = get_pcb();
    if (buf == NULL || nbytes < 0) return -1;
    if (nbytes > 128) nbytes = 128;
    strncpy((int8_t *)buf, (const int8_t *)pcb->args, nbytes);
    return 0;
}*/


/*int32_t sys_open(const uint8_t* filename)
* DESCRIPTION: Open systemcall that initializes the file descriptor table entry
* INPUTS: uint8_t filename
* OUTPUTS: -1 upon failure, fd index upon success
* 
 */
int32_t sys_open (const uint8_t* filename){
    pcb_t* pcb = get_pcb();
    struct dentry d;
    //int file_type = get_filetype(filename);
    int found_open_fd=0;
    int i;

    /*if(filename == NULL || *filename == '\0' || read_dentry_by_name(filename, &d) == -1 || file_type == -1){
        return -1;
    }*/

	if(read_dentry_by_name(filename, &d))
		return -1;

    //return -1 if array is full
    if (d.ft == 0){
        //set the f_op fields to RTC

        for (i = 2; i < MAX_FILES; i++){ // for the length of the file array, excluding stdin/stdout
            if(pcb->file_desc_tb[i].flag == 0){  // if entry dne
                found_open_fd=i;
                pcb->file_desc_tb[i].f_op = &rtc_op_table;
                pcb->file_desc_tb[i].flag=1;
                pcb->file_desc_tb[i].file_position=0;
                (pcb->file_desc_tb[i].f_op)->open(filename);
                break;
            }
        }
        if(found_open_fd ==0){
            return -1;
        }
    }
    else if(d.ft == 1){ 
        //set the f_op fields to directory
        for (i=2; i<8; i++){ //for entire file array
            if(pcb->file_desc_tb[i].flag == 0){  //if entry dne
                found_open_fd=i;
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
    else if(d.ft == 2){
        //set the f_op fields to regular file
        for (i=2; i<8; i++){ //for entire file array
            if(pcb->file_desc_tb[i].flag == 0){  //if entry dne
                found_open_fd=i;
                pcb->file_desc_tb[i].f_op = &file_op_table;
                pcb->file_desc_tb[i].flag=1;
                pcb->file_desc_tb[i].file_position=0;
                pcb->file_desc_tb[i].inode = d.ind;
                break;
            }
        }
        if(found_open_fd ==0){ //if table is full
            return -1;
        }
    }
    return found_open_fd;
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
	int32_t valid;

    if (pcb->file_desc_tb[fd].flag == 0){
        return -1;
    }
	//add values for fd ==0 and fd ==1
    if(fd > 1 && fd <=7 ){ //this means it is an RTC device
        valid = (pcb->file_desc_tb[fd].f_op)->write(fd, buf, nbytes);
        if(valid != -1){
            return nbytes; //this should be fine??
        }
        return -1;
    }
    else if(fd ==0){
        //stdin read-only for keyboard input--invalid
        return -1;
    }
    else if (fd ==1){
        //stdout write-only for terminal output
        valid = (pcb->file_desc_tb[fd].f_op)->write(fd, buf, nbytes);
        return valid;
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
    int32_t valid;
    /*if(fd > 1){
        valid = (pcb->file_desc_tb[fd].f_op)->read(fd, buf, nbytes);
        if(valid != -1){
            //pcb->file_desc_tb[fd].file_position += valid; //changed from nbytes to valid
            return valid;
        }
        return -1;
    }
    else if (fd ==0){
        valid = (pcb->file_desc_tb[fd].f_op)->read(fd, buf, nbytes);
        if(valid != -1){
            //pcb->file_desc_tb[fd].file_position += valid; //changed from nbytes to valid
            return valid;
        }
    }*/
    if (fd<0){
        return -1;
    }

    if(fd == 1){
        return -1;
    }
    if (fd > 7){
        return -1;
    }

    if (pcb->file_desc_tb[fd].flag == 0){
        return -1;
    }

	valid = (pcb->file_desc_tb[fd].f_op)->read(fd, buf, nbytes);
    return valid;
}

/*
 * sys_close
 * DESCRIPTION: closes and makes specific file descriptor available 
 * INPUTS: the file descriptor
 * OUTPUTS: sets all elements in file descriptor to uninitialized values and returns 0
 */
int32_t sys_close (int32_t fd){
    pcb_t * pcb = get_pcb();

    if (fd <= 1 || fd >= 8)
        return -1;

    if (pcb->file_desc_tb[fd].flag == 0){
        return -1;
    }
    // pcb->file_desc_tb[fd].f_op->close(fd);
    // pcb->file_desc_tb[fd].f_op->close = NULL;
    // pcb->file_desc_tb[fd].f_op->read = NULL;
	// pcb->file_desc_tb[fd].f_op->write = NULL;
	// pcb->file_desc_tb[fd].f_op->open = NULL;
    // pcb->file_desc_tb[fd].inode = 0;
    // pcb->file_desc_tb[fd].file_position = 0;
    pcb->file_desc_tb[fd].flag = 0; 

    return 0;
}

/*getargs
*DESCRIPTION: reads the program’s command line arguments into a user-level buffer
*INPUTS: buffer and number of bytes in argument
*OUTPUTS: If no arguments, or if arguments and a terminal NULL (0-byte) do not fit in buffer, return -1.
*SIDE EFFECTS: initialize the shell task’s argument data to the empty string
*/
int32_t sys_getargs (uint8_t* buf, int32_t nbytes){
    if (buf == NULL || nbytes <= 0){
        return -1;
    }

    pcb_t * pcb = get_pcb();
    
    // int i;
    // int flag = 0;

    // if (pcb->args[0] == 0){     // TODO double check this
    //     return -1;
    // }
    
    // for (i = 0; i < strlen((int8_t*)pcb->args +1; i++ ){  //for all of argument array
    //     if(pcb->args[i] == 0){
    //         flag = 1;
    //         break;
    //     }
    // }

    if (*(pcb->args) == '\0'){
        return -1;
    }

    if (nbytes <= strlen((int8_t*)pcb->args)) {
        return -1;
    }
    // printf("args: %s\n", pcb->args);
    memcpy((void *) buf, (void *) pcb->args, nbytes);

    return 0;
}

/*vidmap
*DESCRIPTION: call maps the text-mode video memory into user space at a pre-set virtual address
*INPUTS: screen start is pointer to variable in user memory 
*OUTPUTS: If screen start is inside user space then set it to user space vid_meme, if out of user space return -1.
*SIDE EFFECTS: sets screen start to a virtual address
*/
int32_t sys_vidmap (uint8_t** screen_start){
    if(((uint32_t)screen_start) < _128MB || ((uint32_t)screen_start) > _132MB)
        return -1;
    *screen_start = (uint8_t*)(_4MB * 33 + 0xB8000);    //0xB8000 is vidmem from lib.c
    return 0;
}

/*set_handler
*DESCRIPTION: does nothing
*INPUTS: signum and handler address in argument
*OUTPUTS: return -1.
*SIDE EFFECTS: NONE
*/
int32_t sys_sethandler (int32_t signum, void* handler_address){
    return -1;
}

/*sigreturn
*DESCRIPTION: does nothing
*INPUTS: NONE
*OUTPUTS: return -1.
*SIDE EFFECTS: NONE
*/
int32_t sys_sigreturn (void){
    return -1;
}
