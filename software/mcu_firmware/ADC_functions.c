//Arturs Orbidans, for Master's thesis
//Functions to control ADC (get current sensor output) 

#include "ADC_functions.h"

unsigned char ADC_init(unsigned char pin)
{
	/*
	//Analogue inputs P1.0 and P1.1 are used
	P1SEL1 |= (BIT0 |BIT1);                          
	P1SEL0 |= (BIT0 |BIT1);
	
	// Configure ADC12
	ADC12CTL0 = ADC12SHT0_2 | ADC12ON;        	// Sampling time, S&H=16, ADC12 on
	ADC12CTL1 = ADC12SHP;                     	// Use sampling timer
	ADC12CTL2 |= ADC12RES_2;                  	// 12-bit conversion results
	if (pin == 0) ADC12MCTL0 |= ADC12INCH_0;	// ADC input select; Vref=AVCC
		else if (pin == 1) ADC12MCTL0 |= ADC12INCH_1;
			else return 0;

	return 1;
	*/

	//Analogue inputs P1.0 and P1.1 are used
	P1SEL1 |= (BIT0 |BIT1);
	P1SEL0 |= (BIT0 |BIT1);

	// Configure ADC12
	ADC12CTL0 = ADC12SHT0_2 | ADC12ON;        	// Sampling time, S&H=16, ADC12 on
	ADC12CTL1 = ADC12SHP;                     	// Use sampling timer
	ADC12CTL2 |= ADC12RES_2;                  	// 12-bit conversion results

	if (pin == 0) ADC12MCTL0 = ADC12INCH_2;	// ADC input select; Vref=AVCC
		else if (pin == 1) ADC12MCTL0 = ADC12INCH_3;
			else return 0;

	return 1;
}

uint16_t ADC_get_sample(unsigned char pin)
{
	/*
	ADC12CTL0 |= ADC12ENC | ADC12SC;
	while (!(ADC12CTL1&ADC12BUSY)) {};
	if (pin == 0) return ADC12MEM0;
		else if (pin == 1) return ADC12MEM1;
			else return 0;
			*/
	ADC12CTL0 |= ADC12ENC | ADC12SC;
	//while (!(ADC12CTL1&ADC12BUSY)) {};
	if (pin==0)
	{
	while (!(ADC12IFGR0&ADC12IFG0)) {};
	}
	else
	{
	while (!(ADC12IFGR0&ADC12IFG0)) {};
	};

	if (pin == 0) return ADC12MEM0;
		else if (pin == 1) return ADC12MEM0;
			else return 0;
}

unsigned char ADC_disable(unsigned char pin)
{
	ADC12CTL0 = 0; 
	return 1;
}
