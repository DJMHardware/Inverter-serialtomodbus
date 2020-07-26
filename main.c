/* 
 * File:   main.c
 * Author: sasquachdm
 *
 * Created on March 10, 2020, 10:40 PM
 */

#include <avr/io.h>
#include "hardware.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "nvmctrl.h"
#include "modbus.h"
#include "inverter.h"

volatile uint8_t tickcount=0;

ISR(TCB0_INT_vect) { //this ISR is called 5000 times per second
    TCB0.INTFLAGS = 1 << TCB_CAPT_bp;
	modbus_tick();
    tickcount++;
    if (tickcount >= 5) {
        tickcount = 0;
        INVERTER_tick();
    }
    
}
void tick_interupt_init(void){
    TCB0.CCMP = F_CPU / 5000;
    TCB0.INTCTRL = 1 << TCB_CAPT_bp;
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;
    TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | 1<< TCB_ENABLE_bp;
}

int main(void)
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);
    //USART0_init();
    TCB1.CCMP = 0xffff;
    // TCB1.INTCTRL = 1 << TCB_CAPT_bp;
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    TCB1.CTRLA = TCB_CLKSEL_CLKDIV1_gc | 1<< TCB_ENABLE_bp;
    cli();
    modbus_init();
    tick_interupt_init();
    INVERTER_init();
    sei();
    while (1) 
    {
        //USART0_sendString("Hello World!\r\n");
        modbus_get();
            //    _delay_ms(500);
    }
}


/*
void USART0_init(void);
void USART0_sendChar(char c);
void USART0_sendString(char *str);

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void USART0_init(void)
{
    PORTA.DIR &= ~PIN1_bm;
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    USART0.CTRLB |= USART_TXEN_bm; 
}

void USART0_sendChar(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }        
    USART0.TXDATAL = c;
}

void USART0_sendString(char *str)
{
    for(size_t i = 0; i < strlen(str); i++)
    {
        USART0_sendChar(str[i]);
    }
}

*/
