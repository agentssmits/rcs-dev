//Arturs Orbidans, for Master's thesis
//Functions to control DAC (set output voltage) 

#include "DAC_functions.h"
#include "RCS_auxiliary.h"


//unsigned char DAC_set_value(unsigned char data_msb, unsigned char data_lsb)
//{
//	static const unsigned char dac_register=0b00001111;
//
//	if (data_msb>15) return 0b00011110;
//	SPI_SS_DAC_port &= ~SPI_SS_DAC_pin;
//	//SPI_RCS_send_byte((dac_register<<4)|((data_msb&0b11110000)>>4));
//	SPI_RCS_send_byte((dac_register<<4)|((data_msb&0b00001111)));
//
//	SPI_RCS_send_byte(data_lsb);
//	SPI_SS_DAC_port |= SPI_SS_DAC_pin;
//
//	store_dac(&data_msb,&data_lsb);
//
//	return 0b00011111;
//}

//unsigned char DAC_set_stored_value(void)
//{
//	DAC_set_value(
//		get_stored_settings(index_dac_msb),
//		get_stored_settings(index_dac_lsb));
//	return 1;
//}
	

//unsigned char DAC_gset_set (unsigned char val)
//{
//	if (val == 1) DAC_gset_port |= DAC_gset_pin; else
//		if (val == 0 )DAC_gset_port &= ~ DAC_gset_pin; else
//			return 0;
//	return 1;
//}

//unsigned char DAC_pdv_set (unsigned char val)
//{
//	if (val == 1) DAC_pdv_port |= DAC_pdv_pin; else
//		if (val==0 )DAC_pdv_port &= ~ DAC_pdv_pin; else
//			return 0;
//	return 1;
//}

//unsigned char DAC_pd_set (unsigned char val)
//{
//	if (val == 1) DAC_pd_port |= DAC_pd_pin; else
//		if (val==0 )DAC_pd_port &= ~ DAC_pd_pin; else
//			return 0;
//	return 1;
//}

unsigned char DAC_standby_set (unsigned char val)
{
	if (val == 1) DAC_standby_port |= DAC_standby_pin; else
		if (val==0 )DAC_standby_port &= ~ DAC_standby_pin; else
			return 0;
	return 1;
}



