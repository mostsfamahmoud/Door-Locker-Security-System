 /******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.c
 *
 * Description: Source file for the AVR TIMER driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "Macros.h"

/*******************************************************************************
 *                           Global variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application
   Volatile --> Prevent Compiler Optimization on these pointers
   Static   --> To limit the scope of this global pointer from SW scope to File Scope
                To prevent sharing this resource with unwanted Modules
*/
static volatile void(*g_Timer0_CallBackPtr)(void)  = NULL_PTR;
static volatile void(*g_Timer1_CallBackPtr)(void)  = NULL_PTR;
static volatile void(*g_Timer2_CallBackPtr)(void)  = NULL_PTR;


/*******************************************************************************
 *                           INTERRUPT SERVICE ROUTINE                         *
 *******************************************************************************/

ISR(TIMER0_COMP_vect)
{
	if (g_Timer0_CallBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_Timer0_CallBackPtr)(); /* another method to call the function using pointer to function g_Timer0_CallBackPtr */
	}
}


ISR(TIMER0_OVF_vect)
{
	if (g_Timer0_CallBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_Timer0_CallBackPtr)(); /* another method to call the function using pointer to function g_Timer0_CallBackPtr */
	}

}


ISR(TIMER1_COMPA_vect)
{
	if (g_Timer1_CallBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_Timer1_CallBackPtr)(); /* another method to call the function using pointer to function g_Timer1_CallBackPtr */
	}
}


ISR(TIMER1_OVF_vect)
{
	if (g_Timer1_CallBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_Timer1_CallBackPtr)(); /* another method to call the function using pointer to function g_Timer1_CallBackPtr */
	}
}

ISR(TIMER2_COMP_vect)
{
	if (g_Timer2_CallBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_Timer2_CallBackPtr)(); /* another method to call the function using pointer to function g_Timer2_CallBackPtr */
	}
}


ISR(TIMER2_OVF_vect)
{
	if (g_Timer2_CallBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_Timer2_CallBackPtr)(); /* another method to call the function using pointer to function g_Timer2_CallBackPtr */
	}
}

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/


/*
 Description:
     Function to initialize the Timer driver
 Inputs:
     Pointer to the configuration structure with type Timer1_ConfigType.
*/
void Timer_init(const Timer_ConfigType *Config_Ptr)
{
	/* Check on the Timer ID if it's Timer0 or not
	 * Check on the status of the prescaler if it's DUMMY value or not
	 */
	if ((Config_Ptr->timerId == TIMER0) && (Config_Ptr->prescaler != DUMMY))
	{
		/* Set the initial value of Timer0 that will start counting from it */
		TCNT0 = Config_Ptr->initial_value;

		/* As Prescaler value isn't Dummy value, then put it in TCCR0 register */
		TCCR0 = (Config_Ptr->prescaler << CS00);

		if (Config_Ptr->mode == OVERFLOW_MODE)
		{
			/* Configure Timer0 control register TCCR0:
			 * 1. Normal port operation, OC0 disconnected --> COM01=0 COM00=0
			 * 2. Non PWM Mode --> FOC0=1
			 * 3. Normal Mode WGM01=0 WGM00=0 (Mode Number 0)
			 */
			TCCR0 |= (1 << FOC0);

			/* Enable Timer/Counter0 Overflow Interrupt */
			SET_BIT(TIMSK,TOIE0);
		}
		else if (Config_Ptr->mode == COMPARE_MODE)
		{
			/* Set the Compare value dynamically according to user configurations */
			OCR0 = Config_Ptr->compare_value;

			/* Configure Timer0 control register TCCR0:
			 * 1. Clear OC0 on compare match (Set output to low level) --> COM01=1 COM00=0
			 * 2. Non PWM Mode --> FOC0=1
			 * 3. CTC Mode WGM01=1 WGM00=0 (Mode Number 2)
			 */
			TCCR0 |= (1 << FOC0) | (1 << COM01) | (1 << WGM01);

			/* Enable Timer0 Compare Interrupt */
			SET_BIT(TIMSK,OCIE0);
		}
		else if (Config_Ptr->mode == FAST_PWM_MODE)
		{
			/* Set the Compare value dynamically according to user configurations */
			OCR0 = Config_Ptr->compare_value;

			/* Configure timer control register
			 * 1. Fast PWM mode FOC0=0
			 * 2. Fast PWM Mode WGM01=1 & WGM00=1
			 * 3. Clear OC0 when match occurs (non inverted mode) COM01=1 & COM00=0
			 */
			TCCR0 |= (1 << COM01) | (1 << WGM01) | (1 << WGM00);

			/* Set PB3/OC0 as Output pin --> pin where the PWM signal is generated from MC. */
			GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);
		}
	}

	/* Check on the Timer ID if it's Timer1 or not
	 * Check on the status of the prescaler if it's DUMMY value or not
	 */
	else if ((Config_Ptr->timerId == TIMER1) && (Config_Ptr->prescaler != DUMMY))
	{
		/* Set the initial value of Timer1 that will start counting from it */
		TCNT1 = Config_Ptr->initial_value;

		/* As Prescaler value isn't Dummy value, then put it in TCCR1B register */
		TCCR1B = (Config_Ptr->prescaler << CS10);

		if (Config_Ptr->mode == OVERFLOW_MODE)
		{
			/* Configure Timer1 control register TCCR1A:
			 * 1. Normal port operation, OC1A/OC1B disconnected --> COM1A1=0 COM1A0=0
			 * 2. Non PWM Mode --> FOC1A=1
			 * 3. Normal Mode WGM10=0 WGM11=0 (Mode Number 0)
			 */
			TCCR1A = (1 << FOC1A);

			/* Enable Timer/Counter1 Overflow Interrupt */
			SET_BIT(TIMSK,TOIE1);
		}
		else if (Config_Ptr->mode == COMPARE_MODE)
		{
			/* Set the Compare value dynamically according to user configurations */
			OCR1A = Config_Ptr->compare_value;

			/* Configure Timer1 control register TCCR1A:
			 * 1. Clear OC1A on compare match (Set output to low level) --> COM1A1=1 COM1A0=0
			 * 2. Non PWM Mode --> FOC1A=1
			 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
			 */
			TCCR1A = (1 << FOC1A) | (1 << COM1A1);

			/* Configure Timer1 control register TCCR1B:
			 * CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
			 */
			TCCR1B |= (1 << WGM12);

			/* Enable Timer1 Compare A Interrupt */
			SET_BIT(TIMSK,OCIE1A);
		}

		else if (Config_Ptr->mode == FAST_PWM_MODE)
		{
			/* Set the Compare value dynamically according to user configurations */
			OCR1A = Config_Ptr->compare_value;

			/* Configure timer control register TCCR1A
		     * 1. Clear OC1A on compare match (non inverting mode) COM1A1=1 COM1A0=0
		     * 2. Disconnect OC1B  COM1B0=0 COM1B1=0
		     * 3. PWM Mode --> FOC1A=0 FOC1B=0
		     * 4. Fast Pwm Mode with the TOP in OCR1A WGM10=1 WGM11=1 (Mode Number 15)
			 */
			TCCR1A = (1 << COM1A1) | (1 << WGM10) | (1 << WGM11);

			/* Configure timer control register TCCR1B
			 * Fast Pwm Mode with the TOP in OCR1A WGM12=1 WGM13=1 (Mode Number 15)
		     */
			TCCR1B |= (1 << WGM12) | (1 << WGM13);

			/* Set PD5/OC1A as Output pin --> pin where the PWM signal is generated from MC. */
			GPIO_setupPinDirection(PORTD_ID, PIN5_ID, PIN_OUTPUT);
		}
	}

	/* Check on the Timer ID if it's Timer2 or not
	 * Check on the status of the prescaler if it's DUMMY value or not
	 */
	else if ((Config_Ptr->timerId == TIMER2) && (Config_Ptr->prescaler2 != DUMMY))
	{
		/* Set the initial value of Timer2 that will start counting from it */
		TCNT2 = Config_Ptr->initial_value;

		/* As Prescaler value isn't Dummy value, then put it in TCCR2 register */
		TCCR2 = (Config_Ptr->prescaler2 << CS20);


		if (Config_Ptr->mode == OVERFLOW_MODE)
		{
			/* Configure Timer2 control register TCCR2:
			 * 1. Normal port operation, OC2 disconnected --> COM21=0 COM20=0
			 * 2. Non PWM Mode --> FOC2=1
			 * 3. Normal Mode WGM21=0 WGM20=0 (Mode Number 0)
			 */
			TCCR2 |= (1 << FOC2);

			/* Enable Timer/Counter2 Overflow Interrupt */
			SET_BIT(TIMSK,TOIE2);
		}
		else if (Config_Ptr->mode == COMPARE_MODE)
		{
			/* Set the Compare value dynamically according to user configurations */
			OCR2 = Config_Ptr->compare_value;

			/* Configure Timer2 control register TCCR2:
			 * 1. Clear OC2 on compare match (Set output to low level) --> COM21=1 COM20=0
			 * 2. Non PWM Mode --> FOC2=1
			 * 3. CTC Mode WGM21=1 WGM20=0 (Mode Number 2)
			 */
			TCCR2 |= (1 << FOC2) | (1 << COM21) | (1 << WGM21);

			/* Enable Timer2 Compare Interrupt */
			SET_BIT(TIMSK,OCIE2);
		}
		else if (Config_Ptr->mode == FAST_PWM_MODE)
		{
			/* Set the Compare value dynamically according to user configurations */
			OCR2 = Config_Ptr->compare_value;

			/* Configure Timer2 control register TCCR2:
			 * 1. Fast PWM mode FOC2=0
			 * 2. Fast PWM Mode WGM21=1 & WGM20=1
			 * 3. Clear OC2 when match occurs (non inverted mode) COM21=1 & COM20=0
			 */
			TCCR2 |= (1 << COM21) | (1 << WGM21) | (1 << WGM20);

			/* Set PD7/OC2 as Output pin --> pin where the PWM signal is generated from MC. */
			GPIO_setupPinDirection(PORTD_ID, PIN7_ID, PIN_OUTPUT);
		}
	}
}

/*
 Description:
     Function to set the Call Back function address.
 Inputs:
    1) Pointer to Call Back function.
    2) Variable of enum type Timer_ID to select which Timer you want to set Call Back function for it
*/
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID timerID)
{
	if (timerID == TIMER0)
	{
		g_Timer0_CallBackPtr = a_ptr;
	}
	else if (timerID == TIMER1)
	{
		g_Timer1_CallBackPtr = a_ptr;
	}

	else if (timerID == TIMER2)
	{
		g_Timer2_CallBackPtr = a_ptr;
	}
	else
		return;
}

/*
 Description:
     Function to disable the Timer1.
 Inputs:
     Takes variable of enum type Timer_ID to select which Timer that user wants to de-initialize
*/
void Timer_deInit(Timer_ID timerID)
{
	if (timerID == TIMER0)
	{
		/* Clearing all Timer0 Registers */
		TCCR0 = 0;
		TCNT0 = 0;
		OCR0 = 0;
		CLEAR_BIT(TIMSK,TOIE0);
		CLEAR_BIT(TIMSK,OCIE0);

		/* NULLing the call back pointer of Timer0 */
		g_Timer0_CallBackPtr = NULL_PTR;
	}
	else if (timerID == TIMER1)
	{
		/* Clearing all Timer1 Registers */
		TCCR1A = 0;
		TCCR1B = 0;
		TCNT1 = 0;
		OCR1A = 0;
		CLEAR_BIT(TIMSK,TOIE1);
		CLEAR_BIT(TIMSK,OCIE1A);

		/* NULLing the call back pointer of Timer1 */
		g_Timer1_CallBackPtr = NULL_PTR;
	}

	else if (timerID == TIMER2)
	{
		/* Clearing all Timer2 Registers */
		TCCR2 = 0;
		TCNT2 = 0;
		OCR2 = 0;
		CLEAR_BIT(TIMSK,TOIE2);
		CLEAR_BIT(TIMSK,OCIE2);

		/* NULLing the call back pointer of Timer2 */
		g_Timer2_CallBackPtr = NULL_PTR;
	}
	else
		return;        /* For any invalid input */
}
