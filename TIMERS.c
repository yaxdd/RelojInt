#include <TM4C129.h>
#include "TIMERS.h"
static void (*PeriodicTask) (void) ;
void TIMER0A_Handler(){
	TIMER0->ICR|=0x1;
	(*PeriodicTask)();
}

void Init_Timer0(void(*task)(void),uint32_t period){
	SYSCTL->RCGCTIMER |= 1ul;
	while((SYSCTL->PRTIMER & 1ul ) == 0){}
	PeriodicTask = task;
	TIMER0->CTL&= 0xFFFE;  //Desactivamos el timer 0 si es que estaba activado
	TIMER0->CFG = 0x00000000;
	TIMER0->TAMR = 0x2; //62.5us*2^32 es el tiempo maximo que podemos obtener
	TIMER0->TAILR = (period*16*1000)-1; //(period *16) = 1us => (period*16*1000) => 1ms
	TIMER0->ICR|=0x1;
	NVIC_SetPriority(TIMER0A_IRQn, 0);
	NVIC_EnableIRQ(TIMER0A_IRQn);
	TIMER0->IMR |= 1ul;	
}

void Start_Timer0(){
	TIMER0->CTL|= 0x1;  //Desactivamos el timer 0 si es que estaba activado
}
void Stop_Timer0(){
	TIMER0->CTL&= 0xFFFE;  //Desactivamos el timer 0 si es que estaba activado
}
