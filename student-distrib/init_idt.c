#include "init_idt.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"

#define RUN_TESTS
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))

//enum exceptions {DE=0, DB=1, NMI=2, BP=3, OF=4, BR=5, UD=6, NM=7, DF=8, CSO=9, TS=10, NP=11, SS=12, GP=13, PF=14, R=15, MF=16, AC=17, MC=18, XF=19, KB=33, RTC=40};

/*generalized exception handler that prints */
// void lookup_exception(int exception_num){
//     enum exceptions excep;
//     if (exception_num <= 19 || exception_num == 33 || exception_num == 40) {
//         excep = exception_num;
//         interrupt_handler(excep);
//     } else {
//         interrupt_handler((char*)41);
//     }
// }

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

void KB()
    {
        printf("Keyboard Interrupt");
        while(1){}
        return;
    }

void RTC()
    {
        printf("RTC Interrupt");
        while(1){}
        return;
    }




void init_idt (){
    
     int j;

//typedef void (*func) (void);
void (*exceptions[256])(void);
for(j =0; j<256; j++){
    exceptions[j]=Default_except;
}
exceptions[0]=DE;
exceptions[1]=DB;
exceptions[2]=NMI;
exceptions[3]=BP;
exceptions[4]=OF;
exceptions[5]=BR;
exceptions[6]=UD;
exceptions[7]=NM;
exceptions[8]=DF;
exceptions[9]=CSO;
exceptions[10]=TS;
exceptions[11]=NP;
exceptions[12]=SS;
exceptions[13]=GP;
exceptions[14]=PF;
exceptions[15]=R;
exceptions[16]=MF;
exceptions[17]=AC;
exceptions[18]=MC;
exceptions[19]=XF;
exceptions[33]=KB;
exceptions[40]=RTC;
    int i;
    for(i =0; i < 256; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present =0;

        if (i==0x80)
        {
            idt[i].dpl = 3;

        }
        if(i==0x80 || i<=19 || i==33 || i==40){
            idt[i].present = 1;
        }
        SET_IDT_ENTRY(idt[i], exceptions[i]);
    }
}
