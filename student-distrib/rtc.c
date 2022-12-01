/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"

/* Local variables */
volatile uint8_t rtc_interrupt_occurred = 0;    // flag for RTC interrupt
static volatile int rtc_counter = 0; // counter for RTC interrupts
static volatile int rtc_max_count = RTC_MAX_FREQ / RTC_BASE_FREQ; // max number of interrupts before RTC interrupt occurs

/* Local functions */
/* rtc_set_rate
 * DESCRIPTION: Sets the RTC rate to the given frequency
 * INPUTS: freq - frequency to set the RTC to
 * OUTPUTS: NONE
 * SIDE EFFECTS: Sets the RTC rate to the given frequency
 */
void rtc_set_rate(uint32_t freq) {
    int i;
    int rate = 0;

    for (i = 6; i < 16; i++) {              /* parameter check */
        if (freq == (32768 >> (i - 1))) {   /* kernel limited to 1024 Hz */
            rate = i;
            break;
        }
    }

    if (rate == 0)
        return;

    cli();
    rate &= 0x0F;                           /* rate must be above 6 and not over 15 */
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | rate, RTC_DATA);   /* write the previous value ORed with 0x40 */
    sti();
}


/*
 * rtc_init
 * DESCRIPTION: Initializes the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Enables the RTC, sets the frequency to the default
 */
void rtc_init(void) {
    cli();                              /* disable interrupts */
    outb(RTC_REG_B, RTC_PORT);          /* select register B, and disable NMI */
    char prev = inb(RTC_DATA);          /* read the current value of register B */
    outb(RTC_REG_B, RTC_PORT);          /* set the index again (a read will reset the index to register D) */
    outb(prev | 0x40, RTC_DATA);        /* write the previous value ORed with 0x40 */
    rtc_max_count = RTC_MAX_FREQ / RTC_BASE_FREQ;   /* set the max count to the default frequency */
    rtc_set_rate(RTC_MAX_FREQ);         /* set the frequency to 1024 Hz */
    enable_irq(RTC_IRQ);                /* enable interrupts */
    sti();
}


/*
 * rtc_read
 * DESCRIPTION: Reads data from the RTC
 * INPUTS: fd - file descriptor, buf - buffer to read from, nbytes - number of bytes to read
 * OUTPUTS: none
 * RETURN VALUE: Wait for an interrupt to occur, then return 0
 * SIDE EFFECTS: Blocks until an interrupt occurs
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL)
        return -1;                          /* if the buffer is NULL, the call returns -1 */

    while (!rtc_interrupt_occurred);        /* while the status is open, wait */
    rtc_interrupt_occurred = 0;             /* set the status to open */
    return 0;
}


/*
 * rtc_write
 * DESCRIPTION: Writes data to the RTC, sets the frequency to the given rate
 * INPUTS: fd - file descriptor, buf - buffer to read from, nbytes - number of bytes to write
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Changes the frequency of the RTC
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    if (buf == NULL || nbytes != 4)
        return -1;                          /* if the buffer is NULL, or not a 4-byte value, the call returns -1 */


    int i;
    int rate = 0;
    int freq = *((int*)buf);

    for (i = 6; i < 16; i++) {              /* parameter check */
        if (freq == (32768 >> (i - 1))) {   /* kernel limited to 1024 Hz */
            rate = i;
            break;
        }
    }

    if (rate == 0)
        return -1;                          /* if the frequency is not valid, the call returns -1 */

    cli();
    rtc_max_count = RTC_MAX_FREQ / freq;    /* set the max count to the requested frequency */
    sti();
    return nbytes;
}

/*
 * rtc_open
 * DESCRIPTION: Opens the RTC
 * INPUTS: filename - the name of the file to open
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Sets the RTC status to open
 */
int32_t rtc_open(const uint8_t* filename) {
    if (filename == NULL)
        return -1;                                      /* if the named file does not exist, the call returns -1 */

    rtc_max_count = RTC_MAX_FREQ / RTC_BASE_FREQ;       /* set the max count to the default frequency */
    rtc_interrupt_occurred = 0;                         /* set the status to open */
    return 0;
}


/*
 * rtc_close
 * DESCRIPTION: Closes the RTC
 * INPUTS: fd - file descriptor
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Makes RTC available for return from later calls to open
 */
int32_t rtc_close(int32_t fd) {
    if (fd == NULL)
        return -1;                          /* trying to close an invalid descriptor returns -1 */

    rtc_interrupt_occurred = 1;             /* set the status to closed */
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
    cli();                                  /* disable interrupts */
    #if (RTC_TEST_EN == 1)
    int out;
    int flag;
    int frequency = 32768 >> (15 - 1);  //rate is 15 for test, maximum theoretical frequency is bit-shifted number
    int i_rtc =15; //rate is 15 for test
    int interrupt_count = 0;
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
		} else flag=1;
        i_rtc --;
    }

    if (flag) putc_term('1');
    #endif
    outb(RTC_REG_C, RTC_PORT);              /* select register C */
    inb(RTC_DATA);                          /* just throw away contents */
    rtc_counter++;                          /* increment the counter */

    if (rtc_counter == rtc_max_count) {     /* if the counter reaches the max count */
        rtc_interrupt_occurred = 1;         /* set the status to closed */
        rtc_counter = 0;                    /* reset the counter */
    }

    send_eoi(RTC_IRQ);                      /* send EOI */
    sti();                                  /* enable interrupts */
}
