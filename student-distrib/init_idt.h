//void lookup_exception(int exception_num);
//void interrupt_handler (int exception_num);
extern void init_idt();
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
void KB();
void RTC();
