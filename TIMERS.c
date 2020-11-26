#include <TM4C129.h>
#include "TIMERS.h"
static void (*PeriodicTask) (void) ;
static void (*PeriodicTaskB) (void) ;
static void (*PeriodicTaskC) (void) ;
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
	TIMER0->TAILR = (period*16*1000)+1500; //(period *16) = 1us => (period*16*1000) => 1ms
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

void TIMER1A_Handler(){
	TIMER1->ICR|=0x1;
	(*PeriodicTaskB)();
}

void Init_Timer1(void(*task)(void),uint32_t period){
	SYSCTL->RCGCTIMER |= 1ul<<1;
	while((SYSCTL->PRTIMER & 1ul<<1) == 0){}
	PeriodicTaskB = task;
	TIMER1->CTL&= ~(1ul);  //Desactivamos el timer 0 si es que estaba activado
	TIMER1->CFG = 0x00000000;
	TIMER1->TAMR = 0x2; //62.5us*2^32 es el tiempo maximo que podemos obtener
	TIMER1->TAILR = (period*16*1000)+97; //(period *16) = 1us => (period*16*1000) => 1ms
	TIMER1->ICR|=0x1;
	NVIC_SetPriority(TIMER1A_IRQn, 1);
	NVIC_EnableIRQ(TIMER1A_IRQn);
	TIMER1->IMR |= 1ul;	
}

void Start_Timer1(){
	TIMER1->CTL|= 0x1;  //Desactivamos el timer 0 si es que estaba activado
}
void Stop_Timer1(){
	TIMER1->CTL&= 0xFFFE;  //Desactivamos el timer 0 si es que estaba activado
}



void TIMER2A_Handler(){
	TIMER2->ICR|=0x1;
	(*PeriodicTaskC)();
}

void Init_Timer2(void(*task)(void),uint32_t period){
	SYSCTL->RCGCTIMER |= 1ul<<2;
	while((SYSCTL->PRTIMER & 1ul<<2 ) == 0){}
	PeriodicTaskC = task;
	TIMER2->CTL&= ~(1ul);  //Desactivamos el timer 0 si es que estaba activado
	TIMER2->CFG = 0x00000000;
	TIMER2->TAMR = 0x2; //62.5us*2^32 es el tiempo maximo que podemos obtener
	TIMER2->TAILR = (period*16*1000)+97; //(period *16) = 1us => (period*16*1000) => 1ms
	TIMER2->ICR|=0x1;
	NVIC_SetPriority(TIMER2A_IRQn, 2);
	NVIC_EnableIRQ(TIMER2A_IRQn);
	TIMER2->IMR |= 1ul;	
}

void Start_Timer2(){
	TIMER2->CTL|= 0x1;  //Desactivamos el timer 0 si es que estaba activado
}
void Stop_Timer2(){
	TIMER2->CTL&= 0xFFFE;  //Desactivamos el timer 0 si es que estaba activado
}
