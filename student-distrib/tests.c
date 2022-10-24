#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */				// TODO: improve tests by printing useful exception info

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test() {
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}


/* IDT Test - Divide by Zero
 * 
 * Asserts that divide by zero exception is handled
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, divide by zero exception
 * Files: x86_desc.h/S
 */
int idt_test_div_by_zero() {
	TEST_HEADER;

	int result;
	// int i = 16/0;		/* Suppress compiler warning */
	result = PASS;

	return result;
}


/* Paging Test - Page Fault
 * 
 * Asserts that page fault exception is handled
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Page fault exception
 * Files: x86_desc.h/S
 */
int page_fault() {
	TEST_HEADER;

	int result;
	printf("size of unions: %d, %d\n", sizeof(union dirEntry), sizeof(union tblEntry));
	asm("movl 0, %eax\n\t");
	result = PASS;

	return result;
}


/* Checkpoint 2 tests */
int buffer_test(){
	//check if buffer overflow is handled
	// TEST_HEADER;
	// int result, out, i;
	// char buffer_test2[128];
	// for (i=0; i<128; i++){
	// buffer_test2[i]= "h";
	// }
	// terminal_open(fd);
	// out = terminal_write(int32_t fd, buffer_test2, 129);		//change file
	// terminal_close(fd);
	// result= PASS;
	// if(out != -1){
	// 	assertion_failure();
	// 	result == FAIL;
	// }

	// return result;
	return 0;
}

// int terminal_read_test(){
// 	TEST_HEADER;
// 	char buffer_test3[128];
// 	while(1){
// 		terminal_read(0, buffer_test3, 128);
// 		terminal_write(0, buffer_test3, 128);
// 	}
// 	return PASS;
// }

int rtc_freq_invalid_test(int freq)
{	
	// TEST_HEADER;
	// //ensure frequency is limited to 1024
	// int result, out;
	// int frequency = freq;
	// int *buffer_rtc = &frequency;
	// result = PASS;
	// out = rtc_write(int32_t fd, buffer_rtc, 4); //change file
	// if (out != -1){
	// 	assertion_failure();
	// 	result = FAIL;
	// }
	// return result;
	return 0;
}

int read_file_by_name_test(){
	TEST_HEADER;
	struct dentry dent;
	read_dentry_by_name("frame0.txt", &dent); //change this
	inode
	file_open("frame0.txt");
	file_read("frame0.txt", void* buf, int32_t n)
	return PASS;
}

// int change_rtc_freq_test(){
// 	TEST_HEADER;
// 	int result, out, j, i;
// 	int frequency = 1024;
// 	int *buffer_rtc = &frequency;
// 	result = PASS;
// 	for (i=6; i<15; i++){
// 			*buffer_rtc = 32768 >>  i - 1;
// 			out = rtc_write(0, buffer_rtc, 4); //change file
// 			if (out != -1){
// 				for (j=0; j<5; j++){
// 				printf("rate = %d", i);
// 				}
// 			}
// 	}
// 	return result;
// }

int change_rtc_freq_test(int rate){
	TEST_HEADER;
	int out;
	clear();
	int frequency = 32768 >>  rate - 1;
	int *buffer_rtc = &frequency;
	rtc_open(0);
	out = rtc_write(0, buffer_rtc, 4);
	if (out != -1){
		while(1){
			rtc_read(0, buffer_rtc, 4);
			putc_term('1');
		}
	}
	return PASS;
}


int list_all_files_test(){
	int i;
	uint8_t ls_buffer[33*63];
	dir_open(0);
	dir_read(0, ls_buffer, 33);
	for(i=0; i<33*63; i++) putc_term(ls_buffer[i]);
	
	return PASS;
}




/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("page fault", page_fault());
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("divide by zero", idt_test_div_by_zero());
	//TEST_OUTPUT("buffer overflow", buffer_test());
	//TEST_OUTPUT("invalid frequency", rtc_freq_invalid_test(32768));
	//TEST_OUTPUT("invalid frequency", rtc_freq_invalid_test(16384));
	//TEST_OUTPUT("invalid frequency", rtc_freq_invalid_test(8192));
	//TEST_OUTPUT("invalid frequency", rtc_freq_invalid_test(4096));
	//TEST_OUTPUT("invalid frequency", rtc_freq_invalid_test(2048));
	//TEST_OUTPUT("read file by name", read_file_by_name_test());
	//TEST_OUTPUT("list all files", list_all_files_test());
	//TEST_OUTPUT("change rtc frequency", change_rtc_freq_test(15));
	//TEST_OUTPUT("change rtc frequency", change_rtc_freq_test(2));	
	//TEST_OUTPUT("terminal test", terminal_read_test());

}
