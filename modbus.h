/* 
 * File:   modbus.h
 * Author: sasquachdm
 *
 * Created on June 22, 2020, 3:58 PM
 */

#ifndef MODBUS_H
#define	MODBUS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define USART_NUMBER USART0
#define mbsBAUDRATE 38400L

extern void modbus_init(void);
extern void modbus_tick(void);
extern void modbus_get(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_H */

