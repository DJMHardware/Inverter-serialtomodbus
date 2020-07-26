/*
 * File:   inverter.h
 * Author: sasquachdm
 *
 * Created on June 22, 2020, 4:10 PM
 */

#ifndef INVERTER_H
#define	INVERTER_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t high;
    uint8_t low;
}uint16nib_t;

typedef struct {
    uint8_t INVERTER_STATUS;      // 0         1
    uint8_t INVERTER_FAULT;       // 1         2
    uint16nib_t DC_VOLTS;         // 2,3       3
    uint16nib_t DC_AMPS;          // 4,5       4
    uint8_t AC_VOLTS_OUTPUT;      // 6         5
    uint8_t AC_VOLTS_INPUT;       // 7         6
    uint8_t INVERTER_LED;         // 8        14_H
    uint8_t CHARGER_LED;          // 9        14_L
    uint8_t INVERTER_REVISION ;   // 10       13_L
    uint8_t BATTERY_TEMP ;        // 11       10
    uint8_t TRANSFORMER_TEMP ;    // 12       11
    uint8_t FET_TEMP ;            // 13       12
    uint8_t INVERTER_MODEL ;      // 14       13_H
    uint8_t STACK_MODE ;          // 15       15
    uint8_t AC_INPUT_AMPS ;       // 16        7
    uint8_t AC_OUTPUT_AMPS ;      // 17        8
    uint16nib_t AC_HZ;            // 18,19     9
    uint8_t padding;
} inverter_rx_t;

typedef struct {
    uint16_t INVERTER_STATUS;      //  0       1
    uint16_t INVERTER_FAULT;       //  1       2
    uint16_t DC_VOLTS;             //  2,3     3
    uint16_t DC_AMPS;              //  4,5     4
    uint16_t AC_VOLTS_OUTPUT;      //  6       5
    uint16_t AC_VOLTS_INPUT;       //  7       6
    uint16_t AC_INPUT_AMPS ;       // 16       7
    uint16_t AC_OUTPUT_AMPS ;      // 17       8
    uint16_t AC_HZ;                // 18,19    9
    uint16_t BATTERY_TEMP ;        // 11      10
    uint16_t TRANSFORMER_TEMP ;    // 12      11
    uint16_t FET_TEMP ;            // 13      12
    uint8_t INVERTER_REVISION ;    // 10      13_L
    uint8_t INVERTER_MODEL ;       // 14      13_H
    uint8_t CHARGER_LED;           //  9      14_L
    uint8_t INVERTER_LED;          //  8      14_H
    uint16_t STACK_MODE ;          // 15      15
} inverter_mbs_input_t;

typedef struct {
    uint8_t INVERTER_ON_OFF;        //  0      1
    uint8_t SEARCH_WATTS;           //  1      2
    uint8_t BATTERY_SIZE;           //  2      3
    uint8_t BATTERY_TYPE;           //  3      4
    uint8_t CHARGER_AMPS;           //  4      5
    uint8_t AC_SHORE_AMPS;          //  5      6
    uint8_t REMOTE_REVISION;        //  6      7
    uint8_t FORCE_CHARGE;           //  7      8
    uint8_t AUTO_GENSTART;          //  8      9
    uint8_t LOW_BATTERY_CUT_OUT;    //  9     10
    uint8_t VAC_CUT_OUT_VOLTAGE;    // 10     11
    uint8_t FLOAT_VOLTS;            // 11     12
    uint8_t EQ_VOLTS;               // 12     13
    uint8_t ABSORB_TIME;            // 13     14
    uint8_t HOURS;                  // 14     15_H
    uint8_t MINUTES;                // 15     15_L
} inverter_tx_t;

typedef struct {
    uint16_t INVERTER_ON_OFF;       //  0      1
    uint16_t SEARCH_WATTS;          //  1      2
    uint16_t BATTERY_SIZE;          //  2      3
    uint16_t BATTERY_TYPE;          //  3      4
    uint16_t CHARGER_AMPS;          //  4      5
    uint16_t AC_SHORE_AMPS;         //  5      6
    uint16_t REMOTE_REVISION;       //  6      7
    uint16_t FORCE_CHARGE;          //  7      8
    uint16_t AUTO_GENSTART;         //  8      9
    uint16_t LOW_BATTERY_CUT_OUT;   //  9     10
    uint16_t VAC_CUT_OUT_VOLTAGE;   // 10     11
    uint16_t FLOAT_VOLTS;           // 11     12
    uint16_t EQ_VOLTS;              // 12     13
    uint16_t ABSORB_TIME;           // 13     14
    uint8_t MINUTES;               // 15     15_L
    uint8_t HOURS;                 // 14     15_H
} inverter_mbs_holding_t;

typedef struct {
    uint8_t Inverter : 1;
    uint8_t Charger : 1;
    uint8_t SearchMode : 1;
    uint8_t ACInput : 1;
    uint8_t DisableRefloat : 1;
    uint8_t ForceSilent : 1;
    uint8_t ForceFloat : 1;
    uint8_t ForceBulk : 1;
} onoff_t;

typedef struct{
    onoff_t OnOff;
    uint8_t ForceEQ : 1;
    uint8_t WriteEEprom : 1;
    
} inverter_mbs_coil_t;

typedef struct{
    uint8_t Inverter_On_Off : 1;
    uint8_t Inverter_led_solid : 1;
    uint8_t Inverter_led_blink : 1;
    uint8_t Charger_On_Off : 1;
    uint8_t Charger_led_solid : 1;
    uint8_t Charger_led_blink : 1;
    uint8_t AC_in_valid : 1;
    uint8_t RX_Timeout_between_bytes : 1;
    uint8_t RX_Timeout_between_packets : 1;
    uint8_t RX_Extra_byte : 1;
    uint8_t Inverter_state_error : 1;
    uint8_t Charger_state_error : 1;
   
} inverter_mbs_input_s_t;

typedef struct {
    uint8_t OnOff : 1;
    uint8_t blink : 1;
    uint8_t last : 1;
    uint8_t test : 1;
    uint8_t cnt : 4;
    uint8_t long_cnt;
    uint8_t attempts : 2;
} inverter_LED_state_t;

typedef struct {
    inverter_LED_state_t Inverter;
    inverter_LED_state_t Charger;
} inverter_LED_t;

typedef union {
    onoff_t Coil;
    inverter_tx_t Name;
    uint8_t Array[sizeof(inverter_tx_t)-2];
} eeprom_data_t;

typedef struct {
    union{
        inverter_rx_t Name;
        uint8_t Array[sizeof (inverter_rx_t)];
    } RX;
    uint8_t RX_Array_index;
    union {
        inverter_tx_t Name;
        uint8_t Array[sizeof (inverter_tx_t)];
    } TX;
    uint8_t TX_Array_index;
    uint8_t TX_Wait_count;
    uint8_t Serial_state;
    uint8_t Last_bytetime;
    uint8_t Tick_count;
    inverter_LED_t LED;
} inverter_data_t;

typedef struct {
    union{
        inverter_mbs_input_t Name;
        uint8_t Array[sizeof (inverter_mbs_input_t)];
        uint16_t Registers[(sizeof (inverter_mbs_input_t)/2)];
    } Input;
    union {
        inverter_mbs_holding_t Name;
        uint8_t Array[sizeof (inverter_mbs_holding_t)];
        uint16_t Registers[(sizeof (inverter_mbs_holding_t)/2)];
    } Holding;
    union {
        inverter_mbs_input_s_t Name;
        uint8_t Array[sizeof(inverter_mbs_input_s_t)];
    } InputStatus;
    union {
        inverter_mbs_coil_t Name;
        uint8_t Array[sizeof(inverter_mbs_coil_t)];
    } Coil;
} modbus_data_t;

#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

typedef enum{
    RXStandby,
    RXActive,
    RXComplete,
    TXStandby,
    TXActive,
    TXComplete,
    TestData,
    ComputeResponse
} inverter_com_stat_t;

extern volatile inverter_com_stat_t Inverter_Com_stat;
extern volatile inverter_data_t InverterData;
extern volatile modbus_data_t modbusData;

extern void RTC_0_init(void);

extern void USART_1_init(void);

extern void INVERTER_init(void);

extern void INVERTER_tick(void);

void USART_1_enable(void);

void USART_1_enable_rx(void);

void USART_1_enable_tx(void);

void USART_1_disable(void);

void INVERTER_RX_copy_data(void);

void INVERTER_TX_copy_data(void);

void INVERTER_RX_TestData(void);

extern void INVERTER_RX_ComputeResponse(void);

void INVERTER_load_eeprom(void);

void INVERTER_store_eeprom(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INVERTER_H */
