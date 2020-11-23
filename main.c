#include <stdio.h>
#include "TM4C129.h"                    // Device header
#include "Serial.h"
#include "LED.h"
#include "BTN.h"
#include "SYSTICK.h"
#include "LCD.h"
#include "TIMERS.h"
#include <inttypes.h>
static volatile uint16_t ticks;
static uint16_t tempTicks = 0;
static uint16_t segundos = 0;
static uint16_t minutos = 0;
static uint16_t horas = 0;
static 	char timeTemp[2];
static char textLCD[10];
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
void updateClock(void);
void intToTimeString(uint32_t, char*);
void intToTimeString(uint32_t n, char *pt){
	if (n<10){
		pt[0]='0';
		pt[1]=n+'0';
	}else{
		pt[0]=n/10+'0';
		pt[1] = n%10+'0';
	}
}

void updateClock(){
	LED_Toggle(1);
	ticks++; 
}
int main (void) {	
	LED_Initialize();
	SER_Initialize();
	Init_Timer0(updateClock,1000);
	initSysTick1ms();
	LCD_Init();
	LCD_Clear();
	LCD_OutString("00:00:00");
	returnHome();
	Start_Timer0();
	for(;;){
		tempTicks = ticks;
		horas = tempTicks / 3600;
		tempTicks = tempTicks % 3600;
		minutos = tempTicks / 60;
		segundos = tempTicks % 60;
		intToTimeString(horas,timeTemp);
		textLCD[0]=timeTemp[0];
		textLCD[1]=timeTemp[1];
		textLCD[2]=':';
		intToTimeString(minutos,timeTemp);
		textLCD[3]=timeTemp[0];
		textLCD[4]=timeTemp[1];
		textLCD[5]=':';
		intToTimeString(segundos, timeTemp);
		textLCD[6]=timeTemp[0];
		textLCD[7]=timeTemp[1];
		LCD_OutString(textLCD);
		returnHome();
	}

}
