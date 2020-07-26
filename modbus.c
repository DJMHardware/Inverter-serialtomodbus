/*
 *  Created: 04.02.2016
 *  Author: Max Brueggemann
 */ 

/*
*	An example project implementing a simple modbus slave device using an
*	ATmega88PA running at 20MHz.
*	Baudrate: 38400, 8 data bits, 1 stop bit, no parity
*	Your busmaster can read/write the following data:
*	coils: 0 to 7
*	discrete inputs: 0 to 7
*	input registers: 0 to 3
*	holding registers: 0 to 3
*/

#define clientAddress 0x01

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "modbus.h"
#include "yaMBSiavr.h"
#include "inverter.h"
#include "nvmctrl.h"


volatile uint8_t instate = 0;
volatile uint8_t outstate = 0;
volatile uint8_t ins = 0;
volatile uint8_t outs = 0;
volatile modbus_data_t modbusData;

/*
*   Modify the following 3 functions to implement your own pin configurations...
*/

/*
void SetOuts(volatile uint8_t in) {
	PORTD|= (((in & (1<<3))<<4) | ((in & (1<<4))<<1) | ((in & (1<<5))<<1));
	PORTB|= (((in & (1<<0))<<2) | ((in & (1<<1))) | ((in & (1<<2))>>2));
	in=~in;
	PORTB&= ~(((in & (1<<0))<<2) | ((in & (1<<1))) | ((in & (1<<2))>>2));
	PORTD&= ~(((in & (1<<3))<<4) | ((in & (1<<4))<<1) | ((in & (1<<5))<<1));
}

uint8_t ReadIns(void) {
	uint8_t ins=0x00;
	ins|=(PINC&((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)));
	ins|=(((PIND&(1<<4))<<2)|((PIND&(1<<3))<<4));
	return ins;
}

void io_conf(void) { 
	/*
	 Outputs: PB2,PB1,PB0,PD7,PD5,PD6
	 Inputs: PC0, PC1, PC2, PC3, PC4, PC6, PD4, PD3
	*/
/*
    DDRD=0x00;
	DDRB=0x00;
	DDRC=0x00;
	PORTD=0x00;
	PORTB=0x00;
	PORTC=0x00;
	PORTD|=(1<<0);
	DDRD |= (1<<2)|(1<<5)|(1<<6)|(1<<7);
	DDRB |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
}
*/

void modbus_get(void) {
	if (modbusGetBusState() & (1<<ReceiveCompleted))
	{
		switch(rxbuffer[1]) {
			case fcReadCoilStatus: {
				modbusExchangeBits(modbusData.Coil.Array,mbsSTART_ADDRESS,mbsCOIL_SIZE);
			}
			break;
			
			case fcReadInputStatus: {
				//volatile uint8_t inps = ReadIns();
				modbusExchangeBits(modbusData.InputStatus.Array,mbsSTART_ADDRESS,mbsINPUT_STATUS_SIZE);
			}
			break;
			
			case fcReadHoldingRegisters: {
				modbusExchangeRegisters(modbusData.Holding.Registers,mbsSTART_ADDRESS,mbsHOLDING_SIZE);
			}
			break;
			
			case fcReadInputRegisters: {
				modbusExchangeRegisters(modbusData.Input.Registers,mbsSTART_ADDRESS,mbsINPUT_SIZE);
			}
			break;
			
			case fcForceSingleCoil: {
				modbusExchangeBits(modbusData.Coil.Array,mbsSTART_ADDRESS,mbsCOIL_SIZE);
				//SetOuts(outstate);
			}
			break;
			
			case fcPresetSingleRegister: {
				modbusExchangeRegisters(modbusData.Holding.Registers,mbsSTART_ADDRESS,mbsHOLDING_SIZE);
			}
			break;
			
			case fcForceMultipleCoils: {
				modbusExchangeBits(modbusData.Coil.Array,mbsSTART_ADDRESS,mbsCOIL_SIZE);
				//SetOuts(outstate);
			}
			break;
			
			case fcPresetMultipleRegisters: {
				modbusExchangeRegisters(modbusData.Holding.Registers,mbsSTART_ADDRESS,mbsHOLDING_SIZE);
			}
			break;
			
			default: {
				modbusSendException(ecIllegalFunction);
			}
			break;
		}
	}
}

void modbus_init(void)
{
	//io_conf();
	modbusSetAddress(clientAddress);
	modbusInit();
}

void modbus_tick(void)
{
	    modbusTickTimer();
}

