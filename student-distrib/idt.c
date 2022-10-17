/* idt.c - Functions to initialize and interact with the IDT
 * vim:ts=4 noexpandtab
 */

#include "idt.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"


/* Initialize the IDT
 * Set up the IDT with the default exception handlers and the system call handler
 */
void idt_init(){
    int i, j;

    void (*exceptions[256])(void);          /* Array of pointers to exception handlers */
    for(i = 0; i < 256; i++){
        exceptions[i] = Default_except;     /* Initialize all exception handlers to default */
    }

    /* Initialize the exception handlers */
    exceptions[0] = DE;
    exceptions[1] = DB;
    exceptions[2] = NMI;
    exceptions[3] = BP;
    exceptions[4] = OF;
    exceptions[5] = BR;
    exceptions[6] = UD;
    exceptions[7] = NM;
    exceptions[8] = DF;
    exceptions[9] = CSO;
    exceptions[10] = TS;
    exceptions[11] = NP;
    exceptions[12] = SS;
    exceptions[13] = GP;
    exceptions[14] = PF;
    exceptions[15] = R;
    exceptions[16] = MF;
    exceptions[17] = AC;
    exceptions[18] = MC;
    exceptions[19] = XF;
    exceptions[33] = keyboard_handler_wrapper;          /* Initialize keyboard handler */
    exceptions[40] = rtc_handler_wrapper;               /* Initialize RTC handler */

    for(j = 0; j < 256; j++){                           /* Initialize the IDT */
        idt[j].seg_selector = KERNEL_CS;
        idt[j].reserved4 = 0;
        idt[j].reserved3 = 0;
        idt[j].reserved2 = 1;
        idt[j].reserved1 = 1;
        idt[j].size = 1;
        idt[j].reserved0 = 0;
        idt[j].dpl = 0;
        idt[j].present =0;

        if (j==0x80) {                                  /* Initialize system call handler */
            idt[j].dpl = 3;

        }
        if(j==0x80 || ((j<=19) && (j!=15 && (j != 1))) || j==33 || j==40){  /* Set present bit for all relevant handlers */
            idt[j].present = 1;
        }
        SET_IDT_ENTRY(idt[j], exceptions[j]);           /* Set the IDT entry */
    }
}

/* Exception handlers */
void Default_except()
{
    printf("invalid");
    while(1){}
    return;
}


void DE()
{
    printf("Divide error");
    while(1){}
    return;
}


void DB()
{
    printf("RESERVED");
    while(1){}
    return;
}

  
void NMI()
{
    printf("NMI interrupt");
    while(1){}
    return;
}


void BP()
{
    printf("Breakpoint");
    while(1){}
    return;
}


void OF()
{
    printf("Overflow");
    while(1){}
    return;
}


void BR()
{
    printf("BOUND Range Exceeded");
    while(1){}
    return;
}


void UD()
{
    printf("Invalid Opcode");
    while(1){}
    return;
}


void NM()
{
    printf("Device not available");
    while(1){}
    return;
}


void DF()
{
    printf("Double fault");
    while(1){}
    return;
}


void CSO()
{
    printf("Coprocessor Segment Overrun");
    while(1){}
    return;
}


void TS()
{
    printf("Invalid TSS");
    while(1){}
    return;
}


void NP()
{
    printf("Segment not present");
    while(1){}
    return;
}


void SS()
{
    printf("Stack segment fault");
    while(1){}
    return;
}


void GP()
{
    printf("General Protection");
    while(1){}
    return;
}


void PF()
{
    printf("Page Fault");
    while(1){}
    return;
}


void R()
{
    printf("Intel RESERVED");
    while(1){}
    return;
}


void MF()
{
    printf("x87 FPU Floating Point Error");
    while(1){}
    return;
}


void AC()
{
    printf("Alignment Check");
    while(1){}
    return;
}


void MC()
{
    printf("Machine Check");
    while(1){}
    return;
}


void XF()
{
    printf("SIMD Floating Point Exception");
    while(1){}
    return;
}
