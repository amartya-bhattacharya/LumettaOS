/* idt.h - Defines used in interactions with the IDT
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

/* Externally-visible functions */

/* Initialize the IDT */
extern void idt_init();
/* Exception handlers */
void Default_except();
void DE();
void DB();  
void NMI();
void BP();
void OF();
void BR();
void UD();
void NM();
void DF();
void CSO();
void TS();
void NP();
void SS();
void GP();
void PF();
void R();
void MF();
void AC();     
void MC();
void XF();

#endif /* _IDT_H */
