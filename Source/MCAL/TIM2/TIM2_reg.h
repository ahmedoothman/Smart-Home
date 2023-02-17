/*
 * TIM0_reg.h
 *
 *  Created on: 3 Oct 2022
 *      Author: 20109
 */

#ifndef MCAL_TIM2_TIM2_REG_H_
#define MCAL_TIM2_TIM2_REG_H_

/**************   Timer2 Registers   **************/

/*Register TCCR2*/
#define TCCR2	 *((volatile u8*) 0x45)
/*Pins*/
#define CS20	0
#define CS21	1
#define CS22	2
#define WGM21	3
#define COM20	4
#define COM21	5
#define WGM20	6
#define FOC2	7


/*Register TCNT2*/
#define TCNT2	 *((volatile u8*) 0x44)


/*Register OCR2*/
#define OCR2  	 *((volatile u8*) 0x43)


/**************   Timer Interrupt states Registers   **************/

/*Register TIMSK*/
#define TIMSK	*((volatile u8*) 0x59)

/*Bits: 0-5 are used by other peripherals*/
#define TOIE2  6
#define OCIE2  7

/*Register TIFR*/
#define TIFR  *((volatile u8*) 0x58)

/*Bits: 0-5 are used by other peripherals*/
#define TOV2  6
#define OCF2  7

#endif /* MCAL_TIM2_TIM2_REG_H_ */
