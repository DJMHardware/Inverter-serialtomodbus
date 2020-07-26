/* 
 * File:   hardware.h
 * Author: sasquachdm
 *
 * Created on June 25, 2020, 4:43 PM
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef F_CPU
#define F_CPU 10000000UL
#endif

// #define HARDWARE_EEPROM //comment this line if no eeprom
// #define HARDWARE_FUSES //comment this line if no FUSES
    
#ifdef HARDWARE_EEPROM
__attribute__((section(".eeprom"))) 
uint8_t eepContent[14] = {
    8,
    5,
    20,
    144,
    80,
    30,
    32,
    0,
    0,
    200,
    155,
    132,
    12,
    20
};
#endif
// ATmega4809 Configuration Bit Settings

// 'C' source line config statements
#ifdef HARDWARE_FUSES   
FUSES = {
	0x00, // WDTCFG{PERIOD=OFF, WINDOW=OFF}
	0x00, // BODCFG{SLEEP=DIS, ACTIVE=DIS, SAMPFREQ=1KHZ, LVL=BODLEVEL0}
	0x7E, // OSCCFG{FREQSEL=20MHZ, OSCLOCK=CLEAR}
	0x00, // Reserved
	0x00, // Reserved
	0xF7, // SYSCFG0{EESAVE=SET, RSTPINCFG=GPIO, CRCSRC=NOCRC}
	0xFF, // SYSCFG1{SUT=64MS}
	0xFF, // APPEND
	0xFF, // BOOTEND
};

LOCKBITS = {
	0xC5, // LOCKBIT{LB=NOLOCK}
};
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

