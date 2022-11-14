/* idt.c - Functions to initialize and interact with the IDT
 * vim:ts=4 noexpandtab
 */

#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "syscall.h"
#include "paging.h"


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
    exceptions[0] = de_handler_wrapper;
    exceptions[1] = db_handler_wrapper;
    exceptions[2] = nmi_handler_wrapper;
    exceptions[3] = bp_handler_wrapper;
    exceptions[4] = of_handler_wrapper;
    exceptions[5] = br_handler_wrapper;
    exceptions[6] = ud_handler_wrapper;
    exceptions[7] = nm_handler_wrapper;
    exceptions[8] = df_handler_wrapper;
    exceptions[9] = cso_handler_wrapper;
    exceptions[10] = ts_handler_wrapper;
    exceptions[11] = np_handler_wrapper;
    exceptions[12] = ss_handler_wrapper;
    exceptions[13] = gp_handler_wrapper;
    exceptions[14] = pf_handler_wrapper;
    exceptions[15] = r_handler_wrapper;
    exceptions[16] = mf_handler_wrapper;
    exceptions[17] = ac_handler_wrapper;
    exceptions[18] = mc_handler_wrapper;
    exceptions[19] = xf_handler_wrapper;
    exceptions[33] = keyboard_handler_wrapper;          /* Initialize keyboard handler */
    exceptions[40] = rtc_handler_wrapper;               /* Initialize RTC handler */
    exceptions[128] = syscall_wrapper;                  /* Initialize system call handler */

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


void GP(int32_t arg, void* addr)
{
    // printf("General Protection");
    printf("GP Fault exception code is %d attempting to access %x\n", arg, (int)addr);
    while(1){}
    return;
}


void PF(int32_t arg, void* addr)
{
    printf("Page Fault exception code is %d attempting to access %x\n", arg, (int)addr);
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
