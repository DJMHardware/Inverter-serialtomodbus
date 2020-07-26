## ModBus definitions
# Coils
# Input Registers
| ModBus Input Register | Inverter RX byte| Name| Range | Scaling | Units/notes|
|-------:|--------:|:--------------:|:--------:|--------:|:-------         |
|     1  |     0  |INVERTER_STATUS  | | |  |
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