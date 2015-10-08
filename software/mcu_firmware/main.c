/*
Authors:
Arturs Orbidans
Raivis Paberz
Roberts Trops
*/

#include <msp430.h>
#include "dpuser.h"
#include "RCS_auxiliary.h"
#include "MAX2828_functions.h"
#include "DAC_functions.h"
#include "ADC_functions.h"

#define bin_mode		1
//Menu command numbers:
#define enable_chip 	1
#define disable_chip	2
#define enable_tx		3
#define disable_tx		4
//#define enable_rx		5
//#define disable_rx		6
#define set_pwr			7
#define set_reg			8
#define get_phase_lck	9
//#define set_dac			10
#define stdby_high		11
#define stdby_low		12
//#define gset_high		13
//#define gset_low		14
//#define pdv_high		15
//#define pdv_low			16
//#define pd_high			17
//#define pd_low			18
//#define read_adc		19
#define osc_en			20
#define osc_dis			21
#define reset_settings	99
#define menu			0
#define UART_RX_BUFFER_SIZE		16

#define reg_count   13 // number of configurable registers in max2828
#define reg_bits    14 // number of bits in max2828 conf. register

//Define UART RX buffer and its size
static	unsigned char rx_a0_buffer_index 	= 0;
#pragma NOINIT(UART_RX_BUFFER)
unsigned char UART_RX_BUFFER[UART_RX_BUFFER_SIZE];

unsigned char	UART_read_buffer 	(void);
void			display_main_menu	(void);
void 			display_error		(void);
uint16_t 		get_number			(char);
uint16_t		bin_to_int			(unsigned char*);

// Default register values of max2828
uint16_t default_reg[reg_count]=
{
    0b01000101000000, // R0
    0b00000011001010, // R1
    0b01000000000111, // R2
    0b11000011101000, // R3
    0b00110011001100, // R4
    0b01111101100011, // R5
    0b01110000000000, // R6
    0b00000000101010, // R7
    0b00000000100101, // R8
    0b00001000000000, // R9
    0b00001111000000, // R10
    0b00000001111111, // R11
    0b00000000000000  // R12
};

// Registers defined by user
uint16_t user_reg[reg_count] = {0};

// Function for coping arrays
void cpy_array(uint16_t *dst_array, uint16_t *src_array, uint8_t ARRAY_SIZE){
    int i;
    for (i = 0; i != ARRAY_SIZE; ++i){
            dst_array[i] = src_array[i];
    }
}

void print_bin(uint16_t value, uint8_t bits){
    int i;
    for(i=bits-1; i>=0 ; i--){
        if(((value>>i) & 0x01)==1){
        	dp_display_text("1");
        }
        else{
        	dp_display_text("0");
        }
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	//SET master clock to 8 MHz
    CSCTL0_H	= CSKEY>>8;			//CSKEY password
	CSCTL1		= DCOFSEL_6;  		//Set DCO 8 MHZ
	CSCTL2		= SELM_3 | SELS_3 | SELA_2;
	CSCTL3		= DIVM__1| DIVS__1;
	CSCTL0_H	= 0;

	// Configure Timer1_A
//	P1DIR |= BIT2;             // P1.2 to output
//	TA1CCTL1 = OUTMOD_7;                    // Reset/Set Mode
//	TA1CTL = TASSEL_2 + MC_1 +TACLR;       // SMCLK / Upmode
//	TA1CCR0 = 38-1;                        // PWM Frequency 1.33 MHz
//	TA1CCR1 = 0;                           // 50% Duty Cycle
////	TA1CCR0 = 60-1;                        // PWM Frequency 10 kHz
////	TA1CCR1 = 30;                           // 50% Duty Cycle
//	P1SEL0 |= BIT2;                         // PWM output to LED P1.0
//
//	//TA1CCR2 = 2;
//	TA1CCR2 = 130;
//	TA1CCTL2 = CCIE;

	dp_delay(500);

    uart_a0_init();
    init_pins();
//  SPI_RCS_init();
    //ADC_init(1);
    //ADC_init(0);

    //Board specific assigment
    PJDIR &= ~BIT5;

    PM5CTL0 &= ~LOCKLPM5;	// Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    char 			action_code 		= 0;
    unsigned char 	temp,temp2,temp3 	= 0;
    unsigned long int in_val = 0;
	uint8_t k;

    display_main_menu();

    cpy_array(user_reg, default_reg,reg_count); // Copy default values in user defined
    //Main menu
    while(1)
    {
    dp_display_text("\r\nEnter action code & press enter: ");

    action_code = get_number(DEC);
    switch(action_code)
    {
    case menu:
    	//Display main menu
    	display_main_menu();
    	break;
    case enable_chip:
    	//Enable chip (power on)
    	if (MAX2828_enable_set(1) == 0b01001001)
    		dp_display_text("\r\n--->Chip enabled");
    	else
    		display_error();
    	break;
    case disable_chip:
    	//Disable chip (power off)
    	if (MAX2828_enable_set(0) == 0b01001011)
    		dp_display_text("\r\n--->Chip disabled");
    	else
    		display_error();
    	break;
    case enable_tx:
    	//Enable TX
    	if (MAX2828_TX_set(1) == 0b01001101)
    		dp_display_text("\r\n--->TX on");
    	else
    		dp_display_text("\r\n--->Error");
    	break;
    case disable_tx:
    	//Disable TX
    	if (MAX2828_TX_set(0) == 0b01001011)
    		dp_display_text("\r\n--->TX off");
    	else
    		dp_display_text("\r\n--->Error");
    	break;
//    case enable_rx:
//        //Enable RX
//        if (MAX2828_RX_set(1) == 0b01001101)
//        	dp_display_text("\r\n--->RX on");
//        else
//        	display_error();
//        break;
//    case disable_rx:
//        //Disable RX
//        if (MAX2828_RX_set(0) == 0b01001011)
//        	dp_display_text("\r\n--->RX off");
//        else
//        	display_error();
//        break;
    case set_pwr:
    	dp_display_text("\r\n--->Type TX gain  [0;127] & press enter: ");
    	temp = get_number(DEC);
    	if (MAX2828_TX_set_power(temp)==0b01000111)
    	{
    		dp_display_text("\r\n--->TX gain set to: ");
    		dp_display_value(temp,DEC);
    	}
    	else
    		display_error();
    	break;
    case set_reg:
    	dp_display_text("\r\n--->Type MAX2828 register adress [0;12] & press enter: ");
    	temp = get_number(DEC);	//Adr
    	dp_display_text("\r\n--->Type MAX2828 register 14 bit binary val & press enter: ");
    	in_val = get_number(BIN);
    	temp2 = in_val>>8;		//MSB
    	temp3 = in_val&0xff;	//LSB

    	if (MAX2828_set_tegister_values(temp,in_val)==0b01011111)
    	{
    		dp_display_text("\r\n--->MAX2828 register ");
    		dp_display_value(temp,DEC);
    		dp_display_text(" is set to value: ");
    		dp_display_value(temp2<<8|temp3,DEC);
    		dp_display_text(" = ");
    		dp_display_value(temp2<<8|temp3,BIN);
    	}
    	else
    		display_error();
    	break;
//    case set_dac:
//    	//SET DAC output voltage
///*
//    	dp_display_text("\r\n--->Type DAC MSB val [0;15] & press enter: ");
//    	temp2 = get_number();
//    	dp_display_text("\r\n--->Type DAC LSB val [0;255] & press enter: ");
//    	temp3 = get_number();
//*/
//    	dp_display_text("\r\n--->Type DAC 12 bit val [0;4095] & press enter: ");
//    	in_val = get_number(DEC);
//    	temp2 = in_val>>8;	//MSB
//    	temp3 = in_val&0xff;//LSB
//    	if (DAC_set_value(temp2,temp3)==0b00011111)
//    	{
//    		dp_display_text("\r\n--->DAC is set to val: ");
//    		dp_display_value(temp2<<8|temp3,DEC);
//    	}
//    	else
//    		display_error();
//    	break;
    case get_phase_lck:
    	dp_display_text("\r\n--->MAX282 phase lock: ");
    	if (MAX2828_get_phase_lock_statuss())
    		dp_display_text("Locked");
    	else
    		dp_display_text("not locked");
    	break;
//    case read_adc:
//    	ADC_init(0);
//    	dp_display_text("\r\n--->Sens No0: ");
//    	dp_display_value(ADC_get_sample(0),DEC);
//    	ADC_init(1);
//    	dp_display_text("\r\n--->Sens No1: ");
//    	dp_display_value(ADC_get_sample(1),DEC);
//    	break;
//    case gset_low:
//    	//DAC GSET set to 0
//    	if (DAC_gset_set(0))
//    		dp_display_text("\r\n--->DAC GSET=0");
//    	else
//    		display_error();
//    	break;
//    case gset_high:
//    	//DAC GSET set to 1
//    	if (DAC_gset_set(1))
//    		dp_display_text("\r\n--->DAC GSET=1");
//    	else
//    		display_error();
//    	break;
//    case pdv_low:
//    	//DAC PDV set to 0
//    	if (DAC_pdv_set(0))
//    		dp_display_text("\r\n--->DAC PDV=0");
//    	else
//    		display_error();
//    	break;
//    case pdv_high:
//    	//DAC PDV set to 1
//    	if (DAC_pdv_set(1))
//    		dp_display_text("\r\n--->DAC PDV=1");
//    	else
//    		display_error();
//    	break;
//    case pd_low:
//    	//DAC PD set to 0
//    	if (DAC_pd_set(0))
//    		dp_display_text("\r\n--->DAC PD=0");
//    	else
//    		display_error();
//    	break;
//    case pd_high:
//    	//DAC PD set to 1
//    	if (DAC_pd_set(1))
//    		dp_display_text("\r\n--->DAC PD=1");
//    	else
//    		display_error();
//    	break;
    case stdby_low:
    	//DAC STANDBY set to 0
    	if (DAC_standby_set(0))
    		dp_display_text("\r\n--->DAC STANDBY=0");
    	else
    		display_error();
    	break;
    case stdby_high:
    	//DAC STANDBY set to 1
    	if (DAC_standby_set(1))
    		dp_display_text("\r\n--->DAC STANDBY=1");
    	else
    		display_error();
    	break;
    case reset_settings:
    	dp_display_text("\r\nREG\tHEX\tBIN");
		for(k=0;k<reg_count;k++){
			if (MAX2828_set_tegister_values(k,default_reg[k])==0b01011111)
			{
				dp_display_text("\r\nR"); dp_display_value(k,DEC);
				dp_display_text("\t");
				dp_display_value(default_reg[k],DEC);
				dp_display_text("\t");
				print_bin(default_reg[k],14);
				//dp_display_value(default_reg[k],BIN);
			}
			else{
				display_error();
			}
		}
    	dp_display_text("\r\n--->Default register settings restored.");
    	dp_display_text("PS This function is not ready! a.o.");
    	break;
    case osc_dis:
    	//Disable oscillator
    	if (osc_set(0))
    		dp_display_text("\r\n--->OSC DISABLED");
    	else
    		display_error();
    	break;
    case osc_en:
    	//Enable oscillator
    	if (osc_set(1))
    		dp_display_text("\r\n--->OSC ENABLED");
    	else
    		display_error();
    	break;
    default:
    	dp_display_text("\r\n--->Error");
    	break;
    }
    action_code = 0;
    temp2 = 0;
    temp3 = 0;
    }
	return 0;
}

unsigned char UART_read_buffer (void)
{
	if (rx_a0_buffer_index)
	{
	unsigned char temp = UART_RX_BUFFER[0];
	unsigned int i = 0;
	for (i=0; i<UART_RX_BUFFER_SIZE; i++)
		{UART_RX_BUFFER[i] = UART_RX_BUFFER[i+1];};
	rx_a0_buffer_index--;
	return  temp;
	}
	return UART_RX_BUFFER[0];
}

unsigned char UART_receive_byte_handle(unsigned char in_value)
{
	uart_a0_send_byte(in_value); //echo
	//add byte to UART RX buffer
	if ((rx_a0_buffer_index+1)==UART_RX_BUFFER_SIZE) rx_a0_buffer_index = 0;
	UART_RX_BUFFER[rx_a0_buffer_index++] = in_value;
	return 1;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	  switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
	  {
	    case USCI_NONE: break;
	    case USCI_UART_UCRXIFG:
	      UART_receive_byte_handle(UCA0RXBUF);
	      __bic_SR_register_on_exit(LPM0_bits); //wakeup
	      break;
	    case USCI_UART_UCTXIFG: break;
	    case USCI_UART_UCSTTIFG: break;
	    case USCI_UART_UCTXCPTIFG: break;
	  }
}

void display_main_menu(void)
{
    dp_display_text("\r\n\r\n\t\t--------Main menu--------");
    dp_display_text("\r\n---MAX2828 related---\t\t---DAC related---");
    dp_display_text("\r\n01 ENABLE CHIP\t\t\t10 SET DAC REGISTER");
    dp_display_text("\r\n02 DISABLE CHIP\t\t\t11 DAC: SET STANDBY HIGH");
    dp_display_text("\r\n03 ENABLE TX\t\t\t12 DAC: SET STANDBY LOW");
    dp_display_text("\r\n04 DISABLE TX\t\t\t13 DAC: SET GSET HIGH");
    dp_display_text("\r\n05 ENABLE RX\t\t\t14 DAC: SET GSET LOW");
    dp_display_text("\r\n06 DISABLE RX\t\t\t15 DAC: SET PDV HIGH");
    dp_display_text("\r\n07 SET TX GAIN\t\t\t16 DAC: SET PDV LOW");
    dp_display_text("\r\n08 SET REGISTERS\t\t17 DAC: SET PD HIGH");
    dp_display_text("\r\n09 GET PHASE LOCK PIN STAT\t18 DAC: SET PD LOW");
    dp_display_text("\r\n\t\t------Other------");
    dp_display_text("\r\n\t\t19 READ ADC");
    dp_display_text("\r\n\t\t20 ENABLE OSC");
    dp_display_text("\r\n\t\t21 DISABLE OSC");
    dp_display_text("\r\n\t\t99 RESTORE PREV SETTINGS");
    dp_display_text("\r\n\t\t00 DISPLAY MAIN MENU\r\n");
}

void display_error(void)
{
	dp_display_text("\r\n--->Error");
}

uint16_t get_number(char mode)
{
	unsigned char j=0;
    unsigned char input_array[16]={0};
    //for (j=0;j<16;j++) input_array[j]=0;
    unsigned char input_char = 0;
    //j=0;

	while(1)
	{
		_BIS_SR(LPM0_bits + GIE); 				//GOTO sleep
	    input_char = UART_read_buffer();
	    if (input_char != 13) input_array[j] = input_char; else
	    {
	    	input_array[j] = '\0';
	    	break;
	    }
	    j++;
	}
	if (mode == DEC)
		return atoi(input_array);
	else
		return bin_to_int(input_array);
}

//Convert char array representing binary number into uint16_t
uint16_t  bin_to_int(unsigned char *array)
{
	uint16_t res = 0;
	uint16_t weight = 1;
	char size=0;
	char index = 0;
	while (array[index++]) size++;
	for (index=size; index>0; index-- )
	{
		if (array[index-1]-48) res += weight;
		weight=weight<<1;
	}
	return res;
}
