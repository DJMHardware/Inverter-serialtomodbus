# ModBus definitions
## Coils
## Input Registers
| Input Register | Inverter RX byte| Name| Range | Scaling | Units/notes|
|-------:|--------:|:--------------:|:--------:|--------:|:-------         |
|     1  |     0  |INVERTER_STATUS  | | | [Inverter status](#inverter-status)|
|     2  |     1  |INVERTER_FAULT   | | | [Inverter fault](#inverter-fault)|
|     3  |   2,3  |DC_VOLTS         |   0-1000   |  0.1 | Volts
|     4  |   4,5  |DC_AMPS          |   0-500    |   1  | Amps
|     5  |     6  |AC_VOLTS_OUTPUT  |   0-150    |   1  | Volts ac RMS
|     6  |     7  |AC_VOLTS_INPUT   |   0-255    |   1  | Volts ac peak
|     7  |    16  |AC_INPUT_AMPS    |   0-255    |   1  | Amp AC
|     8  |    17  |AC_OUTPUT_AMPS   |   0-255    |   1  | Amp AC
|     9  | 18,19  |AC_HZ            |   0-1200   |  0.1 | Hz
|    10  |    11  |BATTERY_TEMP     |   0-150    |   1  | deg C
|    11  |    12  |TRANSFORMER_TEMP |   0-150    |   1  | deg C
|    12  |    13  |FET_TEMP         |   0-150    |   1  | deg C
|  13_L  |    10  |INVERTER_REVISION|   10-256   |  0.1 | Non-zero is mandatory for remote start up |
|  13_H  |    14  |INVERTER_MODEL   |  | | [Inverter model](#inverter-model) |
|  14_L  |     9  |CHARGER_LED      |  | | IF = 0, then CHG LED is off else CHG Led on remote ON |
|  14_H  |     8  |INVERTER_LED     |  | | IF = 0, then INV LED is off else INV Led on remote ON |
|    15  |    15  |STACK_MODE       |  | | [Stack mode](#stack-mode) |

## Holding Registers
| Holding Register | Inverter TX byte| Name| Range | Scaling | Default | Units/notes|
|-----:|----:|:--------------------:|:--------:|--------:|-----:|:-------         |
|    1 | 0   |  INVERTER_ON_OFF     | Read Only  |     |      | use Coils to set |
|    2 | 1   |  SEARCH_WATTS        |   5 - 50   |  1  |   5  | use Coil to turn On/Off |
|    3 | 2   |  BATTERY_SIZE        |  20 - 160  | 10  | 200  | increments of 200 amp/hrs |
|    4 | 3   |  BATTERY_TYPE        | 144 - 200  | 0.1 | 14.4 | custom absorb voltage for presets see [Battery Type](#battery-type)|
|    5 | 4   |  CHARGER_AMPS        | 0% - 100%  |  1  |  50% | Must be in steps of 10 |
|    6 | 5   |  AC_SHORE_AMPS       |    5-60    |  1  |  30  | Amps |
|    7 | 6   |  REMOTE_REVISION     |  10 - 255  | 0.1 |  3.6 |  |
|    8 | 7   |  FORCE_CHARGE        |   0 - 10   | 10  |  10% | [Force charge](#force-charge) |
|    9 | 8   |  AUTO_GENSTART       |   0 - 5    |  1  |   0  | [Auto Genstart](#auto-genstart)|
|   10 | 9   |  LOW_BATTERY_CUT_OUT |  90 - 160  | 0.1 | 10.0 | 12 volt scale |
|      |     |                      | 190 - 255  | 0.1 | 20.0 | 24 volt scale |
|      |     |                      | 190 - 255  | 0.2 | 40.0 | 48 volt scale |
|   11 | 10  |  VAC_CUT_OUT_VOLTAGE | 110 - 254  |     |      | [VAC cut out voltage](#vac-cut-out-voltage)  |
|   12 | 11  |  FLOAT_VOLTS         |  90 - 160  | 0.1 | 13.2 | Volts  |
|   13 | 12  |  EQ_VOLTS            |   0 - 20   | 0.1 | 1.6  | Volts (this is added to Absorption voltage) |
|   14 | 13  |  ABSORB_TIME         |   0 - 255  | 0.1 | 2    | Hours |
| 15_L | 15  |  MINUTES             |   0 - 23   |  1  |      | set once |
| 15_H | 14  |  HOURS               |   0 - 59   |  1  |      |  rtc updates |

# Notes
## Inverter status

| Text  | Value | Notes|
|:------|:-----:|:-----|
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

## Inverter fault

| Text  | Value | Notes|
|:------|:-----:|:-----|
| No Fault on inverter  | 0x00 | // not used |
| STUCKRELAY            | 0x01 |   |
| DC_OVERLOAD           | 0x02 | // DC bridge overload fault  |
| AC_OVERLOAD           | 0x03 | // AC output overload fault  |
| DEAD BAT              | 0x04 | // charging a dead battery  |
| BACKFEED              | 0x05 | AC Backfeed  |
| LOWBAT                | 0x08 | // low bat cutout  |
| HIGHBAT               | 0x09 | // high bat cutout  |
| HIGHACVOLTS           | 0x0A | // high ac output voltage  |
| BAD_BRIDGE            | 0x10 |  // Remote displays = Internal Fault 1 = bad fet bridge |
| NTC_FAULT             | 0x12 |  // Remote displays = Internal Fault 2 = fets too hot (>20C step) |
| FET_OVERLOAD          | 0x13 |  // Remote displays = Fet Overload = FETS too hot too fast (6C over 4 sec) |
| INTERNAL_FAULT4       | 0x14 |  // Not used |
| STACKER_MODE_FAULT    | 0x16 |  // Stacker fault, both units not in same mode, problem with "other" inverter |
| STACKER_CLK_PH_FAULT  | 0x18 |  // Stacker sync clocks are not in phase |
| STACKER_NO_CLK_FAULT  | 0x17 |  // Stacker does not detect other stackers sync clock |
| STACKER_PH_LOSS_FAULT | 0x19 |  // Stacker AC output voltage is not properly phased |
| OVERTEMP              | 0x20 |  // overtemp shutdown |
| RELAY_FAULT           | 0x21 |  // Transfer relay is not closed in chg mode |
| CHARGER_FAULT         | 0x80 |  // Bridge fault in chg mode = max PWM, no ac amps, Vbat < FloatV |
| HIBATEMP              | 0x81 |  // high battery temp |
| OPEN_SELCO_TCo        | 0x90 |  // open transformer TCO temperature cutout |
| CB3 OPEN FAULT        | 0x91 |  // open input 30 amp AC Breaker CB3 |

## Inverter model

| Text  | Decimal | Hex |
|:------|------:|:-----|
| MM612        |  6   |    0x06 |
| MM612-AE     |  7   |    0x07 |
| MM1212       |  8   |    0x08 |
| MMS1012      |  9   |    0x09 |
| MM1012E      |  10  |   0x0A  |
| MM1512       |  11  |   0x0B  |
| ME1512       |  15  |   0x0F  |
| ME2012       |  20  |   0x14  |
| ME2512       |  25  |   0x19  |
| ME3112       |  30  |   0x1E  |
| MS2012       |  35  |   0x23  |
| MS2012E      |  40  |   0x28  |
| MS2812       |  45  |   0x2D  |
| MS2712E      |  47  |   0x2F  |
| MM1324E      |  53  |   0x35  |
| MM1524       |  54  |   0x36  |
| RD1824       |  55  |   0x37  |
| RD2624E      |  59  |   0x3B  |
| MM1x24E      |  60  |   0x3C  |
| RD2824       |  63  |   0x3F  |
| RD4024E      |  69  |   0x45  |
| RD3924       |  74  |   0x4A  |
| MS4124E      |  90  |   0x5A  |
| MS2024       |  91  |   0x5B  |
| MS4024       |  105 |  0x69   |
| MS4024AE     |  106 |  0x6A   |
| MS4024PAE    |  107 |  0x6B   |
| MS4448AE     |  111 |  0x6F   |
| MS3748AEJ    |  112 |  0x70   |
| MS4448PAE    |  115 |  0x73   |
| MS3748PAEJ   |  116 |  0x74   |

## Stack mode

| Text  | Hex |
|:------|:-----|
| Standalone unit         | 0x00 |
| Parallel stack - master | 0x01 |
| Parallel stack - slave  | 0x02 |
| Series stack - master   | 0x04 |
| Series stack - slave    | 0x08 |

## Battery Type

| Text  | Decimal | Note |
|:--------|:-----:|:-----|
| Gel     |  2    |       |
| Flooded |  4    | (default) |
| AGM     |  8    |       |
| AGM2    |  10   |       |
| Custom  |  100+ |This becomes Absorption; 144 = 14.4 volts. |

## Force Charge

| Text  | Bit # | Notes |
|:------|:------:|:------|
| Parallel threshold  | 0-3 | 1 count = 10% of unit capacity |
| Disable Refloat     |  4  | read only use modbus Coil to control |
| Force Silent        |  5  | read only use modbus Coil to control |
| Force Float         |  6  | read only use modbus Coil to control |
| Force Bulk          |  7  | read only use modbus Coil to control |

## Auto Genstart

| Text  | Hex | Notes |
|:------|:------:|:------|
| Off                    | 0x00 | (default)  |
| Enable                 | 0x01 | Allow Auto Gen Start  |
| Test                   | 0x02 |   |
| Enable with Quiet Time | 0x04 | Don't start during quiet time hours  |
| On                     | 0x05 | Only in AGS rev 5.0 and later |

## VAC cut out voltage

| Text  | Decimal | Note |
|:------|:-----:|:-----|
| 60VAC |  110  |            |
| 65V   |  122  |            |
| 70V   |  135  |            |
| 75V   |  145  |            |
| 80V   |  155  |  (default) |
| 85V   |  165  |            |
| 90V   |  175  |            |
| 95V   |  182  |            |
| 100V  |  190  |            |
| EMS over-ride- open relay | 255 | read only use modbus Coil to control |
