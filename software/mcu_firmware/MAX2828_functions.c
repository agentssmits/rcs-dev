//Arturs Orbidans, for Master's thesis
//Functions to control MAX2828 transmitter

#include "MAX2828_functions.h"
#include "RCS_auxiliary.h"


unsigned char MAX2828_get_phase_lock_statuss(void)
{
	if (MAX2828_lock_port & MAX2828_lock_pin) return 1;
	return 0;
}

unsigned char MAX2828_TX_set(unsigned char val)
{
	if (val == 1)
	{
		MAX2828_tx_en_port |= MAX2828_tx_en_pin;
		store_max2828_gpio_settings(1,1);
		return 0b01001101;
	} else
	if (val == 0)
	{
		MAX2828_tx_en_port &= ~MAX2828_tx_en_pin;
		store_max2828_gpio_settings(1,0);
		return 0b01001011;
	} else
	return 0;
}

//unsigned char MAX2828_RX_set(unsigned char val)
//{
//	if (val == 1)
//	{
//		MAX2828_rx_en_port |= MAX2828_rx_en_pin;
//		//store_max2828_gpio_settings(1,1);
//		return 0b01001101;
//	} else
//	if (val == 0)
//	{
//		MAX2828_rx_en_port &= ~MAX2828_rx_en_pin;
//		//store_max2828_gpio_settings(1,0);
//		return 0b01001011;
//	} else
//	return 0;
//}

unsigned char MAX2828_enable_set(unsigned char val)
{
	if (val == 1)
	{
		MAX2828_shdn_port |= MAX2828_shdn_pin;
		store_max2828_gpio_settings(2,1);
		return 0b01001001;
	} else
	if (val == 0)
	{
		MAX2828_shdn_port &= ~MAX2828_shdn_pin;
		store_max2828_gpio_settings(2,0);
		return 0b01001011;
	} else
	return 0;
}

unsigned char MAX2828_TX_set_power(unsigned char value)
{
	unsigned char status_code = 0;
	if (value>127){
		status_code = 0b01000110;
	}
	else{
		if (value &0x1) MAX2828_pwr_set_port1 |= MAX2828_pwr_set_pin1; else MAX2828_pwr_set_port1 &= ~MAX2828_pwr_set_pin1;
		value = value>>1;
		if (value &0x1) MAX2828_pwr_set_port2 |= MAX2828_pwr_set_pin2; else MAX2828_pwr_set_port2 &= ~MAX2828_pwr_set_pin2;
		value = value>>1;
		if (value &0x1) MAX2828_pwr_set_port3 |= MAX2828_pwr_set_pin3; else MAX2828_pwr_set_port3 &= ~MAX2828_pwr_set_pin3;
		value = value>>1;
		if (value &0x1) MAX2828_pwr_set_port4 |= MAX2828_pwr_set_pin4; else MAX2828_pwr_set_port4 &= ~MAX2828_pwr_set_pin4;
		value = value>>1;
		if (value &0x1) MAX2828_pwr_set_port5 |= MAX2828_pwr_set_pin5; else MAX2828_pwr_set_port5 &= ~MAX2828_pwr_set_pin5;
		value = value>>1;
		if (value &0x1) MAX2828_pwr_set_port6 |= MAX2828_pwr_set_pin6; else MAX2828_pwr_set_port6 &= ~MAX2828_pwr_set_pin6;
		value = value>>1;
		if (value &0x1) MAX2828_pwr_set_port7 |= MAX2828_pwr_set_pin7; else MAX2828_pwr_set_port7 &= ~MAX2828_pwr_set_pin7;
		store_max2828_power_settings(&value);
		status_code = 0b01000111;
	}
	return status_code;
}

//unsigned char MAX2828_set_tegister_values(unsigned char adress, unsigned char data_msb,unsigned char data_lsb)
unsigned char MAX2828_set_tegister_values(unsigned char adress, unsigned long int data)
{
	int i=0;
	unsigned char temp[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//	if (adress>12 || data_msb>63) return 0b01011110;
	if (adress>12 || data>16383) return 0b01011110;
	data = data << 4;
	data |= adress;
	for (i=17;i>=0;i--){
			temp[17-i]=(data >> i) & 1;
	}
	SPI_SS_MAX2828_port &= ~SPI_SS_MAX2828_pin;
//	TA1CCR1 = 19;
//	unsigned char byte1 = (data_msb&0b110000)>>4;
//	unsigned char byte2 = ((data_msb&0b1111)<<4) | ((data_lsb&0b11110000)>>4);
//	unsigned char byte3 = ((data_lsb&0b1111)<<4) | (adress&0b1111);
	for (i=0;i<18;i++){
		if(temp[i]) SPI_DIN_MAX2828_port |= SPI_DIN_MAX2828_pin;
		else SPI_DIN_MAX2828_port &= ~SPI_DIN_MAX2828_pin;
		SPI_CLK_MAX2828_port ^= SPI_CLK_MAX2828_pin;
		if(temp[i]) SPI_DIN_MAX2828_port |= SPI_DIN_MAX2828_pin;
		else SPI_DIN_MAX2828_port &= ~SPI_DIN_MAX2828_pin;
		SPI_CLK_MAX2828_port ^= SPI_CLK_MAX2828_pin;
	}

//	SPI_RCS_send_byte(byte1);
//	SPI_RCS_send_byte(byte2);
//	SPI_RCS_send_byte(byte3);

	SPI_SS_MAX2828_port |= SPI_SS_MAX2828_pin;
//	TA1CCR1 = 0;
	SPI_DIN_MAX2828_port &= ~SPI_DIN_MAX2828_pin;

//	store_max2828_registers (&adress,&data_msb,&data_lsb);
	store_max2828_registers (&adress,&data);
	return 0b01011111;
}

unsigned char MAX2828_set_GPIO_stored (void)
{
	unsigned char temp = get_stored_settings(index_max2828_gpio);
	MAX2828_TX_set(temp&0x1);
	MAX2828_enable_set(temp&0b10>>1);
	return 1;
}

unsigned char MAX2828_pwr_set_stored (void)
{
	MAX2828_TX_set_power(get_stored_settings(index_max2828_pwr));
	return 1;
}

unsigned char MAX2828_set_registers_stored (void)
{
	unsigned char i = 0;
	for (i=0;i<13; i++)
	{
//	MAX2828_set_tegister_values(i,
//			get_stored_settings(index_max2828_reg1+i*2),
//			get_stored_settings(index_max2828_reg2+i*2));
	MAX2828_set_tegister_values(i, get_stored_settings(index_max2828_reg1+i*2));
	}
	return 1;
}
