//Arturs Orbidans, for Master's thesis
//Functions to control ADC (get current sensor output) 

#ifndef ADC_FUNCTIONS_H_
#define ADC_FUNCTIONS_H_


#include <msp430.h>
#include <stdint.h>


unsigned char	ADC_init		(unsigned char);
uint16_t		ADC_get_sample	(unsigned char);
unsigned char	ADC_disable		(unsigned char);

#endif 
