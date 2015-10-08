//Arturs Orbidans, for Master's thesis
//Auxiliary functions for implementing RCS control software

#ifndef RCS_AUXILIARY_H_
#define RCS_AUXILIARY_H_

#include <MSP430.h>

//Define SS pins for peripherals
//#define SPI_SS_DAC_pin			BIT5
//#define SPI_SS_FPGA_pin			BIT6
#define SPI_CLK_MAX2828_pin			BIT2
#define SPI_DIN_MAX2828_pin			BIT6
#define SPI_SS_MAX2828_pin			BIT0//BIT7

//#define SPI_SS_DAC_port			P3OUT
//#define SPI_SS_FPGA_port			P3OUT
#define SPI_CLK_MAX2828_port		P1OUT
#define SPI_DIN_MAX2828_port		P1OUT
#define SPI_SS_MAX2828_port			P3OUT

//#define SPI_SS_DAC_ddr			P3DIR
//#define SPI_SS_FPGA_ddr			P3DIR
#define SPI_CLK_MAX2828_ddr			P1DIR
#define SPI_DIN_MAX2828_ddr			P1DIR
#define SPI_SS_MAX2828_ddr			P3DIR

//#define DAC_gset_pin			BIT1
//#define DAC_gset_port			P4OUT
//#define DAC_gset_ddr			P4DIR

//#define DAC_pdv_pin				BIT4
//#define DAC_pdv_port			P3OUT
//#define DAC_pdv_ddr				P3DIR

//#define DAC_pd_pin				BIT3
//#define DAC_pd_port				P4OUT
//#define DAC_pd_ddr				P4DIR

#define DAC_standby_pin			BIT2
#define DAC_standby_port		P4OUT
#define DAC_standby_ddr			P4DIR

#define osc_en_pin				BIT5
#define osc_en_port				P1OUT
#define osc_en_ddr				P1DIR

//Define current sensor inputs
//#define sensor1_pin				BIT0
//#define sensor1_port			P1OUT
//#define sensor1_ddr				P1DIR

//#define sensor2_pin				BIT0
//#define sensor2_port			P1OUT
//#define sensor2_ddr				P1DIR

//Define MAX2828 related pins
#define MAX2828_lock_pin		BIT7//BIT2
#define MAX2828_lock_port		P1IN//P1IN
#define MAX2828_lock_ddr		P1DIR
#define MAX2828_lock_pullup		P1REN

#define MAX2828_tx_en_pin		BIT4//BIT4
#define MAX2828_tx_en_port		P1OUT//PJOUT
#define MAX2828_tx_en_ddr		P1DIR//PJDIR

//#define MAX2828_rx_en_pin		BIT0
//#define MAX2828_rx_en_port		P1OUT
//#define MAX2828_rx_en_ddr		P1DIR

#define MAX2828_shdn_pin		BIT3//BIT0
#define MAX2828_shdn_port		P1OUT//P3OUT
#define MAX2828_shdn_ddr		P1DIR//P3DIR

#define MAX2828_pwr_set_pin1	BIT6//BIT2
#define MAX2828_pwr_set_pin2	BIT5//BIT1
#define MAX2828_pwr_set_pin3	BIT4//BIT7
#define MAX2828_pwr_set_pin4	BIT2//BIT3
#define MAX2828_pwr_set_pin5	BIT4//BIT4
#define MAX2828_pwr_set_pin6	BIT3//BIT3
#define MAX2828_pwr_set_pin7	BIT2//BIT0

#define MAX2828_pwr_set_port1	P3OUT
#define MAX2828_pwr_set_port2	P3OUT
#define MAX2828_pwr_set_port3	P3OUT//P4OUT
#define MAX2828_pwr_set_port4	P2OUT//P3OUT
#define MAX2828_pwr_set_port5	P2OUT//P1OUT
#define MAX2828_pwr_set_port6	P4OUT//P1OUT
#define MAX2828_pwr_set_port7	P4OUT

#define MAX2828_pwr_set_ddr1	P3DIR
#define MAX2828_pwr_set_ddr2	P3DIR
#define MAX2828_pwr_set_ddr3	P3DIR//P4DIR
#define MAX2828_pwr_set_ddr4	P2DIR//P3DIR
#define MAX2828_pwr_set_ddr5	P2DIR//P1DIR
#define MAX2828_pwr_set_ddr6	P4DIR//P1DIR
#define MAX2828_pwr_set_ddr7	P4DIR

//#define index_dac_msb		0
//#define index_dac_lsb		1
#define index_max2828_pwr	2
#define index_max2828_gpio	3
#define index_max2828_reg1	4
//#define index_max2828_reg2	5


#define settings_array_size	17

#include <msp430.h>


unsigned char 	init_pins						(void);					//Init MCU pins 
unsigned char 	osc_set 						(unsigned char);
unsigned char	SPI_RCS_init					(void);					//Init SPI realised by eUSCI B0
unsigned char	SPI_RCS_send_byte				(unsigned char);	

//Functions to store DAC, MAX2828 settings
//in case of brown out etc.
unsigned char	store_dac						(unsigned char*,unsigned char*);
unsigned char	store_max2828_power_settings	(unsigned char*);
unsigned char	store_max2828_gpio_settings		(unsigned char,unsigned char);
//unsigned char	store_max2828_registers			(unsigned char*,unsigned char*,unsigned char*);
unsigned char	store_max2828_registers			(unsigned char*,unsigned long int*);

//Read stored RCS settings by accessing a single byte
//address specified in storage array
signed	 char	get_stored_settings				(unsigned char);
//Other auxiliary functions
unsigned char	init_timer_b0					(void);

#endif 
