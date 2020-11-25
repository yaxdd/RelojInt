#include <TM4C129.h>
#include <stdio.h>
#include "SYSTICK.h"
#include <inttypes.h>
#include "LCD.h"
volatile uint32_t msTicks;
volatile uint32_t curTicks;
static const uint32_t MAX_TICKS = 250;
volatile int exceeded_dead_time = 0;
void SysTick_Handler(void) {
  msTicks++;
}
void delay (uint32_t time) {
  uint32_t curTicksLocal;
  curTicksLocal = msTicks;
  while ((msTicks - curTicksLocal) < time) { __NOP(); }	
}
uint32_t initCycle(void){
	curTicks = msTicks;
	return curTicks;
 	//printf("Inicia ciclo\n");
}
void waitEndCycle (void) {
  while ((msTicks - curTicks) < MAX_TICKS) {__NOP();  }
}

uint64_t singleTest(int log){
	uint32_t array[11];
	uint64_t promedio = 0;
	int cycle = 0;
	int i;
	while(cycle<=11){
		array[cycle]=initCycle();
		if (log==1){
			printf("Ciclo %d -- Tiempo actual %d\n",cycle,array[cycle]);
		}
		delay(200);
		waitEndCycle();
		cycle++;
	}
	for(i=11;i>1;i--){
		promedio+=array[i]-array[i-1];
		if (log==1){
			printf("Diferencia entre tiempos:%" PRIu32 "\n"  ,array[i]-array[i-1]);
		}
	}
	promedio/=10;
	printf("Promedio:%" PRIu64 "\n",promedio);
	return promedio;
}
void MultipleTest(int iteraciones){
	uint64_t prom = 0;
	uint64_t temp;
	int i = 0;
	for (i=0;i<iteraciones;i++){
		temp =singleTest(0);
		prom+=temp;
	}
	printf("promedio de promedios= %" PRIu64 "\n",prom/iteraciones);
}
void initSysTick1ms(){
	SystemCoreClockUpdate();                      
	SysTick_Config((SystemCoreClock / 1000ul));    
}
void disableSysTick(){
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
void enableSysTick(){
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}