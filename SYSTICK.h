#ifndef SYSTICK_H
#define SYSTICK_H
extern volatile uint32_t msTicks;
extern volatile uint32_t curTicks;;
extern volatile int exceeded_dead_time;
extern void SysTick_Handler(void);
extern void delay(uint32_t );
extern uint32_t  initCycle(void);
extern void waitEndCycle(void);
extern uint64_t singleTest(int);
extern void MultipleTest(int);
extern void initSysTick1ms(void);
extern void disableSysTick(void);
extern void enableSysTick(void);
#endif
