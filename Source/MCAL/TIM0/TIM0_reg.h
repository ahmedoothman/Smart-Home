/*********************************************************/
/***********		Author: TaqiEldeen	 	**************/
/***********		Layer: MCAL			 	**************/
/***********		Component: TIMER		**************/
/***********		Version: 1.02		 	**************/
/***********		Date: 13 Sep 2022	 	**************/
/***********		Updated: 17 Sep 2022	**************/
/*********************************************************/

#ifndef MCAL_TIM0_TIM0_REG_H_
#define MCAL_TIM0_TIM0_REG_H_

/**************   Timer0 Registers   **************/

/*Register TCCR0*/
#define TCCR0	 *((volatile u8*) 0x53)
/*Pins*/
#define CS00  0 /*Clock select*/
#define CS01  1 /*Clock select*/
#define CS02  2 /*Clock select*/
#define WGM01 3 /*Waveform gen mode*/
#define COM00 4 /*OC0 pin MODES*/
#define COM01 5 /*OC0 pin MODES*/
#define WGM00 6 /*Waveform gen mode*/
#define FOC0  7 /*Force output compare*/

/*register TCNT0 timer counter*/
#define TCNT0	 *((volatile u8*) 0x52)

/*Register OCR0*/
#define OCR0  *((volatile u8*) 0x5C)

/*Register TIMSK*/
#define TIMSK	*((volatile u8*) 0x59)

/*Bits: 2-7 are used by other timers*/
#define TOIE0  0
#define OCIE0  1

/*Register TIFR*/
#define TIFR  *((volatile u8*) 0x58)

/*Bits: 2-7 are used by other timers*/
#define TOV0  0
#define OCF0  1


#endif /* MCAL_TIM0_TIM0_REG_H_ */
