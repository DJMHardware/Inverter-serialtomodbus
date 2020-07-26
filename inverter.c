#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include "inverter.h"
#include "modbus.h"
#include "nvmctrl.h"

volatile uint8_t Inverter_RX_Error;
volatile inverter_data_t InverterData;
volatile eeprom_data_t EEpromData;

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
        modbusData.Holding.Name.MINUTES++;
        if (modbusData.Holding.Name.MINUTES >= 60)
        {
           modbusData.Holding.Name.MINUTES = 0;
           modbusData.Holding.Name.HOURS++;
            if (modbusData.Holding.Name.HOURS >= 24)
            {
                modbusData.Holding.Name.HOURS = 0;
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
    if (((InverterData.Serial_state==RXStandby) | (InverterData.Serial_state==RXActive))){
        current_time = InverterData.Tick_count-InverterData.Last_bytetime;
        InverterData.Last_bytetime = InverterData.Tick_count;
        if (current_time > 1)
        {
            if (InverterData.RX_Array_index>0)
            {
                modbusData.InputStatus.Name.RX_Timeout_between_bytes=1;
                InverterData.RX_Array_index=0;
                InverterData.Serial_state=RXStandby;
                InverterData.Last_bytetime=0;
                return;
            }
            InverterData.RX_Array_index=0;
            InverterData.Serial_state=RXActive;
        }
        InverterData.RX.Array[InverterData.RX_Array_index] = USART1.RXDATAL;
        InverterData.RX_Array_index++;
        if (InverterData.RX_Array_index >= 20)  
        {
            InverterData.RX_Array_index=0;
            InverterData.Serial_state=RXComplete;
            InverterData.Last_bytetime=0;
        }            
    }else{
        current_time = (uint16_t) USART1.RXDATAL;
        modbusData.InputStatus.Name.RX_Extra_byte = 1;
    }
}

/* Interrupt service routine for Data Register Empty */
ISR(USART1_DRE_vect)
{
    InverterData.TX_Array_index++;
    if (InverterData.TX_Array_index < 16){
        USART1.TXDATAL = InverterData.TX.Array[InverterData.TX_Array_index];
    }
    else {
        USART1.CTRLA &= ~(1 << USART_DREIE_bp);
        InverterData.Serial_state=TXComplete;
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
   InverterData.Serial_state = RXStandby;
   NVMCTRL_init();
   INVERTER_load_eeprom();
    USART_1_init();
    RTC_0_init();
}

void INVERTER_RX_copy_data(void){
    modbusData.Input.Name.INVERTER_STATUS = InverterData.RX.Name.INVERTER_STATUS;
    modbusData.Input.Name.INVERTER_FAULT = InverterData.RX.Name.INVERTER_FAULT;
    modbusData.Input.Name.DC_VOLTS = (InverterData.RX.Name.DC_VOLTS.high << 8) | InverterData.RX.Name.DC_VOLTS.low;
    modbusData.Input.Name.DC_AMPS = (InverterData.RX.Name.DC_AMPS.high << 8) | InverterData.RX.Name.DC_AMPS.low;
    modbusData.Input.Name.AC_VOLTS_OUTPUT = InverterData.RX.Name.AC_VOLTS_OUTPUT;
    modbusData.Input.Name.AC_VOLTS_INPUT = InverterData.RX.Name.AC_VOLTS_INPUT;
    modbusData.Input.Name.INVERTER_LED = InverterData.RX.Name.INVERTER_LED;
    modbusData.Input.Name.CHARGER_LED = InverterData.RX.Name.CHARGER_LED;
    modbusData.Input.Name.INVERTER_REVISION = InverterData.RX.Name.INVERTER_REVISION;
    modbusData.Input.Name.BATTERY_TEMP = InverterData.RX.Name.BATTERY_TEMP;
    modbusData.Input.Name.TRANSFORMER_TEMP = InverterData.RX.Name.TRANSFORMER_TEMP;
    modbusData.Input.Name.FET_TEMP = InverterData.RX.Name.FET_TEMP;
    modbusData.Input.Name.INVERTER_MODEL = InverterData.RX.Name.INVERTER_MODEL;
    modbusData.Input.Name.STACK_MODE = InverterData.RX.Name.STACK_MODE;
    modbusData.Input.Name.AC_INPUT_AMPS = InverterData.RX.Name.AC_INPUT_AMPS;
    modbusData.Input.Name.AC_OUTPUT_AMPS = InverterData.RX.Name.AC_OUTPUT_AMPS;
    modbusData.Input.Name.AC_HZ = (InverterData.RX.Name.AC_HZ.high << 8) | InverterData.RX.Name.AC_HZ.low;
    modbusData.Input.Name.STACK_MODE=InverterData.LED.Charger.long_cnt;
 }

void INVERTER_TX_copy_data(void){
    if (InverterData.TX.Name.INVERTER_ON_OFF > 0){
        modbusData.Holding.Name.INVERTER_ON_OFF = 0;
    }
    for (uint8_t c=0; c < 14;c++){
        InverterData.TX.Array[c] = modbusData.Holding.Registers[c];
    }
    if (modbusData.Coil.Name.OnOff.SearchMode == 0){
        InverterData.TX.Name.SEARCH_WATTS = 0;
    }
    if (modbusData.Coil.Name.OnOff.ACInput == 0){
        InverterData.TX.Name.VAC_CUT_OUT_VOLTAGE = 255;
    }
    
    InverterData.TX.Name.FORCE_CHARGE &= 0x0f;
    InverterData.TX.Name.FORCE_CHARGE |= (0xf0 & modbusData.Coil.Array[0]);
            
    InverterData.TX.Name.HOURS = modbusData.Holding.Name.HOURS;
    InverterData.TX.Name.MINUTES = modbusData.Holding.Name.MINUTES;
}

void INVERTER_TestLED(uint8_t raw_led, volatile inverter_LED_state_t *test, uint8_t blink_cnt, uint8_t solid_cnt){
    uint8_t cnt = (uint8_t) test->cnt;
    if (raw_led != test->last){
        test->last = raw_led;    
        cnt = 0;
    }else{
        cnt++;
    }    
    if (cnt > blink_cnt && cnt <= solid_cnt) {
        test->blink = 1;
        test->OnOff = 0;
    }
    if (cnt > solid_cnt) {
        test->OnOff = raw_led;
        test->blink = 0;
        cnt = solid_cnt;
    }
   test->cnt = cnt;
}

void INVERTER_RX_TestData(void){
    INVERTER_TestLED((InverterData.RX.Name.INVERTER_LED > 0), &InverterData.LED.Inverter, 3, 9);
    modbusData.InputStatus.Name.Inverter_On_Off = InverterData.LED.Inverter.OnOff | InverterData.LED.Inverter.blink;
    modbusData.InputStatus.Name.Inverter_led_solid = InverterData.LED.Inverter.OnOff;
    modbusData.InputStatus.Name.Inverter_led_blink = InverterData.LED.Inverter.blink;

    INVERTER_TestLED((InverterData.RX.Name.CHARGER_LED > 0), &InverterData.LED.Charger, 0, 3);
    InverterData.LED.Charger.test = InverterData.LED.Charger.blink & (modbusData.Input.Name.INVERTER_STATUS > 0);
    modbusData.InputStatus.Name.Charger_On_Off = InverterData.LED.Charger.OnOff | InverterData.LED.Charger.test;
    modbusData.InputStatus.Name.Charger_led_solid = InverterData.LED.Charger.OnOff;
    modbusData.InputStatus.Name.Charger_led_blink = InverterData.LED.Charger.blink;
    modbusData.InputStatus.Name.AC_in_valid = InverterData.LED.Charger.OnOff | InverterData.LED.Charger.blink;
}

void INVERTER_RX_ComputeResponse(void){
    if (modbusData.Coil.Name.OnOff.Inverter != modbusData.InputStatus.Name.Inverter_On_Off){
        InverterData.LED.Inverter.long_cnt++;
        if ((InverterData.LED.Inverter.long_cnt > 20) && !modbusData.InputStatus.Name.Inverter_state_error){
                InverterData.LED.Inverter.long_cnt=0;
            if (InverterData.LED.Inverter.attempts < 3){
                InverterData.LED.Inverter.attempts++;
                modbusData.Holding.Name.INVERTER_ON_OFF=1;
            }else{
                modbusData.InputStatus.Name.Inverter_state_error=1;
            }
        }
    } else {
        InverterData.LED.Inverter.long_cnt=20;
        InverterData.LED.Inverter.attempts=0;
    }
    if (modbusData.InputStatus.Name.AC_in_valid && (modbusData.Coil.Name.OnOff.Charger != modbusData.InputStatus.Name.Charger_On_Off)){
        InverterData.LED.Charger.long_cnt++;
        if ((InverterData.LED.Charger.long_cnt > 100) && !modbusData.InputStatus.Name.Charger_state_error){
                InverterData.LED.Charger.long_cnt=0;
            if (InverterData.LED.Charger.attempts < 3){
                InverterData.LED.Charger.attempts++;
                modbusData.Holding.Name.INVERTER_ON_OFF=2;
            }else{
                modbusData.InputStatus.Name.Charger_state_error=1;
            }
        }
    } else {
        InverterData.LED.Charger.long_cnt=100;
        InverterData.LED.Charger.attempts=0;
    }
    if (modbusData.Coil.Name.WriteEEprom == 1 ) INVERTER_store_eeprom();
}

void INVERTER_load_eeprom(void){
    NVMCTRL_read_eeprom_block(0, (uint8_t*)&EEpromData, sizeof(EEpromData));
    for (uint8_t c=1; c < 14;c++){
        modbusData.Holding.Registers[c] = EEpromData.Array[c];
    }
    modbusData.Coil.Name.OnOff.Inverter = EEpromData.Name.INVERTER_ON_OFF;

}

void INVERTER_store_eeprom(void){
    for (uint8_t c=1; c < 14;c++){
         EEpromData.Array[c] = modbusData.Holding.Registers[c];
    }
    EEpromData.Coil = modbusData.Coil.Name.OnOff;
    //NVMCTRL_write_eeprom_block(0, (uint8_t*)&EEpromData, sizeof(EEpromData));
    while ((NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)==1)
        ;
    cli();
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
    while ((NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)==1)
        ;
    for (uint8_t c=0; c < sizeof(EEpromData.Array);c++){
         *(uint8_t *)(EEPROM_START+c) = EEpromData.Array[c];
    }
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
    sei();
    modbusData.Coil.Name.WriteEEprom = 0;
}

void INVERTER_tick(void){
    InverterData.Tick_count++;
    if (InverterData.Tick_count > 220){
        InverterData.Tick_count = 0;
        modbusData.InputStatus.Name.RX_Timeout_between_packets = 1;
    }
    
    switch (InverterData.Serial_state){
        case RXComplete: {
            InverterData.Serial_state=TXStandby;
            InverterData.TX_Wait_count++;
            INVERTER_TX_copy_data();
        } break;

        case TXStandby: {
            InverterData.TX_Wait_count++;
            if (InverterData.TX_Wait_count >= 10) {
                InverterData.TX_Wait_count=0;
                InverterData.Serial_state=TXActive;
                InverterData.TX_Array_index=0;
                USART1.TXDATAL = InverterData.TX.Array[0];
            
                USART1.CTRLA |= 1 << USART_DREIE_bp;
                
            }
        } break;
        
        case TXComplete: {
            InverterData.Tick_count = 0;
            INVERTER_RX_copy_data();
            InverterData.Serial_state=TestData;
        } break;

        case TestData: {
            INVERTER_RX_TestData();
            InverterData.Serial_state=ComputeResponse;
        } break;

        case ComputeResponse: {
            INVERTER_RX_ComputeResponse();
            InverterData.Serial_state=RXStandby;
            modbusData.InputStatus.Name.RX_Extra_byte = 0;
            modbusData.InputStatus.Name.RX_Timeout_between_bytes = 0;
            modbusData.InputStatus.Name.RX_Timeout_between_packets = 0;
        } break;

        default: {
        } break;
    }
}
