#include <stdio.h>
#include "TM4C129.h"                    // Device header
#include "Serial.h"
#include "LED.h"
#include "BTN.h"
#include "SYSTICK.h"
#include "LCD.h"
#include "TIMERS.h"
#include <inttypes.h>
static volatile uint32_t ticks;
static uint32_t debounceTicks;
static uint32_t progTicks;
static char timeTemp[2];
static char textLCD[10];
static const uint32_t debouncePeriod = 5;
static int enabledProgMode = 0;
static int selector = 0;
static int tempSelector = 0;


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
void debounceClock(void);
void updateClockProgMode(void);
void updateClock(void);
void handleButtons(uint32_t);
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
	if (ticks == 86400){
		ticks = 0;
	}
}
void debounceClock(){
	uint32_t btns= BTN_Get();
	if (btns>0){
		debounceTicks++; 
	}else{
		Stop_Timer1();
		debounceTicks = 0;
		BTN_EnableInt();
	}
	if (debounceTicks % debouncePeriod  == 0){
			if (btns>0){
				LED_Toggle(2); // la accion se esta ejecutando continuamente
				if (btns == 1){
					if (enabledProgMode == 0){
						Stop_Timer0();
						Start_Timer2();
						LED_On(3);
						enabledProgMode = 1;
					}else{
						Start_Timer0();
						Stop_Timer2();
						LED_Off(3);
						enabledProgMode = 0;
					}
				}  else if (enabledProgMode == 1 && ( btns > 1 )){
					uint32_t tempTicks = ticks;
					uint32_t horas = tempTicks / 3600;
					tempTicks = tempTicks % 3600;
					uint32_t minutos = tempTicks / 60;
					uint32_t segundos = tempTicks % 60;
					if (btns == 2){
					selector++;
				} else 
					if (btns == 3 ){	
						if (selector%3 == 0){
							if (segundos == 0){
								segundos = 59;
							}else{
								segundos--;
							}
						} else if (selector%3 == 1){
							if (minutos == 0){
								minutos = 59;
							}else{
								minutos--;
							}
						} else {
							if (horas == 0){
								horas = 23;
							}else{
								horas--;
							}
						}
						
					} else {
						/**/
						if (selector%3 == 0){
							if (segundos == 59){
								segundos = 0;
							}else{
								segundos++;
							}
						} else if (selector%3 == 1){
							if (minutos == 59){
								minutos = 0;
							}else{
								minutos++;
							}
						} else {
							if (horas == 23){
								horas = 0;
							}else{
								horas++;
							}
						}
					}
					ticks = horas * 3600 + minutos * 60 + segundos * 1;
				}
				
			}else{
				LED_Off(2);
				Stop_Timer1();
				debounceTicks = 0;
				BTN_EnableInt();
			}
			
	}
	
}
void updateClockProgMode(){
	progTicks++; 
	if (progTicks % 2 == 0){
		tempSelector = selector;
	}
}
void handleButtons(){
	debounceTicks = 1;
	Start_Timer1();
	BTN_DisableInt();	
}
int main (void) {	
	uint32_t tempTicks = 0;
	uint32_t segundos = 0;
	uint32_t minutos = 0;
	uint32_t horas = 0;
	LED_Initialize();
	//SER_Initialize();
	BTN_Initialize();
	BTN_SetupInt(handleButtons);
	Init_Timer0(updateClock,1000);
	Init_Timer1(debounceClock,100);
	Init_Timer2(updateClockProgMode,500);
	initSysTick1ms();
	LCD_Init();
	LCD_Clear();
	LCD_OutString("00:00:00");
	LCD_ReturnHome();
	Start_Timer0();
	for(;;){
		tempTicks = ticks;
		horas = tempTicks / 3600;
		tempTicks = tempTicks % 3600;
		minutos = tempTicks / 60;
		segundos = tempTicks % 60;
		if ((enabledProgMode == 1) && (progTicks % 2 == 0)){
			if (tempSelector  % 3 == 0){
				textLCD[6] = ' ';
				textLCD[7] = ' ';
			} else if (tempSelector  % 3 == 1){
				textLCD[3] = ' ';
				textLCD[4] = ' ';
			} if (tempSelector  % 3 == 2){
				textLCD[0] = ' ';
				textLCD[1] = ' ';
			}
		}else{
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
		}
		LCD_OutString(textLCD);
		LCD_ReturnHome();
	}

}
