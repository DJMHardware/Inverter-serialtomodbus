/*
 * File:   modbus.h
 * Author: sasquachdm
 *
 * Created on June 22, 2020, 3:58 PM
 */

#ifndef MODBUS_H
#define	MODBUS_H

#include "inverter.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define USART_NUMBER USART0
#define mbsBAUDRATE 38400L
#define mbsSTART_ADDRESS 1
#define mbsHOLDING_SIZE (sizeof(inverter_mbs_holding_t)/2)
#define mbsINPUT_SIZE (sizeof(inverter_mbs_input_t)/2)
#define mbsCOIL_SIZE (sizeof(inverter_mbs_coil_t)*8)
#define mbsINPUT_STATUS_SIZE (sizeof(inverter_mbs_input_s_t)*8)
extern void modbus_init(void);
extern void modbus_tick(void);
extern void modbus_get(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_H */
