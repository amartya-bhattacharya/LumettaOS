/* idt.h - Defines used in interactions with the IDT
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

/* Externally-visible functions */

/* Initialize the IDT */
extern void idt_init();
extern void de_handler_wrapper();
extern void db_handler_wrapper();
extern void nmi_handler_wrapper();
extern void bp_handler_wrapper();
extern void of_handler_wrapper();
extern void br_handler_wrapper();
extern void ud_handler_wrapper();
extern void nm_handler_wrapper();
extern void df_handler_wrapper();
extern void cso_handler_wrapper();
extern void ts_handler_wrapper();
extern void np_handler_wrapper();
extern void ss_handler_wrapper();
extern void gp_handler_wrapper();
extern void pf_handler_wrapper();
extern void r_handler_wrapper();
extern void mf_handler_wrapper();
extern void ac_handler_wrapper();
extern void mc_handler_wrapper();
extern void xf_handler_wrapper();





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
