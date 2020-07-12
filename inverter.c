#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include "inverter.h"

volatile uint16_t Inverter_last_bytetime;
volatile uint16_t Inverter_Tick_count=0;
volatile uint8_t Inverter_Com_count=0;
volatile uint8_t Inverter_RX_Error;
volatile inverter_raw_t Inverter;
volatile inverter_com_stat_t Inverter_Com_stat = RXStandby;

ISR(RTC_CNT_vect)
{
    if ((RTC.INTFLAGS & RTC_CMP_bm) >0)
    {
        RTC.INTFLAGS = RTC_CMP_bm;
        RTC.INTCTRL &= ~(1 << RTC_CMP_bp);
    /* test removed
        Inverter_TX_index=0;
        USART1.TXDATAL = Inverter_TX[0];
    */
        

    } 
   
    if ((RTC.INTFLAGS & RTC_OVF_bm) >0)
    {
        /* Insert your RTC Overflow interrupt handling code */

        //Inverter_TX[15]++;
        Inverter.TX.Name.MINUTES++;
        if (Inverter.TX.Name.MINUTES >= 60)
        {
            Inverter.TX.Name.MINUTES = 0;
            Inverter.TX.Name.HOURS++;
            if (Inverter.TX.Name.HOURS >= 24)
            {
                Inverter.TX.Name.HOURS = 0;
            }
        }
        /* Overflow interrupt flag has to be cleared manually */
        RTC.INTFLAGS = RTC_OVF_bm;
    }
}

/* Interrupt service routine for RX complete */
ISR(USART1_RXC_vect)
{
    uint16_t current_time;
    current_time = Inverter_Tick_count;
    if (current_time >= Inverter_last_bytetime)
    {
        current_time -= Inverter_last_bytetime;
    }
    else
    {
        current_time += (0xFFFF-Inverter_last_bytetime);
    }
    Inverter_last_bytetime = Inverter_Tick_count;
    if (current_time > 1)
    {
        if (Inverter.RX_Array_index>0)
        {
            Inverter_RX_Error=1;
        }
        Inverter.RX_Array_index=0;
        Inverter_Com_stat=RXActive;
    }
    Inverter.RX.Array[Inverter.RX_Array_index] = USART1.RXDATAL;
    Inverter.RX_Array_index++;
    if (Inverter.RX_Array_index >= 20)  
    {
        Inverter.RX_Array_index=0;
        Inverter_Com_stat=RXComplete;
    }            
}

/* Interrupt service routine for Data Register Empty */
ISR(USART1_DRE_vect)
{
    Inverter.TX_Array_index++;
    if (Inverter.TX_Array_index < 20){
        USART1.TXDATAL = Inverter.TX.Array[Inverter.TX_Array_index];
    }
    else {
        USART1.CTRLA &= ~(1 << USART_DREIE_bp);
        Inverter_Com_stat=TXComplete;
    }
    
}

/**
 * \brief Initialize rtc interface
 * \return Initialization status.
 */
void RTC_0_init(void)
{

	while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */
	}

	// RTC.CMP = 0x0; /* Compare: 0x0 */

	// RTC.CNT = 0x0; /* Counter: 0x0 */

	RTC.CTRLA = RTC_PRESCALER_DIV32_gc  /* 32 */
	            | 1 << RTC_RTCEN_bp     /* Enable: enabled */
	            | 1 << RTC_RUNSTDBY_bp; /* Run In Standby: enabled */

	RTC.PER = 0xefff; /* Period: 0xefff */

	// RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; /* 32KHz Internal Ultra Low Power Oscillator (OSCULP32K) */

	RTC.DBGCTRL = 1 << RTC_DBGRUN_bp; /* Run in debug: enabled */

	RTC.INTCTRL = 0 << RTC_CMP_bp    /* Compare Match Interrupt enable: disabled */
	              | 1 << RTC_OVF_bp; /* Overflow Interrupt enable: enabled */

	// RTC.PITCTRLA = RTC_PERIOD_OFF_gc /* Off */
	//		 | 0 << RTC_PITEN_bp; /* Enable: disabled */

	// RTC.PITDBGCTRL = 0 << RTC_DBGRUN_bp; /* Run in debug: disabled */

	// RTC.PITINTCTRL = 0 << RTC_PI_bp; /* Periodic Interrupt: disabled */

}

/**
 * \brief Initialize USART interface
 * If module is configured to disabled state, the clock to the USART is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 * \retval 0 the USART init was successful
 * \retval 1 the USART init was not successful
 */
void USART_1_init(void)
{
    PORTC.DIRSET = 1 << PORT0;
    PORTC.OUTSET = 0 << PORT0;
    PORTC.DIRSET = 1 << PORT3;
    PORTC.OUTSET = 0 << PORT3;

	USART1.BAUD = (uint16_t)USART1_BAUD_RATE(19200); /* set baud rate register */

	USART1.CTRLA = 0 << USART_ABEIE_bp    /* Auto-baud Error Interrupt Enable: disabled */
	               | 0 << USART_DREIE_bp  /* Data Register Empty Interrupt Enable: disabled */
	               | 0 << USART_LBME_bp   /* Loop-back Mode Enable: disabled */
	               | USART_RS485_EXT_gc  /* RS485 Mode disabled */
	               | 1 << USART_RXCIE_bp  /* Receive Complete Interrupt Enable: enabled */
	               | 0 << USART_RXSIE_bp  /* Receiver Start Frame Interrupt Enable: disabled */
	               | 0 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */

	USART1.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
	               | 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
	               | 1 << USART_RXEN_bp     /* Reciever enable: enabled */
	               | USART_RXMODE_NORMAL_gc /* Normal mode */
	               | 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
	               | 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */

	// USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
	//		 | USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
	//		 | USART_PMODE_DISABLED_gc /* No Parity */
	//		 | USART_SBMODE_1BIT_gc; /* 1 stop bit */

	// USART1.DBGCTRL = 0 << USART_DBGRUN_bp; /* Debug Run: disabled */

	// USART1.EVCTRL = 0 << USART_IREI_bp; /* IrDA Event Input Enable: disabled */

	// USART1.RXPLCTRL = 0x0 << USART_RXPL_gp; /* Receiver Pulse Length: 0x0 */

	// USART1.TXPLCTRL = 0x0 << USART_TXPL_gp; /* Transmit pulse length: 0x0 */

}

/**
 * \brief Enable RX and TX in USART_0
 * 1. If supported by the clock system, enables the clock to the USART
 * 2. Enables the USART module by setting the RX and TX enable-bits in the USART control register
 *
 * \return Nothing
 */
void USART_1_enable(void)
{
	USART1.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

/**
 * \brief Enable RX in USART_0
 * 1. If supported by the clock system, enables the clock to the USART
 * 2. Enables the USART module by setting the RX enable-bit in the USART control register
 *
 * \return Nothing
 */
void USART_1_enable_rx(void)
{
	USART1.CTRLB |= USART_RXEN_bm;
}

/**
 * \brief Enable TX in USART_0
 * 1. If supported by the clock system, enables the clock to the USART
 * 2. Enables the USART module by setting the TX enable-bit in the USART control register
 *
 * \return Nothing
 */
void USART_1_enable_tx(void)
{
	USART1.CTRLB |= USART_TXEN_bm;
}

/**
 * \brief Disable USART_0
 * 1. Disables the USART module by clearing the enable-bit(s) in the USART control register
 * 2. If supported by the clock system, disables the clock to the USART
 *
 * \return Nothing
 */
void USART_1_disable(void)
{
	USART1.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
}

/**
 * \brief Get recieved data from USART_0
 *
 * \return Data register from USART_0 module
 */
uint8_t USART_1_get_data(void)
{
	return USART1.RXDATAL;
}

void INVERTER_init(void){
    USART_1_init();
    RTC_0_init();
}

void INVERTER_tick(void){
    Inverter_Tick_count++;
    
    switch (Inverter_Com_stat){
        case RXComplete: {
            Inverter_Com_count++;
            if (Inverter_Com_count >= 10) {
                Inverter_Com_count=0;
                Inverter_Com_stat=TXActive;
                Inverter.TX_Array_index=0;
                USART1.TXDATAL = Inverter.TX.Array[0];
            
                USART1.CTRLA |= 1 << USART_DREIE_bp;
            }
        } break;
        
        case TXComplete: {
            Inverter_Com_stat=RXStandby;
            
        } break;

        default: {
        } break;
    }
}
