#include <TM4C129.h>
#include <stdio.h>
#include "CLOCK_HANDLER.h"
volatile uint16_t segundos;
volatile uint16_t minutos;
volatile uint16_t horas=12;
char textLCD[10];
void incrementarHora(){
	horas++;
	if(horas==24){
		horas = 0;
	}
}
void incrementarMinuto(){
	minutos++;
	if (minutos == 60){
		minutos=0;
		incrementarHora();
	}
}
void incrementarSegundo(){
	segundos++;
	if (segundos == 60){
		segundos=0;
		incrementarMinuto();
	}
}
void programarReloj(uint16_t selector,uint16_t flag){
	switch(selector){
		case 0:
			if (flag){
				horas++;
				if (horas==24){
					horas=0;
				}
			}else{
				if (horas==0){
					horas=23;
				}else{
					horas--;
				}	
			}
			break;
		case 1:
			if (flag){
				minutos++;
				if (minutos==60){
					minutos=0;
				}
			}else{
				if (minutos==0){
					minutos=59;
				}else{
					minutos--;
				}	
			}
			break;
		case 2:
			if (flag){
				segundos++;
				if (segundos==60){
					segundos=0;
				}
			}else{
				if (segundos==0){
					segundos=59;
				}else{
					segundos--;
				}	
			}
			break;
	}
}

