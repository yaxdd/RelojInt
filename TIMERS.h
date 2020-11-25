#ifndef TIMERS_H
#define TIMERS_H
extern void TIMER0A_Handler(void);
extern void Init_Timer0(void(*)(void),uint32_t);
extern void Start_Timer0(void);
extern void Stop_Timer0(void);
extern void TIMER1A_Handler(void);
extern void Init_Timer1(void(*)(void),uint32_t);
extern void Start_Timer1(void);
extern void Stop_Timer1(void);
extern void TIMER2A_Handler(void);
extern void Init_Timer2(void(*)(void),uint32_t);
extern void Start_Timer2(void);
extern void Stop_Timer2(void);
#endif
