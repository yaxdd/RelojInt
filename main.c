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
  Prototipos
 *----------------------------------------------------------------------------*/
void debounceClock(void);
void updateClockProgMode(void);
void updateClock(void);
void handleButtons(uint32_t);
void intToTimeString(uint32_t, char*);
/**
* Funcion para convertir enteros a un char array de 2 digitos
*/
void intToTimeString(uint32_t n, char *pt){
	if (n<10){
		pt[0]='0';
		pt[1]=n+'0';
	}else{
		pt[0]=n/10+'0';
		pt[1] = n%10+'0';
	}
}
/**
* Callback para el reloj principal
* Incrementa ticks cada segundo
*/
void updateClock(){
	LED_Toggle(1);
	ticks++; 
	if (ticks == 86400){
		ticks = 0;
	}
}
/**
* Callback para el debounce de los botons
* controlamos los eventos de los botones despues de un determinado tiempo
*/
void debounceClock(){
	uint32_t btns= BTN_Get();
	// Si esta presionado el boton en el tick actual, acumula 
	if (btns>0){
		debounceTicks++; 
	}else{
		// si se llegó a soltar, deten el timer y reinicia todo
		Stop_Timer1();
		debounceTicks = 0;
		BTN_EnableInt();
	}
	// si el timer ha estado presionado durante intervalor de tiempos regulares
	if (debounceTicks % debouncePeriod  == 0){
		//volvemos a valir que se haya mantenido presionado
			if (btns>0){
				// Activamos un led para indicar que se esta ejecutando una accion
				LED_Toggle(2); // la accion se esta ejecutando continuamente
				if (btns == 1){
					// si se presiona el boton 1, revisamos el modo programador
					if (enabledProgMode == 0){
						// si no estaba en modo programador, desactivo el timer principal y ejecuto el timer 2
						// para indicar que elemento esta seleccionado ( a un tiempo distinto)
						Stop_Timer0();
						Start_Timer2();
						LED_On(3);
						enabledProgMode = 1;
					}else{
						// si estaba activado, lo desactivamos y reiniciamos la operacion
						Start_Timer0();
						Stop_Timer2();
						LED_Off(3);
						enabledProgMode = 0;
					}
					// validamos los otros botones unicamente si el modo programador esta activo
				}  else if (enabledProgMode == 1 && ( btns > 1 )){
					// calculamos el tiempo actual
					uint32_t tempTicks = ticks;
					uint32_t horas = tempTicks / 3600;
					tempTicks = tempTicks % 3600;
					uint32_t minutos = tempTicks / 60;
					uint32_t segundos = tempTicks % 60;
					if (btns == 2){
						// si se presiono el boton 2, cambiamos de selector
					selector++;
				} else 
					if (btns == 3 ){	
						// si se presionó el boton 3, disminumimos segun el selector
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
						// si se presionó el boton 4, incrementamos segun el selector
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
					// recuperamos el tiempo total
					ticks = horas * 3600 + minutos * 60 + segundos * 1;
				}
				
			}else{
				// si se cumplió el tiempo pero no habia nada presionado, desactivamos el timer y reiniciamos la operacion
				LED_Off(2);
				Stop_Timer1();
				debounceTicks = 0;
				BTN_EnableInt();
			}
			
	}
	
}
/**
* Callback para el timer2
* Controlamos que parpadee la pantalla led cada medio segundo
*/
void updateClockProgMode(){
	progTicks++; 
	if (progTicks % 2 == 0){
		tempSelector = selector;
	}
}
/**
* Callback para los botones
* Al presionarse un boton, iniciamos el timer 1 para el debounce
* y desactivamos las interrupciones del boton
*/
void handleButtons(){
	Start_Timer1();
	BTN_DisableInt();	
}
int main (void) {	
	//********************
	// Variables locales
	uint32_t tempTicks = 0;
	uint32_t segundos = 0;
	uint32_t minutos = 0;
	uint32_t horas = 0;
	//********************
	LED_Initialize(); // Inicializamos los leds de la tarjeta
	BTN_Initialize(); // inicializamos los botones
	//********************
	// Interrupciones
	BTN_SetupInt(handleButtons); // activamos las interupciones de los botones
	Init_Timer0(updateClock,1000); // activamos las interupciones del timer 0, 1 y 2
	Init_Timer1(debounceClock,100);
	Init_Timer2(updateClockProgMode,500);
	//********************
	//********************
	// Inicializamos la lcd y corremos el timer
	initSysTick1ms();
	LCD_Init();
	LCD_Clear();
	LCD_OutString("00:00:00");
	LCD_ReturnHome();
	Start_Timer0();
	//********************
 // constantemente imprimimos los numeros
	for(;;){
		tempTicks = ticks;
		horas = tempTicks / 3600;
		tempTicks = tempTicks % 3600;
		minutos = tempTicks / 60;
		segundos = tempTicks % 60;
		// si esta activado el modo prog, aplicamos un blink a los seleccionado
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
