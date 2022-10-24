/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"

#define RTC_TEST_EN 0
/* Local variables */
volatile uint8_t rtc_status = 0;
int interrupt_count = 0;
int i_rtc =15; //rate is 15 for test
int frequency = 32768 >> (15 - 1);  //rate is 15 for test, maximum theoretical frequency is bit-shifted number
/*
 * rtc_init
 * DESCRIPTION: Initializes the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Enables the RTC, sets the frequency to the default
 */
void rtc_init(void) {
    outb(RTC_REG_B, RTC_PORT);          /* select register B, and disable NMI */
    char prev = inb(RTC_DATA);          /* read the current value of register B */
    outb(RTC_REG_B, RTC_PORT);          /* set the index again (a read will reset the index to register D) */
    outb(prev | 0x40, RTC_DATA);        /* write the previous value ORed with 0x40 */
    // rtc_open(void);                         /* set the frequency to 2 Hz */

    enable_irq(RTC_IRQ);                /* enable interrupts */
    rtc_status = 1;                     /* set the status to open */
}


/*
 * rtc_read
 * DESCRIPTION: Reads data from the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: Wait for an interrupt to occur, then return 0
 * SIDE EFFECTS: Blocks until an interrupt occurs
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) {
        return -1;                          /* if the buffer is NULL, the call returns -1 */
    }
    /* wait for the next interrupt */
    while (rtc_status == 0) {
        /* do nothing */
    }
    rtc_status = 0;
    return 0;
}


/*
 * rtc_write        // TODO virtualization
 * DESCRIPTION: Writes data to the RTC, sets the frequency to the given rate
 * INPUTS: freq - the rate to change the frequency to
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Changes the frequency of the RTC
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {      // TODO must get its input parameter through a buffer and not read the value directly
    if (buf == NULL) {
        return -1;                          /* if the buffer is NULL, the call returns -1 */
    }
    int i;
    int rate = 0;
    int freq = *((int*)buf);    // TODO check if this is the right way to do this
    for (i = 6; i < 16; i++) {              /* parameter check */
        if (freq == (32768 >> (i - 1))) {   //maximum frequency theoretically
            rate = i;
            break;
        }
    }
    if (rate == 0) {        
        return -1;                          /* if the frequency is not valid, the call returns -1 */
    }

    rate &= 0x0F;                           /* rate must be above 6 and not over 15 */
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | rate, RTC_DATA);   /* write the previous value ORed with 0x40 */
    return 0;
}   // TODO do I have to disable interrupts for the duration of this function?

/*
 * rtc_change_freq
 * DESCRIPTION: Changes the frequency of the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Changes the frequency of the RTC to 2 Hz
 */
int32_t rtc_open(const uint8_t* filename) {                         /* rate must be above 2 and not over 15 */
    if (filename == NULL) {    // TODO check descriptors
        return -1;                          /* if the named file does not exist or no descriptors are free, the call returns -1 */
    }
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | RTC_BASE_RATE, RTC_DATA);   /* write only our rate to A */
    return 0;
}   // TODO do I have to disable interrupts for the duration of this function?


/*
 * rtc_close
 * DESCRIPTION: Closes the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Makes RTC available for return from later calls to open
 */
int32_t rtc_close(int32_t fd) {  // TODO useless until virtualization
    if (fd == NULL) {            // You should not allow the user to close the default descriptors (0 for input and 1 for output)  
        return -1;                          /* trying to close an invalid descriptor returns -1 */
    }
    rtc_status = 0;                         /* set the status to closed */
    return 0;
}

/*
 * rtc_handler
 * DESCRIPTION: Handles the RTC interrupt
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Sends an EOI to the RTC
 */
void rtc_handler(void) {
    #if (RTC_TEST_EN == 1)
    int out;
    int flag;
    interrupt_count++;
	if (interrupt_count/(*buffer_rtc) == 4){
		*buffer_rtc = 32768 >>  i_rtc - 1;
		out = rtc_write(0, buffer_rtc, 4); //change file
		interrupt_count = 0;
        clear();
        putc_term('\n');
		if (out == -1){
			printf("STOPPP: %d", i_rtc);
            //i_rtc ++;
            flag=0;
		}else flag=1;
        i_rtc --;
    }
    if (flag) putc_term('1');
    #endif
    rtc_status=1;
    outb(RTC_REG_C, RTC_PORT);              /* select register C */
    inb(RTC_DATA);                          /* just throw away contents */
    send_eoi(RTC_IRQ);                      /* send EOI */
    
}
