#ifndef TIMERS_H
#define TIMERS_H
extern void TIMER0A_Handler(void);
extern void Init_Timer0(void(*)(void),uint32_t);
extern void Start_Timer0(void);
extern void Stop_Timer0(void);
#endif
