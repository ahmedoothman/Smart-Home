/*
 * TIM0_prg.c
 *
 *  Created on: 3 Oct 2022
 *      Author: 20109
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/DIO/DIO_int.h"
#include "TIM1_pri.h"
#include "TIM1_reg.h"
#include "TIM1_cfg.h"
#include "TIM1_int.h"
#include <math.h>

static u16 G_u16ISRCounter = 0;


/**********************************************************************************************************
 * Description : Interface Function to 1-prescaler select, 2-Timer1 mode select, 3-Set OC0 Pin state
 * Outputs     : void
 * Inputs      : void
 * Notes	   : timer is initially off
 ***********************************************************************************************************/
void TIM1_vInit(){
	#if TIMER1_STATE == TIMER_ENABLE

		/*Setup the mode*/
		TIMER_vTimer1_setupWGM();

		CLR_BIT(TCCR1A, FOC1B);
		CLR_BIT(TCCR1A, FOC1A);
	#else

	#endif
}

/**********************************************************************************************************
 * Description : Interface Function to Turn on a timer
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM1_vTurnOn (){
	TCCR1B &= TIMER1_CLK_SELECT_MASK;
	TCCR1B |= TIMER1_CLK_SELECT;
}

/**********************************************************************************************************
 * Description : Interface Function to set a delay for a specific timer
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM1_vTurnOff	(){
	TCCR1B &= TIMER1_CLK_SELECT_MASK;
	TCCR1B |= TIMER1_PRESCALER_NO_CLK;
	DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, VAL_LOW);
}

void TIM1_vOcr1bOn(){
	TCCR1A |= (1<<COM1B1);
	TCCR1A &= ~(1<<COM1B0);
}

void TIM1_vOcr1bOff(){
	TCCR1A &= ~(1<<COM1B1);
	TCCR1A &= ~(1<<COM1B0);
}

/**********************************************************************************************************
 * Description : Interface Function to set preload to start counting from this value
 * Outputs     : void
 * Inputs      : Preload value
 ***********************************************************************************************************/
void TIM1_vSetPreload (u16 A_u16PreloadVal){
	TCNT1 = A_u16PreloadVal;
	G_u16Timer1_Preload_Val = A_u16PreloadVal;
}

/**********************************************************************************************************
 * Description : Interface Function to set OCR1A register
 * Outputs     : void
 * Inputs      : OCR value
 ***********************************************************************************************************/
void TIM1_vSetOcr1aVal (u16 A_u16OcrVal){
	OCR1A = A_u16OcrVal;
}

/**********************************************************************************************************
 * Description : Interface Function to set OCR1B register
 * Outputs     : void
 * Inputs      : OCR value
 ***********************************************************************************************************/
void TIM1_vSetOcr1bVal (u16 A_u16OcrVal){
	OCR1B = A_u16OcrVal;
}

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during overflow
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM1_vCallBack_OVF	(ptr_func_t ptr){
	G_PTRF_TIM1_OVF = ptr;
}

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during OCF1A
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM1_vCallBack_OCF1A (ptr_func_t ptr){
	G_PTRF_TIM1_OC1AF = ptr;
}

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during OCF1B
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM1_vCallBack_OCF1B (ptr_func_t ptr){
	G_PTRF_TIM1_OC1BF = ptr;
}

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during ICR event
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIMER_vCallBack_ICF1 (ptr_func_t ptr){
	G_PTRF_TIM1_ICF1 = ptr;
}

/**********************************************************************************************************
 * Description : interface Function to setup the OC1a pin duty cycle if in PWM mode
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM1_vSetDutyCycleOC1A (u8 A_u8DutyCycle){
	#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
		TIM1_vSetOcr1aVal( ( (0xFF + 1) * A_u8DutyCycle ) /100);
	#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
		TIM1_vSetOcr1aVal( ( -( (A_u8DutyCycle/100) - 1) ) * (0xFF + 1) );
	#endif
}

/**********************************************************************************************************
 * Description : interface Function to setup the OC1b pin duty cycle if in PWM mode
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM1_vSetDutyCycleOC1B (u8 A_u8DutyCycle){
	#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
		TIM1_vSetOcr1bVal( (u16)( ( (f32)( (TIMER1_MAX_COUNT) * A_u8DutyCycle ) ) /100.0) ) ;
	#elif TIMER1_OC1B_PWM_MODE == PWM_INVERTING
		TIM1_vSetOcr1bVal( ((A_u8DutyCycle*256)/100.0 - 1) );
	#endif
}

/**********************************************************************************************************
 * Description : Interface Function to set ICR1 register
 * Outputs     : void
 * Inputs      : ICR value
 ***********************************************************************************************************/
void TIM1_vSetIcr1Val (u16 A_u16IcrVal){
	ICR1 = A_u16IcrVal;
}

/**********************************************************************************************************
 * Description : Interface Function to set TIMER1 in ICU mode
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM1_vInitICU (){
	/*WGM of normal mode*/
	TCCR1A &= ~( (1<<WGM10) | (1<<WGM11) );
	TCCR1B &= ~( (1<<WGM12) | (1<<WGM13) );

	/*Enable Capture interrupt and overflow*/
	TIMSK |= (1<< TICIE1) | (1<<TOIE1);

	/*IC pin as input*/
	DIO_vSetPinDir(PORTD_ID, PIN6_ID, DIR_INPUT);

	/*Edge select of the ICU: Rising edge*/
	SET_BIT(TCCR1B, ICES1);

	/*PreScaling of 8*/
	TCCR1B |= (1<<CS11) ;
	TCCR1B &= ~( (1<<CS10) | (1<<CS12) );
}

/**********************************************************************************************************
 * Description : Interface Function to turn off interrupt of iCR
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM1_vDisableICU (){
	CLR_BIT(TIMSK, TICIE1);
}

/**********************************************************************************************************
 * Description : Interface Function to set the Triggering edge for the ICU
 * Outputs     : void
 * Inputs      : Trigger mode
 ***********************************************************************************************************/
void TIM1_vSetTriggering (u8 A_u8Trigger){
	switch(A_u8Trigger) {
		case ICR_FALLING_EDGE: TCCR1B &= ~(1<<ICES1); break;
		case ICR_RISING_EDGE: TCCR1B |= (1<<ICES1); break;
	}
}

/**********************************************************************************************************
 * Description : Interface Function to set ICR1 register
 * Outputs     : ICR
 * Inputs      : void
 ***********************************************************************************************************/
u16 TIM1_vGetIcr1Val (){
	return ICR1;
}


/**********************************************************************************************************
 * Description : private Function to setup the Timer1 based on configuration
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
static void TIMER_vTimer1_setupWGM() {

	#if TIMER1_WGM_MODE == TIMER1_WGM_NORMAL_MODE
		TCCR1A &= ~( (1<<WGM10) | (1<<WGM11) );
		TCCR1B &= ~( (1<<WGM12) | (1<<WGM13) );

		/*Setup the OC1x pin mode*/
		TCCR1A &= TIMER1_COM1xx_MASK;
		TCCR1A |= (TIMER1_OC1A_MODE << COM1A0);
		TCCR1A |= (TIMER1_OC1B_MODE << COM1B0);

		#if TIMER1_OVF_INT_STATE == INT_ENABLE
			TIMSK |= (1<<TOIE1);
		#endif

		/*Set the pin direction*/
		#if TIMER1_OC1A_MODE != TIMER1_OC1x_DISCONNECT
			DIO_vSetPinDir(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#endif

		#if TIMER1_OC1B_MODE != TIMER1_OC1x_DISCONNECT
			DIO_vSetPinDir(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_PWM_PHASE_CORRECT_8BIT_MODE
		TCCR1A |= (1<<WGM10);
		TCCR1A &= ~(1<<WGM11);
		TCCR1B &= ~( (1<<WGM12) | (1<<WGM13) );

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_PWM_PHASE_CORRECT_9BIT_MODE
		TCCR1A |= (1<<WGM11);
		TCCR1A &= ~(1<<WGM10);
		TCCR1B &= ~( (1<<WGM12) | (1<<WGM13) );

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_PWM_PHASE_CORRECT_10BIT_MODE
		TCCR1A |= (1<<WGM11) | (1<<WGM10);
		TCCR1B &= ~( (1<<WGM12) | (1<<WGM13) );

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_CTC_MODE
		TCCR1A &= ~( (1<<WGM10) | (1<<WGM11) );
		TCCR1B |= (1<<WGM12);
		TCCR1B &= ~(1<<WGM13);

		/*Setup the OC1x pin mode*/
		TCCR1A &= TIMER1_COM1xx_MASK;
		TCCR1A |= (TIMER1_OC1A_MODE << COM1A0);
		TCCR1A |= (TIMER1_OC1B_MODE << COM1B0);

		/*Set the pin direction*/
		#if TIMER1_OC1A_MODE != TIMER1_OC1x_DISCONNECT
			DIO_vSetPinDir(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#endif

		#if TIMER1_OC1B_MODE != TIMER1_OC1x_DISCONNECT
			DIO_vSetPinDir(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_FAST_PWM_8BIT_MODE
		TCCR1A |= (1<<WGM10);
		TCCR1A &= ~(1<<WGM11);
		TCCR1B |= (1<<WGM12);
		TCCR1B &= ~(1<<WGM13);

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_FAST_PWM_9BIT_MODE
		TCCR1A &= ~(1<<WGM10);
		TCCR1A |= (1<<WGM11);
		TCCR1B |= (1<<WGM12);
		TCCR1B &= ~(1<<WGM13);

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_FAST_PWM_10BIT_MODE
		TCCR1A |= (1<<WGM10) | (1<<WGM11);
		TCCR1B |= (1<<WGM12);
		TCCR1B &= ~(1<<WGM13);

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif
	#elif TIMER1_WGM_MODE == TIMER1_WGM_PWM_PHASE_CORRECT_TOP_ICR1
		TCCR1A &= ~( (1<<WGM10) | (1<<WGM11) );
		TCCR1B &= ~(1<<WGM12);
		TCCR1B |= (1<<WGM13);

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_PWM_PHASE_CORRECT_TOP_OCR1A
		TCCR1A |= (1<<WGM10);
		TCCR1A &= ~(1<<WGM11);
		TCCR1B &= ~(1<<WGM12);
		TCCR1B |= (1<<WGM13);

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif
	#elif TIMER1_WGM_MODE == TIMER1_WGM_CTC_TOP_ICR1
		TCCR1A &= ~( (1<<WGM10) | (1<<WGM11) );
		TCCR1B |= ( (1<<WGM12) | (1<<WGM13) );

		/*Setup the OC1x pin mode*/
		TCCR1A &= TIMER1_COM1xx_MASK;
		TCCR1A |= (TIMER1_OC1A_MODE << COM1A0);
		TCCR1A |= (TIMER1_OC1B_MODE << COM1B0);

		/*Set the pin direction*/
		#if TIMER1_OC1A_MODE != TIMER1_OC1x_DISCONNECT
			DIO_vSetPinDir(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#endif

		#if TIMER1_OC1B_MODE != TIMER1_OC1x_DISCONNECT
			DIO_vSetPinDir(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_FAST_PWM_TOP_ICR1
		TCCR1A &= ~(1<<WGM10);
		TCCR1A |= (1<<WGM11);
		TCCR1B |= ( (1<<WGM12) | (1<<WGM13) );

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinDir(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A1) | (1<<COM1A0) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinDir(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1B_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1B_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif

	#elif TIMER1_WGM_MODE == TIMER1_WGM_FAST_PWM_TOP_OCR1A
		TCCR1A |= (1<<WGM10) | (1<<WGM11);
		TCCR1B |= (1<<WGM12) | (1<<WGM13);

		/*Setup the PWM state of OC1A*/
		#if TIMER1_OC1A_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1A1);
			TCCR1A &= ~(1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
			DIO_vSetPinVal(OC1A_PORT, OC1A_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1A0) | (1<<COM1A1) );
		#endif

		/*Setup the PWM state of OC1B*/
		#if TIMER1_OC1B_PWM_MODE == PWM_NON_INVERTING
			TCCR1A |= (1<<COM1B1);
			TCCR1A &= ~(1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_INVERTING
			TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
			DIO_vSetPinVal(OC1B_PORT, OC1B_PIN, DIR_OUTPUT);
		#elif TIMER1_OC1A_PWM_MODE == PWM_OFF
			TCCR1A &= ~( (1<<COM1B0) | (1<<COM1B1) );
		#endif
	#endif
}

/**
 * Description : Interface Function to set a delay for a specific timer
 * Outputs     : void
 * Inputs      :required delay in milliseconds, the function to be executed
 * NOTES       : still need some work!
 **/
void  TIM1_vDelayMilli (u16 A_u16DelayMs, ptr_func_t ptr){
    u16 L_u16PrescaleVal = 1;
    f32 L_f32TimeOverFlow;
    f32 L_f32TickTime;
    u16 L_u16OverFlowCounts = 0;
    f32 L_f32PreloadVal = 0;
	/* Resetting the ISR Counter */
	G_u16ISRCounter = 1;

    switch(TIMER1_CLK_SELECT){
        case TIMER1_PRESCALER_8:    L_u16PrescaleVal = 8;     break;
        case TIMER1_PRESCALER_64:   L_u16PrescaleVal = 64;    break;
        case TIMER1_PRESCALER_256:  L_u16PrescaleVal = 256;	  break;
        case TIMER1_PRESCALER_1024: L_u16PrescaleVal = 1024;  break;
    }

    /* Tick time Calculation */
    L_f32TickTime = (f32)L_u16PrescaleVal / TIMER1_InputFreq;

    /* Time required for complete 1 over flow */
    L_f32TimeOverFlow = ((u32)TIMER1_MAX_COUNT + 1.0) * (L_f32TickTime * (1000UL));

    /* Over flow needed by this delay */
    L_u16OverFlowCounts = (u16) ceil(( ((f32)A_u16DelayMs /L_f32TimeOverFlow) ));

    /* Calculating OverFlow counts & preload value */
    L_f32PreloadVal = ( ( (f32)L_u16OverFlowCounts - ((f32)A_u16DelayMs / L_f32TimeOverFlow) )  * TIMER1_MAX_COUNT );
    G_u16Timer1_Cov = (u16)L_u16OverFlowCounts;


#if  TIMER1_WGM_MODE == TIMER1_WGM_CTC_MODE
        //TIM1_vSetOcr1aVal((TIMER0_MAX_COUNT - L_u16PreloadVal));
        //G_PTRF_TIM0_CTC = ptr;
#elif TIMER1_WGM_MODE == TIMER1_WGM_NORMAL_MODE
        G_PTRF_TIM1_OVF = ptr;
		TIM1_vSetPreload( (u16) L_f32PreloadVal);
#endif

}



/**************   TIMER1 ISR   **************/

/*TIMER1 CAPT*/
void __vector_6(void)  {
	if(G_PTRF_TIM1_ICF1 != ADDRESS_NULL) {
		G_PTRF_TIM1_ICF1();
	} else {

	}
}

/*TIMER1 COMPA*/
void __vector_7(void){
	if(G_PTRF_TIM1_OC1AF != ADDRESS_NULL) {
		G_PTRF_TIM1_OC1AF();
	} else {

	}
}

/*TIMER1 COMPB*/
void __vector_8(void){
	if(G_PTRF_TIM1_OC1BF != ADDRESS_NULL) {
		G_PTRF_TIM1_OC1BF();
	} else {

	}
}

/*TIMER1 OVF*/
void __vector_9(void){
	if(G_PTRF_TIM1_OVF != ADDRESS_NULL)
	{
		if(G_u16ISRCounter == G_u16Timer1_Cov)
		{
			G_u16ISRCounter = 1;
			G_PTRF_TIM1_OVF();
			TCNT1 = G_u16Timer1_Preload_Val;
		} else {
			G_u16ISRCounter++;
		}
	} else {
		/* Handle callback error */
	}
	return;
}
