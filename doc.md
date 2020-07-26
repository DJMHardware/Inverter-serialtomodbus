# ModBus definitions
## Coils
## Input Registers
| ModBus Input Register | Inverter RX byte| Name| Range | Scaling | Units/notes|
|-------:|--------:|:--------------:|:--------:|--------:|:-------         |
|     1  |     0  |INVERTER_STATUS  | | | [Inverter status](#inverter-status)|
|     2  |     1  |INVERTER_FAULT   | | |  |
|     3  |   2,3  |DC_VOLTS         |   0-100.0  |  0.1 | Volts
|     4  |   4,5  |DC_AMPS          |   0-500    |   1  | Amps
|     5  |     6  |AC_VOLTS_OUTPUT  |   0-150    |   1  | Volts ac RMS
|     6  |     7  |AC_VOLTS_INPUT   |   0-255    |   1  | Volts ac peak
|     7  |    16  |AC_INPUT_AMPS    |   0-255    |   1  | Amp AC
|     8  |    17  |AC_OUTPUT_AMPS   |   0-255    |   1  | Amp AC
|     9  | 18,19  |AC_HZ            |   0-120.0  | 0.1  | Hz
|    10  |    11  |BATTERY_TEMP     |   0-150    |   1  | deg C
|    11  |    12  |TRANSFORMER_TEMP |   0-150    |   1  | deg C
|    12  |    13  |FET_TEMP         |   0-150    |   1  | deg C
|  13_L  |    10  |INVERTER_REVISION|   1.0-25.6 | 0.1  | Non-zero is mandatory for remote start up |
|  13_H  |    14  |INVERTER_MODEL   |  | |  |
|  14_L  |     9  |CHARGER_LED      |  | | IF = 0, then CHG LED is off else CHG Led on remote ON |
|  14_H  |     8  |INVERTER_LED     |  | | IF = 0, then INV LED is off else INV Led on remote ON |
|    15  |    15  |STACK_MODE       |  | |  |  |

# Notes
## Inverter status

| Inverter status  | value | Notes|
|:-----------------|:-----:|:-----|
| Charger Standby  |  0x00 | AC in, charging disabled |
| EQMODE           |  0x01 | // unit is eq'ing with ac |
| FLOATMODE        |  0x02 | // unit is float charging w/ac |
| ABSORBMODE       |  0x04 | // unit is charging in absorb w/ ac |
| BULKMODE         |  0x08 | // unit is charging in bulk mode with ac applied |
| BATSAVERMODE     |  0x09 | // unit is charge mode but no charge current since bat is full |
| CHARGEMODE       |  0x10 | // unit is in charge mode, no ac applied |
| Off              |  0x20 | // inverter off, charger off waiting for user input (no AC pass through) |
| INVERTMODE       |  0x40 | // inverter on (charger on or off) |
| Inverter Standby |  0x50 | MS rev 4.0 or later only (PAE) |
| SEARCHMODE       |  0x80 | // searching for load (charger on or off) |
