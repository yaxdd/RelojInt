#ifndef CLOCK_HANDLER_H
#define CLOCK_HANDLER_H
extern char textLCD[10];
extern volatile uint16_t segundos;
extern volatile uint16_t minutos;
extern volatile uint16_t horas;
extern void incrementarHora(void);
extern void incrementarMinuto(void);
extern void incrementarSegundo(void);
extern void programarReloj(uint16_t,uint16_t);
#endif
