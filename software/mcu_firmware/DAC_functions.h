//Arturs Orbidans, for Master's thesis
//Functions to control DAC (set output voltage) 

#ifndef DAC_FUNCTIONS_H_
#define DAC_FUNCTIONS_H_

#include <msp430.h>


unsigned char	DAC_set_value			(unsigned char,unsigned char);
unsigned char	DAC_set_stored_value	(void);

unsigned char 	DAC_gset_set 		(unsigned char);
unsigned char 	DAC_pdv_set 		(unsigned char);
unsigned char 	DAC_pd_set 			(unsigned char);
unsigned char 	DAC_standby_set 	(unsigned char);


#endif 
