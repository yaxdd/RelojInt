// Inicializamos los botones de la tiva y 2 externos mas
// Se utilizan los puertos A y J
// pines J0, J1, A4, A5

#include <TM4C129.h>
#include "BTN.h"
static void (*IntTask) (uint32_t) ;
/*----------------------------------------------------------------------------
  initialize Push Button Pins (PA2, PA3 , PA4, PA5)
 *----------------------------------------------------------------------------*/

void BTN_Initialize (void) {


  SYSCTL->RCGCGPIO |=  (1UL << 0);                /* enable clock for GPIOs    */
	GPIOA_AHB->DR2R |=  ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5)); 
  GPIOA_AHB->PDR  |=  ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5)); 
  GPIOA_AHB->DIR  &=  ~((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5)); 
  GPIOA_AHB->DEN  |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5)); 
}


/*----------------------------------------------------------------------------
  Get Push Button status
 *----------------------------------------------------------------------------*/
uint32_t BTN_Get (void) {

	if (((GPIOA_AHB->DATA) & (1ul << 2)) == (1ul << 2)){
		return 4;
	} else if (((GPIOA_AHB->DATA) & (1ul << 3)) == (1ul << 3)){
		return 3;
	} else if (((GPIOA_AHB->DATA) & (1ul << 4)) == (1ul << 4)){
		return 2;
	} else if (((GPIOA_AHB->DATA) & (1ul << 5)) == (1ul << 5)){
		return 1;
	} else {
		return 0;
	}
}

void BTN_SetupInt(void(*task)(uint32_t)){
	GPIOA_AHB->IM &= ~((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	GPIOA_AHB->IS &= ~(0ul);
	GPIOA_AHB->IBE &= ~((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	GPIOA_AHB->IEV  |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	GPIOA_AHB->ICR |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	IntTask = task;
	GPIOA_AHB->IM |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	NVIC_SetPriority(GPIOA_IRQn,7);
	NVIC_EnableIRQ(GPIOA_IRQn);
}
void BTN_DisableInt(){
	NVIC_DisableIRQ(GPIOA_IRQn);
}
void BTN_EnableInt(){
	NVIC_EnableIRQ(GPIOA_IRQn);
}
void GPIOA_Handler(){
	GPIOA_AHB->ICR |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	uint32_t btns= BTN_Get();
	(*IntTask)(btns);
}

