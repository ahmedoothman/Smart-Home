/*********************************************************/
/***********		Author: TaqiEldeen	 	**************/
/***********		Layer: MCAL			 	**************/
/***********		Component: TIMER		**************/
/***********		Version: 1.02		 	**************/
/***********		Date: 13 Sep 2022	 	**************/
/***********		Updated: 17 Sep 2022	**************/
/*********************************************************/

#ifndef MCAL_TIM0_TIM0_INT_H_
#define MCAL_TIM0_TIM0_INT_H_


#define	TIMER_DUTY_CYCLE_25		25
#define	TIMER_DUTY_CYCLE_50		50
#define	TIMER_DUTY_CYCLE_75		75
#define	TIMER_DUTY_CYCLE_100	100

/**********************************************************************************************************
 * Description : Interface Function to 1-prescaler select, 2-Timer0 mode select, 3-Set OC0 Pin state
 * Outputs     : void
 * Inputs      : void
 * Notes	   : timer is initially off
 ***********************************************************************************************************/
void TIM0_vInit ();

/**********************************************************************************************************
 * Description : Interface Function to Turn on a timer
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM0_vTurnOn ();

/**********************************************************************************************************
 * Description : Interface Function to set a delay for a specific timer
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM0_vTurnOff	();

/**********************************************************************************************************
 * Description : Interface Function to set preload to start counting from this value
 * Outputs     : void
 * Inputs      : Preload value
 ***********************************************************************************************************/
void TIM0_vSetPreload (u8 A_u8PreloadVal);

/**********************************************************************************************************
 * Description : Interface Function to set OCR0 register
 * Outputs     : void
 * Inputs      : OCR value
 ***********************************************************************************************************/
void TIM0_vSetOcr0Val (u8 A_u8OcrVal);

/**********************************************************************************************************
 * Description : Interface Function to get OCR0 register
 * Outputs     : OCR0
 * Inputs      : void
 ***********************************************************************************************************/
u8 TIM0_vGetOcr0Val ();

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during OCF0
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM0_vCallBack_OCF0 (ptr_func_t ptr);

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during overflow
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM0_vCallBack_OVF	(ptr_func_t ptr);

/**********************************************************************************************************
 * Description : Interface Function to set a delay for a specific timer
 * Outputs     : void
 * Inputs      : Timer ID, required delay in milliseconds, the function to be executed
 * NOTES       : still need some work!
 ***********************************************************************************************************/
void  TIM0_vDelayMilli (u16 A_u16DelayMs, u8 A_u8TimerId, ptr_func_t ptr);

/**********************************************************************************************************
 * Description : interface Function to setup the OC0 pin duty cycle if in PWM mode
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM0_vSetDutyCycleOC0 (u8 A_u8DutyCycle);

#endif /* MCAL_TIM0_TIM0_INT_H_ */
