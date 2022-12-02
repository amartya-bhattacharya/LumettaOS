/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"

/* Local variables */
volatile int rtc_interrupt_occurred = 0;    // flag for RTC interrupt
volatile int rtc_counter = 0; // counter for RTC interrupts
int rtc_max_count = RTC_MAX_FREQ / RTC_BASE_FREQ; // max number of interrupts before RTC interrupt occurs

/* Local functions */
/* Set the RTC rate to the given frequency */
int rtc_set_rate(uint32_t freq);

/* rtc_set_rate
 * DESCRIPTION: Sets the RTC rate to the given frequency
 * INPUTS: freq - frequency to set the RTC to
 * OUTPUTS: NONE
 * RETURN VALUE: 1 on success, 0 on failure
 * SIDE EFFECTS: Sets the RTC rate to the given frequency
 */
int rtc_set_rate(uint32_t freq) {
    int rate = 0;
    int i;
    for (i = 6; i < 16; i++) {
        if (freq == (32768 >> (i - 1))) {    /* kernel limited to 1024 Hz */
            rate = i;
            break;
        }
    }

    if (rate == 0)
        return 0;
    
    rate &= 0x0F;                           /* rate must be above 6 and not over 15 */
    cli();                                  /* disable interrupts */
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | rate, RTC_DATA);   /* write the previous value ORed with 0xF0 */
    sti();                                  /* enable interrupts */
    return 1;
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
    #if RTC_VT_EN
    rtc_max_count = RTC_MAX_FREQ / RTC_BASE_FREQ;   /* set the max count to the default frequency */
    rtc_set_rate(RTC_MAX_FREQ);         /* set the frequency to 1024 Hz */
    #endif
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

    sti();
    while (!rtc_interrupt_occurred);        /* while the status is open, wait */
    rtc_interrupt_occurred = 0;             /* set the status to open */
    return 0;
}


/*
 * rtc_write
 * DESCRIPTION: Writes data to the RTC, sets the frequency to the given rate
 * INPUTS: fd - file descriptor, buf - buffer to read from, nbytes - number of bytes to write
 * OUTPUTS: none
 * RETURN VALUE: number of bytes written on success, -1 on failure
 * SIDE EFFECTS: Changes the frequency of the RTC
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    if (buf == NULL || nbytes != 4)
        return -1;                          /* if the buffer is NULL, or not a 4-byte value, the call returns -1 */

    uint32_t freq;
    if (!(freq = rtc_set_rate(*(uint32_t*)buf)))    /* if the frequency is invalid, return -1 */
        return -1;

    #if RTC_VT_EN
    cli();
    rtc_max_count = RTC_MAX_FREQ / freq;    /* set the max count to the requested frequency */
    sti();
    #endif
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

    #if RTC_VT_EN
    rtc_max_count = RTC_MAX_FREQ / RTC_BASE_FREQ;       /* set the max count to the default frequency */
    rtc_interrupt_occurred = 0;                         /* set the status to open */
    #else
    rtc_set_rate(RTC_BASE_FREQ);                        /* set the frequency to 2 Hz */
    #endif
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
    outb(RTC_REG_C, RTC_PORT);              /* select register C */
    inb(RTC_DATA);                          /* just throw away contents */

    #if RTC_VT_EN
    rtc_counter++;                          /* increment the counter */
    // printf("RTC: %d", rtc_counter);
    if (rtc_counter == rtc_max_count) {     /* if the counter reaches the max count */
        rtc_interrupt_occurred = 1;         /* set the status to closed */
        // printf("INTERRUPT\n");
        rtc_counter = 0;                    /* reset the counter */
    }
    #else
    rtc_interrupt_occurred = 1;             /* set the status to closed */
    #endif

    send_eoi(RTC_IRQ);                      /* send EOI */
    sti();                                  /* enable interrupts */
}
