// Inicializamos los botones de la tiva y 2 externos mas
// Se utilizan los puertos A y J
// pines J0, J1, A4, A5

#include <TM4C129.h>
#include <inttypes.h>
#include "Serial.h"
#include <stdio.h>
#include "BTN.h"
static void (*IntTask) (uint32_t) ;
/*----------------------------------------------------------------------------
  initialize Push Button Pins (PJ0, PJ1 , PA4, PA5)
 *----------------------------------------------------------------------------*/

void BTN_Initialize (void) {


  SYSCTL->RCGCGPIO |=  (1UL << 0);                /* enable clock for GPIOs    */
//  GPIOJ_AHB->DR2R |=  ((1ul << 0) | (1ul << 1)); /* PJ0, PJ1 2-mA Drive       */
//  GPIOJ_AHB->PUR  |=  ((1ul << 0) | (1ul << 1)); /* PJ0, PJ1 pull-down          */
//  GPIOJ_AHB->DIR  &= ~((1ul << 0) | (1ul << 1)); /* PJ0, PJ1 is intput        */
//  GPIOJ_AHB->DEN  |=  ((1ul << 0) | (1ul << 1)); /* PJ0, PJ1 is digital func. */
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
	
//	GPIOJ_AHB->IM &= ~((1ul << 0) | (1ul << 1));
//	GPIOJ_AHB->IS &= ~(0ul);
//	GPIOJ_AHB->IBE |= ~((1ul << 0) | (1ul << 1));
//	GPIOJ_AHB->IEV &=  ~((1ul << 0) | (1ul << 1));
//	GPIOJ_AHB->ICR |= ((1ul << 0) | (1ul << 1));
	GPIOA_AHB->IM &= ~((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	GPIOA_AHB->IS &= ~(0ul);
	GPIOA_AHB->IBE &= ~((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	GPIOA_AHB->IEV  |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	GPIOA_AHB->ICR |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	IntTask = task;
	GPIOA_AHB->IM |= ((1ul << 2) |(1ul << 3) |(1ul << 4) | (1ul << 5));
	//GPIOJ_AHB->IM |= ((1ul << 0) | (1ul << 1));
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
//void GPIOJ_Handler(){
//	GPIOJ_AHB->ICR |= ((1ul << 0) | (1ul << 1));
//	uint32_t btns= BTN_Get();
//	(*IntTask)(btns);
//}
