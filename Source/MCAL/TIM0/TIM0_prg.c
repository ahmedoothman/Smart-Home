/*********************************************************/
/***********		Author: TaqiEldeen	 	**************/
/***********		Layer: MCAL			 	**************/
/***********		Component: TIMER		**************/
/***********		Version: 1.02		 	**************/
/***********		Date: 13 Sep 2022	 	**************/
/***********		Updated: 17 Sep 2022	**************/
/*********************************************************/

#include <math.h>
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../DIO/DIO_int.h"
#include "TIM0_reg.h"
#include "TIM0_pri.h"
#include "TIM0_cfg.h"
#include "TIM0_int.h"


static u16 G_u16ISRCounter = 0;


/**********************************************************************************************************
 * Description : Interface Function to 1-prescaler select, 2-Timer0 mode select, 3-Set OC0 Pin state
 * Outputs     : void
 * Inputs      : void
 * Notes	   : timer is initially off
 ***********************************************************************************************************/
void TIM0_vInit (){
#if TIMER0_STATE == TIMER_ENABLE
	/*Step 1: Choose timer mode*/
	#if TIMER0_WGM_MODE == TIMER0_WGM_NORMAL_MODE
		TCCR0 &= TIMER0_WGM_MASK;
		TCCR0 |= TIMER0_WGM_MODE;

		/*Set the interrupt*/
		#if TIMER0_OVF_INT_STATE == INT_ENABLE
			SET_BIT(TIMSK, TOIE0);
		#endif

		/*Setup the OC0 Pin*/
		TCCR0 &= TIMER0_COM_MASK;
		TCCR0 |= TIMER0_OC0_MODE;

	#elif TIMER0_WGM_MODE == TIMER0_WGM_CTC_MODE
		TIM0_vSetupCTC();
	#elif TIMER0_WGM_MODE == TIMER0_WGM_FAST_PWM_MODE
		TIM0_vSetupFastPWM();
	#elif TIMER0_WGM_MODE == TIMER0_WGM_PWM_PHASE_CORRECT_MODE
		TIM0_vSetupPhaseCorrectPWM();
	#endif

	/*Disable force output compare by default*/
	CLR_BIT(TCCR0, FOC0);

#else

#endif
}

/**********************************************************************************************************
 * Description : Interface Function to Turn on a timer
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM0_vTurnOn (){
	TCCR0 &= TIMER0_CLK_SELECT_MASK;
	TCCR0 |= TIMER0_CLK_SELECT;
}

/**********************************************************************************************************
 * Description : Interface Function to set a delay for a specific timer
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM0_vTurnOff	(){
	TCCR0 &= TIMER0_CLK_SELECT_MASK;
	TCCR0 |= TIMER0_PRESCALER_NO_CLK;
}

/**********************************************************************************************************
 * Description : Interface Function to set preload to start counting from this value
 * Outputs     : void
 * Inputs      : Preload value
 ***********************************************************************************************************/
void TIM0_vSetPreload (u8 A_u8PreloadVal){
	TCNT0 = A_u8PreloadVal;
	G_u8Timer0_Preload_Val = A_u8PreloadVal;
}

/**********************************************************************************************************
 * Description : Interface Function to set OCR0 register
 * Outputs     : void
 * Inputs      : OCR value
 ***********************************************************************************************************/
void TIM0_vSetOcr0Val (u8 A_u8OcrVal){
	OCR0 = A_u8OcrVal;
}

/**********************************************************************************************************
 * Description : Interface Function to get OCR0 register
 * Outputs     : OCR0
 * Inputs      : void
 ***********************************************************************************************************/
u8 TIM0_vGetOcr0Val (){
	return OCR0;
}

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during OCF0
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM0_vCallBack_OCF0 (ptr_func_t ptr){
	G_PTRF_TIM0_CTC = ptr;
}

/**********************************************************************************************************
 * Description : Interface Function to set the call back function that will be executed during overflow
 * Outputs     : void
 * Inputs      : pointer to function
 ***********************************************************************************************************/
void TIM0_vCallBack_OVF	(ptr_func_t ptr){
	G_PTRF_TIM0_OVF = ptr;
}

/**********************************************************************************************************
 * Description : interface Function to setup the OC0 pin duty cycle if in PWM mode
 * Outputs     : void
 * Inputs      : void
 ***********************************************************************************************************/
void TIM0_vSetDutyCycleOC0 (u8 A_u8DutyCycle){
	#if TIMER0_PWM_MODE == PWM_NON_INVERTING
		TIM0_vSetOcr0Val( ( (TIMER0_MAX_COUNT + 1) * A_u8DutyCycle ) / 100);
	#elif TIMER0_PWM_MODE == PWM_INVERTING
		TIM0_vSetOcr0Val( ( -( (A_u8DutyCycle/100) - 1) ) * (TIMER0_MAX_COUNT + 1);
	#endif
}

/**
 * Description : Interface Function to set a delay for a specific timer
 * Outputs     : void
 * Inputs      : Timer ID, required delay in milliseconds, the function to be executed
 * NOTES       : still need some work!
 **/
void  TIM0_vDelayMilli (u16 A_u16DelayMs, u8 A_u8TimerId, ptr_func_t ptr){
    u16 L_u16PrescaleVal = 1;
    f32 L_f32TimeOverFlow;
    f32 L_f32TickTime;
    u16 L_u16OverFlowCounts = 0;
    u16 L_u8PreloadVal = 0;
	/* Resetting the ISR Counter */
	G_u16ISRCounter = 0;

    switch(TIMER0_CLK_SELECT){
        case TIMER0_PRESCALER_8:    L_u16PrescaleVal = 8;     break;
        case TIMER0_PRESCALER_64:   L_u16PrescaleVal = 64;    break;
        case TIMER0_PRESCALER_256:  L_u16PrescaleVal = 256;	  break;
        case TIMER0_PRESCALER_1024: L_u16PrescaleVal = 1024;  break;
    }

    /* Tick time Calculation */
    L_f32TickTime = (f32)L_u16PrescaleVal / TIMER0_InputFreq;

    /* Time required for complete 1 over flow */
    L_f32TimeOverFlow = (TIMER0_MAX_COUNT + 1) * (L_f32TickTime * ((u16)1000));

    /* Over flow needed by this delay */
    L_u16OverFlowCounts = (u16) ceil(( ((f32)A_u16DelayMs /L_f32TimeOverFlow) ));

    /* Calculating OverFlow counts & preload value */
    L_u8PreloadVal = (f32)L_u16OverFlowCounts - ((f32)A_u16DelayMs /L_f32TimeOverFlow);
    G_u16Timer0_Cov = L_u16OverFlowCounts;


#if  TIMER0_WGM_MODE == TIMER0_WGM_CTC_MODE
        TIM0_vSetOcr0Val((TIMER0_MAX_COUNT - L_u8PreloadVal));
        G_PTRF_TIM0_CTC = ptr;
#elif TIMER0_WGM_MODE == TIMER0_WGM_NORMAL_MODE
        G_PTRF_TIM0_OVF = ptr;
		TIM0_vSetPreload(L_u8PreloadVal);
#endif
}



/**************   TIMER0 mode functions   **************/

static void TIM0_vSetupCTC(){
	/*Setup the mode*/
	TCCR0 &= TIMER0_WGM_MASK;
	TCCR0 |= TIMER0_WGM_MODE;

	/*Setup the OC0 Pin*/
	TCCR0 &= TIMER0_COM_MASK;
	TCCR0 |= TIMER0_OC0_MODE;

	#if TIMER0_OC0_MODE == TIMER0_OC0_DISCONNECT

	#else
		/*Must set the OC0 pin output if in CTC, fast PWM or phase correct PWM modes*/
		DIO_vSetPinDir(OC0_PORT, OC0_PIN, DIR_OUTPUT);
	#endif

	#if TIMER0_CTC_INT_STATE == INT_ENABLE
		SET_BIT(TIMSK, OCIE0);
	#endif

	return;
}

static void TIM0_vSetupFastPWM	(){
	TCCR0 &= TIMER0_WGM_MASK;
	TCCR0 |= TIMER0_WGM_MODE;

	/*Step 2: Setup the OC0 Pin*/
	#if TIMER0_PWM_MODE == PWM_INVERTING
		SET_BIT(TCCR0, COM00);
		SET_BIT(TCCR0, COM01);
	#elif TIMER0_PWM_MODE == PWM_NON_INVERTING
		CLR_BIT(TCCR0, COM00);
		SET_BIT(TCCR0, COM01);
	#else

	#endif

	/*Calculate the dutyCycle*/
	TIM0_vSetDutyCycleOC0(TIMER0_DUTY_CYCLE);

	/*Must set the OC0 pin output if in CTC, fast PWM or phase correct PWM modes*/
	DIO_vSetPinDir(OC0_PORT, OC0_PIN, DIR_OUTPUT);

	return;
}

static void TIM0_vSetupPhaseCorrectPWM	(){
	TCCR0 &= TIMER0_WGM_MASK;
	TCCR0 |= TIMER0_WGM_MODE;

	/*Step 2: Setup the OC0 Pin*/
	#if TIMER0_PWM_MODE == PWM_INVERTING
		SET_BIT(TCCR0, COM00);
		SET_BIT(TCCR0, COM01);
	#elif TIMER0_PWM_MODE == PWM_NON_INVERTING
		CLR_BIT(TCCR0, COM00);
		SET_BIT(TCCR0, COM01);
	#else

	#endif

	/*Must set the OC0 pin output if in CTC, fast PWM or phase correct PWM modes*/
	DIO_vSetPinDir(OC0_PORT, OC0_PIN, DIR_OUTPUT);

	return;
}


/*TIMER0 COMP*/
void __vector_10(void){
	if(G_PTRF_TIM0_CTC != ADDRESS_NULL){
		G_PTRF_TIM0_CTC();
	} else {
		/*Handle callback error*/
	}
}

/* TIMER0 OVF */
void __vector_11(void)
{
	if(G_PTRF_TIM0_OVF != ADDRESS_NULL)
	{
		if(G_u16ISRCounter == G_u16Timer0_Cov)
		{
			G_u16ISRCounter = 0;
			G_PTRF_TIM0_OVF();
			TCNT0 = G_u8Timer0_Preload_Val;
		 } else {
			G_u16ISRCounter++;
			}
	} else {
		/* Handle callback error */
	  }
	return;
}