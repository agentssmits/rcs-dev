//Arturs Orbidans, for Master's thesis
//Functions to control MAX2828 transmitter

#ifndef MAX2828_FUNCTIONS_H_
#define MAX2828_FUNCTIONS_H_


#include <msp430.h>

unsigned char MAX2828_get_phase_lock_statuss	(void);
unsigned char MAX2828_TX_set					(unsigned char);
unsigned char MAX2828_RX_set					(unsigned char);
unsigned char MAX2828_enable_set				(unsigned char);
unsigned char MAX2828_TX_set_power				(unsigned char);
//unsigned char MAX2828_set_tegister_values		(unsigned char,unsigned char,unsigned char);
unsigned char MAX2828_set_tegister_values		(unsigned char, unsigned long int);

unsigned char MAX2828_set_GPIO_stored			(void);
unsigned char MAX2828_pwr_set_stored			(void);
unsigned char MAX2828_set_registers_stored		(void);
#endif 
