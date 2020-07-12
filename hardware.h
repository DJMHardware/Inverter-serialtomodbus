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
// ATmega4809 Configuration Bit Settings

// 'C' source line config statements

FUSES = {
	0x00, // WDTCFG{PERIOD=OFF, WINDOW=OFF}
	0x00, // BODCFG{SLEEP=DIS, ACTIVE=DIS, SAMPFREQ=1KHZ, LVL=BODLEVEL0}
	0x7E, // OSCCFG{FREQSEL=20MHZ, OSCLOCK=CLEAR}
	0x00, // Reserved
	0x00, // Reserved
	0xF6, // SYSCFG0{EESAVE=CLEAR, RSTPINCFG=GPIO, CRCSRC=NOCRC}
	0xFF, // SYSCFG1{SUT=64MS}
	0xFF, // APPEND
	0xFF, // BOOTEND
};

LOCKBITS = {
	0xC5, // LOCKBIT{LB=NOLOCK}
};

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

