/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Microsemi Corporation 2014                */
/*  Version 3.1     Release date  November 14, 2014                         */
/*  Modifications done by Arturs orbidans for Master's thesis               */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpuser.c												*/
/*                                                                          */
/*  Description:    user specific file containing JTAG interface functions  */
/*                  and delay function                                      */
/*                                                                          */
/****************************************************************************/

#include "dpuser.h"
//#include "dpalg.h"
//#include "dputil.h"

//Buffer for UART A1
//For firmware paging purposes
#define rx_a1_buffer_size 128u
static char rx_a1_buffer[rx_a1_buffer_size];
static unsigned int rx_a1_buffer_index = 0;
DPUCHAR firmware_version;

//Set JTAG pin direction
void set_jtag_pins(void)
{
	JTAG_PORT_DIR = 0;
	JTAG_PORT_DIR |= (TCK | TMS | TRST | TDI);
    return;
}

//Function to convert long int to string
//http://e2e.ti.com/support/microcontrollers/msp430/f/166/p/291574/1017958
void itoa(long unsigned int value, char* result, int base)
    {
      // check that the base if valid
      if (base < 2 || base > 36) { *result = '\0';}

      char* ptr = result, *ptr1 = result, tmp_char;
      int tmp_value;

      do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      } while ( value );

      // Apply negative sign
      if (tmp_value < 0) *ptr++ = '-';
      *ptr-- = '\0';
      while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
      }

    }

int atoi(unsigned char* p)
{
	int res = 0;
    while(1)
    {
        res = res+(*p - '0');
        p++;
        if (!(*p)) break;
        res = res*10;
    }

    return res;
}

//Init UART A0, default baud 9600	
void uart_a0_init(void)
{
	//Baud: 9600 at 1 Mhz master clock!
    P2SEL1 |= BIT0 | BIT1;			// USCI_A0 UART operation
    P2SEL0 &= ~(BIT0 | BIT1);
    UCA0CTLW0 = UCSWRST;			// Put eUSCI in reset
    UCA0CTLW0 = UCSSEL_2;			//Clock source: master clock (SMCLK)
    UCA0CTLW1 = UCGLIT_3;			//Glitch settings
    UCA0BRW = 52;					//Set baud rate register
    UCA0MCTLW_H = 0x49;
    UCA0MCTLW = UCOS16 | (1<<4);
    UCA0IE = UCRXIE;
    UCA0CTLW0 &= ~UCSWRST;			// Initialize eUSCI
}

//Init UART A1, default baud 9600
void uart_a1_init(void)
{
	//Baud: 9600 at 1 Mhz master clock!
    P2SEL1 |= BIT6 | BIT5;                    // USCI_A1 UART operation
    P2SEL0 &= ~(BIT6 | BIT5);
    UCA1CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA1CTLW0 = UCSSEL_2; //SMCLK
    UCA1CTLW1 = UCGLIT_3;
    UCA1BRW = 6;
    UCA1MCTLW_H = 0x20;
    UCA1MCTLW = UCOS16 | (8<<4);
    UCA1IE = UCRXIE;
    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
}

//Send 1 byte via UART A0
void uart_a0_send_byte(DPCHAR byte)
{
	UCA0TXBUF = byte;
	while (!(UCA0IFG & UCTXCPTIFG)) {};
	UCA0IFG ^= UCTXCPTIFG;
}

//Send 1 byte via UART A1
void uart_a1_send_byte(unsigned char byte)
{
	UCA1TXBUF = byte;
	while (!(UCA1IFG & UCTXCPTIFG)) {};
	UCA1IFG ^= UCTXCPTIFG;
}

/*
 * User attention:
 * jtag_port_reg: 	8 bit Static variable to keep track of the state of all the JTAG pins 
 * 					at all times during the programming operation.
 * Note: User check the variable size to make sure it can cover the hardware IO register. 
 * 
*/
static DPUCHAR jtag_port_reg;
/*
 * User attention: 
 * Module: jtag_inp
 * 		purpose: report the logic state of tdo jtag pin
 * Arguments: None
 * Return value: 8 bit value
 * 		0, 0x80
 * 
*/
DPUCHAR jtag_inp(void)
{	
	if (JTAG_PORT_IN & TDO) return 0x80;
	return 0;
}

/*
 * User attention: 
 * Module: jtag_outp
 * 		purpose: Set the JTAG port (all JTAG pins)
 * Arguments: 8 bit value containing the new state of all the JTAG pins
 * Return value: None
 * 
*/
void jtag_outp(DPUCHAR outdata)
{
	JTAG_PORT_OUT = (outdata);
}

/*
 * No need to change this function
 * Module: dp_jtag_init
 * 		purpose: Set tck and trstb pins to logic level one
 * Arguments:
 * 		None
 * Return value: None
 * 
*/
void dp_jtag_init(void)
{
	jtag_port_reg = TCK | TRST; 
	jtag_outp(jtag_port_reg);
}

/*
 * No need to change this function
 * Module: dp_jtag_tms
 * 		purpose: Set tms pin to a logic level one or zero and pulse tck.
 * Arguments: 
 * 		tms: 8 bit value containing the new state of tms
 * Return value: None
 * Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
 * 				to modify the required jtag pins and preseve the reset.
 * 
*/
void dp_jtag_tms(DPUCHAR tms)		 
{	
	jtag_port_reg &= ~(TMS | TCK);
    jtag_port_reg |= (tms ? TMS : 0u);
	jtag_outp(jtag_port_reg);
	jtag_port_reg |= TCK;
	jtag_outp(jtag_port_reg);
}

/*
 * No need to change this function
 * Module: dp_jtag_tms_tdi
 * 		purpose: Set tms amd tdi pins to a logic level one or zero and pulse tck.
 * Arguments: 
 * 		tms: 8 bit value containing the new state of tms
 * 		tdi: 8 bit value containing the new state of tdi
 * Return value: None
 * Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
 * 				to modify the required jtag pins and preseve the reset.
 * 
*/
void dp_jtag_tms_tdi(DPUCHAR tms, DPUCHAR tdi)
{
	jtag_port_reg &= ~(TMS | TCK | TDI);
    jtag_port_reg |= ((tms ? TMS : 0u) | (tdi ? TDI : 0u));
	jtag_outp(jtag_port_reg);
	jtag_port_reg |= TCK;
	jtag_outp(jtag_port_reg);
}

/*
 * No need to change this function
 * Module: dp_jtag_tms_tdi_tdo
 * 		purpose: Set tms amd tdi pins to a logic level one or zero, 
 * 				 pulse tck and return tdi level
 * Arguments: 
 * 		tms: 8 bit value containing the new state of tms
 * 		tdi: 8 bit value containing the new state of tdi
 * Return value: 
 * 		ret: 8 bit variable ontaining the state of tdo.
 * Valid return values: 
 * 		0x80: indicating a logic level high on tdo
 * 		0: indicating a logic level zero on tdo
 * Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
 * 				to modify the required jtag pins and preseve the reset.
 * 
*/
DPUCHAR dp_jtag_tms_tdi_tdo(DPUCHAR tms, DPUCHAR tdi)
{
    DPUCHAR ret = 0x80u;
	jtag_port_reg &= ~(TMS | TCK | TDI);
    jtag_port_reg |= ((tms ? TMS : 0u) | (tdi ? TDI : 0u));
	jtag_outp(jtag_port_reg);
	ret = jtag_inp() ;
	jtag_port_reg |= TCK;
	jtag_outp(jtag_port_reg);
	return ret;
}

/*
 * User attention: 
 * Module: dp_delay
 * 		purpose: Execute a time delay for a specified amount of time.
 * Arguments: 
 * 		microseconeds: 32 bit value containing the amount of wait time in microseconds.
  * Return value: None
 * 
*/
void dp_delay(DPULONG microseconds)
{
	//dp_display_value(microseconds,DEC);
    //TA0CCR0 = microseconds;
    //TA0CTL = TASSEL_2 + MC_1+ID_0+TAIFG;               // SMCLK

    //while (!(TA0CCTL0 & CCIFG)) {};

    //TA0CTL = 0;
    //TA0CCTL0 = 0;

    volatile DPULONG i;
    for(i=0;i<microseconds;i++)
    {
    }
}

#ifdef ENABLE_DISPLAY

void dp_display_text(DPCHAR *text)
{
	while (*text)
	{
		uart_a0_send_byte(*text++);
	};
}

void dp_display_value(DPULONG value,DPUINT descriptive)
{
    char value_text[256];
    if (descriptive == HEX)
    {
		itoa( value, value_text, 16);
    }
    if (descriptive == BIN)
    {
		itoa( value, value_text, 2);
    }
    else
    {
        itoa(value, value_text, 10);
    }

    dp_display_text (value_text);
}


void dp_display_array(DPUCHAR *value,DPUINT bytes, DPUINT descriptive)
{
    int idx;
    for (idx=bytes;idx>0;idx--)
    {
    	dp_display_value(*value++,DEC);
        dp_display_text(" ");
    }
}

void dp_display_value_a1(long value,int descriptive)
{
    char value_text[256];
    if (descriptive == HEX)
    {
            itoa( value, value_text, 16);
    }
    else
    {
            itoa(value, value_text, 10);
    }

    dp_display_text_a1 (value_text);
}

void dp_display_text_a1(char *text)
{
	while (*text)
	{
		uart_a1_send_byte(*text++);
	};
}

void uart_a1_read_file(long address, unsigned char* rx_buffer, long size)
{
	dp_display_value_a1(address,DEC);
	uart_a1_send_byte(32);
	dp_display_value_a1(size,DEC);
	uart_a1_send_byte(13);
	unsigned int i = 0;
	for (i=0; i<50; i++)
	{
	dp_delay(85000);
	};
	for (i=0; i<size; i++)
	{
		rx_buffer[i] = rx_a1_buffer[i];
		//dp_display_text("\r\nRX: ");
		//dp_display_value(rx_a1_buffer[i], HEX);
	}
	rx_a1_buffer_index = 0;
	//for (i=0; i<size; i++)rx_a1_buffer[i]=0;
}

void uart_a1_rx_int_handle(unsigned char in_value)
{
	if (rx_a1_buffer_index+1==rx_a1_buffer_size)
	{
		rx_a1_buffer_index = 0;
	}
	rx_a1_buffer[rx_a1_buffer_index++] = in_value;
}

//Interrupt vector for UART A1 data RX
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
	  {
	    case USCI_NONE: break;
	    case USCI_UART_UCRXIFG:
	      uart_a1_rx_int_handle(UCA1RXBUF);
	      break;
	    case USCI_UART_UCTXIFG: break;
	    case USCI_UART_UCSTTIFG: break;
	    case USCI_UART_UCTXCPTIFG: break;
	  }
}

#endif
