/**
 * \file
 *
 * \brief NVMCTRL Basic driver declaration.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#ifndef NVMCTRL_BASIC_H_INCLUDED
#define NVMCTRL_BASIC_H_INCLUDED

// #include <compiler.h>
#include <stddef.h>

#define BOOTLOADER_SECTION __attribute__((section(".bootloader")))

#ifdef __cplusplus
extern "C" {
#endif
typedef uint8_t bool;
/** Datatype for flash address */
typedef uint16_t flash_adr_t;

/** Datatype for EEPROM address */
typedef uint16_t eeprom_adr_t;

/** Datatype for return status of NVMCTRL operations */
typedef enum {
	NVM_OK    = 0, ///< NVMCTRL free, no write ongoing.
	NVM_ERROR = 1, ///< NVMCTRL operation retsulted in error
	NVM_BUSY  = 2, ///< NVMCTRL busy, write ongoing.
} nvmctrl_status_t;

typedef void (*eeprom_write_done_cb_t)(void);

void NVMCTRL_register_callback(eeprom_write_done_cb_t f);

int8_t NVMCTRL_init();

uint8_t NVMCTRL_read_eeprom_byte(eeprom_adr_t eeprom_adr);

nvmctrl_status_t NVMCTRL_write_eeprom_byte(eeprom_adr_t eeprom_adr, uint8_t data);

void NVMCTRL_read_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size);

nvmctrl_status_t NVMCTRL_write_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size);

bool NVMCTRL_is_eeprom_ready();

#ifdef __cplusplus
}
#endif

#endif /* NVMCTRL_BASIC_H_INCLUDED */