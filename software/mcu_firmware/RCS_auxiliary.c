//Arturs Orbidans, for Master's thesis
//Auxiliary functions for implementing RCS control software

#include "RCS_auxiliary.h"
#include "dpuser.h"

//Array in MCU's non-volatile FRAM memory to sotre RCS settings
#pragma NOINIT(settings_array)
//unsigned char settings_array[17];
unsigned int settings_array[17];

unsigned char init_pins()
{
	//Init SS pins for all SPI peripherals  as outputs
//	SPI_SS_DAC_ddr 		|= SPI_SS_DAC_pin;
//	SPI_SS_FPGA_ddr		|= SPI_SS_FPGA_pin;
	SPI_SS_MAX2828_ddr 	|= SPI_SS_MAX2828_pin;
	SPI_DIN_MAX2828_ddr |= SPI_DIN_MAX2828_pin;
	SPI_CLK_MAX2828_ddr |= SPI_CLK_MAX2828_pin;
	
	//Set all SS pins as 1 (default) - not active
//	SPI_SS_DAC_port		|= SPI_SS_DAC_pin;
//	SPI_SS_FPGA_port 	|= SPI_SS_FPGA_pin;
	SPI_SS_MAX2828_port |= SPI_SS_MAX2828_pin;
	SPI_DIN_MAX2828_port &= ~SPI_DIN_MAX2828_pin;//Default 0
	SPI_CLK_MAX2828_port &= ~SPI_CLK_MAX2828_pin;
	
	//set MAX2828 lock  read pins as input
	MAX2828_lock_ddr 	&= ~MAX2828_lock_pin;
	MAX2828_lock_pullup &= ~MAX2828_lock_pin;
	//Init MAX2828 related pins
	MAX2828_tx_en_ddr	|= MAX2828_tx_en_pin;
//	MAX2828_rx_en_ddr	|= MAX2828_rx_en_pin;
	MAX2828_shdn_ddr	|= MAX2828_shdn_pin;

	MAX2828_tx_en_port	&= ~MAX2828_tx_en_pin;
//	MAX2828_rx_en_port	&= ~MAX2828_rx_en_pin;
	MAX2828_shdn_port	&= ~MAX2828_shdn_pin;
	
	MAX2828_pwr_set_ddr1 |= MAX2828_pwr_set_pin1;
	MAX2828_pwr_set_ddr2 |= MAX2828_pwr_set_pin2;
	MAX2828_pwr_set_ddr3 |= MAX2828_pwr_set_pin3;
	MAX2828_pwr_set_ddr4 |= MAX2828_pwr_set_pin4;
	MAX2828_pwr_set_ddr5 |= MAX2828_pwr_set_pin5;
	MAX2828_pwr_set_ddr6 |= MAX2828_pwr_set_pin6;

	MAX2828_pwr_set_port1 &= ~MAX2828_pwr_set_pin1;
	MAX2828_pwr_set_port2 &= ~MAX2828_pwr_set_pin2;
	MAX2828_pwr_set_port3 &= ~MAX2828_pwr_set_pin3;
	MAX2828_pwr_set_port4 &= ~MAX2828_pwr_set_pin4;
	MAX2828_pwr_set_port5 &= ~MAX2828_pwr_set_pin5;
	MAX2828_pwr_set_port6 &= ~MAX2828_pwr_set_pin6;

	//init DAC related pins
//	DAC_gset_ddr	|= DAC_gset_pin;
//	DAC_pdv_ddr 	|= DAC_pdv_pin;
//	DAC_pd_ddr 		|= DAC_pd_pin;
//	DAC_standby_ddr |= DAC_standby_pin;
	osc_en_ddr		|= osc_en_pin;

//	DAC_gset_port		&= ~DAC_gset_pin;
//	DAC_pdv_port		&= ~DAC_pdv_pin;
//	DAC_pd_port			&= ~DAC_pd_pin;
//	DAC_standby_port	&= ~DAC_standby_pin;
	osc_en_port			&= ~osc_en_pin;

	P1SEL1 &= ~BIT5;
	P1SEL0 &= ~BIT5;

	return 1;
}

unsigned char osc_set (unsigned char val)
{
	if (val == 1) osc_en_port |= osc_en_pin; else
		if (val==0 )osc_en_port &= ~ osc_en_pin; else
			return 0;
	return 1;
}

unsigned char SPI_RCS_init()
{
  // Configure GPIO
  //P1SEL1 |= BIT6+BIT7;
  //P1SEL0 &= ~(BIT6+BIT7);

	P1SEL1 |= BIT6;
	P1SEL0 &= ~BIT6;

	P2SEL1 |= BIT2;
	P2SEL0 &= ~BIT2;
	P1DIR |= BIT6;

	UCB0CTLW0 |=UCSWRST;
	UCB0CTLW0 |= UCSYNC;
	UCB0CTLW0 = UCMST | UCMODE_0 | UCSSEL_2 | UCMSB;
	UCB0BRW = 100;
	UCB0CTLW0 &= ~UCSWRST;

	return 1;
}

unsigned char SPI_RCS_send_byte(unsigned char data)
{
	//Send a byte via SPI when SPI is ready for TX
	while (UCB0STATW & UCBUSY) {};
	UCB0TXBUF = data;
	while (UCB0STATW & UCBUSY) {};

	return 1;
}

//unsigned char store_dac(unsigned char* data_msb, unsigned char* data_lsb)
//{
//	settings_array[index_dac_msb] = *data_msb;
//	settings_array[index_dac_lsb] = *data_lsb;
//	return 1;
//}

unsigned char store_max2828_power_settings (unsigned char* data)
{
	settings_array[index_max2828_pwr] = *data;
	return 1;
}

unsigned char store_max2828_gpio_settings (unsigned char bit_no, unsigned char state)
{
	//Clears or sets specified bit in register
	if (state) settings_array[index_max2828_gpio] |= (1<<bit_no); else settings_array[3] &= ~(1<<bit_no);
	return 1;
}

//unsigned char store_max2828_registers (unsigned char* register_no, unsigned char* data_msb, unsigned char* data_lsb)
unsigned char store_max2828_registers (unsigned char* register_no, unsigned long int* data)
{
//	unsigned int offset = *register_no *2;
//	settings_array[index_max2828_reg1+offset]	=  *data_msb;
//	settings_array[index_max2828_reg2+offset]	=  *data_lsb;
	unsigned int offset = *register_no;
	settings_array[index_max2828_reg1+offset]	=  *data;
	return 1;
}

signed char get_stored_settings (unsigned char requested_byte)
{
	if (requested_byte > settings_array_size-1)
		{
			#ifdef ENABLE_DISPLAY
			dp_display_text("Incorrect index!");
			#endif
			return -1;
		}
	return settings_array[requested_byte];
}


//Timer B0 counts up to 10; needed for timing of the pause in
//escape sequence
unsigned char init_timer_b0 (void)
{
	TB0R = 0;
	TB0CTL = TBSSEL_1 | MC_1 | ID_3 ; //Clk - ACLK, DIV 8
	TB0CCR0 = 10;
	TB0CCTL0 = CCIE;				 //Enale interrupts
	TB0EX0 = TBIDEX_7;				 //Clk DIV 8
	
	return 1;
}


