
/**
 * \file
 *
 * \brief NVMCTRL Basic driver implementation.
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

/**
 * \defgroup doc_driver_nvmctrl_basic NVMCTRL Basic Driver
 * \ingroup doc_driver_nvmctrl
 *
 * \section doc_driver_nvmctrl_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include "nvmctrl.h"

typedef struct NVMCTRL_descriptor_s {
	nvmctrl_status_t       status;
	uint8_t *              eeprom;
	uint8_t *              data;
	uint8_t                size;
	eeprom_write_done_cb_t cb;
} NVMCTRL_descriptor_t;

static volatile NVMCTRL_descriptor_t NVMCTRL_desc;

ISR(NVMCTRL_EE_vect)
{

	/* The interrupt flag has to be cleared manually */
	NVMCTRL.INTFLAGS = NVMCTRL_EEREADY_bm;

	if (NVMCTRL_desc.size == 0) {
		NVMCTRL_desc.status = NVM_OK;
		NVMCTRL.INTCTRL &= ~NVMCTRL_EEREADY_bm;
		if (NVMCTRL_desc.cb != NULL) {
			NVMCTRL_desc.cb();
		}
	} else {
		while (1) {
			/* Write byte to page buffer */
			*NVMCTRL_desc.eeprom++ = *NVMCTRL_desc.data++;
			NVMCTRL_desc.size--;
			// If we have filled an entire page or written last byte to a partially filled page
			if ((((uintptr_t)NVMCTRL_desc.eeprom % EEPROM_PAGE_SIZE) == 0) || (NVMCTRL_desc.size == 0)) {
				/* Erase written part of page and program with desired value(s) */
				 _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
				break;
			}
		}
	}
}

/**
 * \brief Register a callback function to be called by EEPROM ISR after block write has finished.
 *
 * \param[in] f Pointer to function to be called
 *
 * \return Nothing.
 */
void NVMCTRL_register_callback(eeprom_write_done_cb_t f)
{
	NVMCTRL_desc.cb = f;
}

/**
 * \brief Initialize nvmctrl interface
 * \return Return value 0 if success
 */
int8_t NVMCTRL_init()
{

	// NVMCTRL.CTRLB = 0 << NVMCTRL_APCWP_bp /* Application code write protect: disabled */
	//		 | 0 << NVMCTRL_BOOTLOCK_bp; /* Boot Lock: disabled */

	// NVMCTRL.INTCTRL = 0 << NVMCTRL_EEREADY_bp; /* EEPROM Ready: disabled */

	NVMCTRL_desc.cb = NULL;
	return 0;
}

/**
 * \brief Read a byte from eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to read from
 *
 * \return The read byte
 */
uint8_t NVMCTRL_read_eeprom_byte(eeprom_adr_t eeprom_adr)
{
	// Wait until any write operation has finished
	while (NVMCTRL_desc.status == NVM_BUSY)
		;

	return *(uint8_t *)(EEPROM_START + eeprom_adr);
}

/**
 * \brief Write a byte to eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to write to
 * \param[in] data The byte to write
 *
 * \return Status of write operation
 */
nvmctrl_status_t NVMCTRL_write_eeprom_byte(eeprom_adr_t eeprom_adr, uint8_t data)
{
	return (NVMCTRL_write_eeprom_block(eeprom_adr, &data, 1));
}

/**
 * \brief Read a block from eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to read from
 * \param[in] data Buffer to place read data into
 *
 * \return Nothing
 */
void NVMCTRL_read_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size)
{
	// Wait until any write operation has finished
	while (NVMCTRL_desc.status == NVM_BUSY)
		;

	memcpy(data, (uint8_t *)(EEPROM_START + eeprom_adr), size);
}

/**
 * \brief Write a block to eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to write to
 * \param[in] data The buffer to write
 *
 * \return Status of write operation
 */
nvmctrl_status_t NVMCTRL_write_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size)
{

	// Wait until any write operation has finished
	while (NVMCTRL_desc.status == NVM_BUSY)
		;

	NVMCTRL_desc.eeprom = (uint8_t *)(EEPROM_START + eeprom_adr);
	NVMCTRL_desc.data   = data;
	NVMCTRL_desc.size   = size;
	NVMCTRL_desc.status = NVM_BUSY;

	/* Clear page buffer */
	 _PROTECTED_WRITE(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
	NVMCTRL.INTCTRL |= NVMCTRL_EEREADY_bm;

	return NVM_OK;
}

/**
 * \brief Check if the EEPROM can accept data to be read or written
 *
 * \return The status of EEPROM busy check
 * \retval false The EEPROM can not receive data to be read or written
 * \retval true The EEPROM can receive data to be read or written
 */
bool NVMCTRL_is_eeprom_ready()
{
	return (NVMCTRL_desc.status == NVM_BUSY);
}
