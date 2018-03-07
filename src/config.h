#ifndef __CONFIG_H__
#define __CONFIG_H__

// REF_VOTAGE and CELL_ADDRESS are defined in the Makefile

#define BALANCE_PIN 0
#define THERMISTOR_PIN 1
#define RX_PIN 4
#define TX_PIN 3

#define ADC_NUM_SAMPLES 10

#define THERMISTOR_NOMINAL 100000
#define THERMISTOR_COEF 3950
#define TEMP_NOMINAL 25

#define COMMAND_SEND_VOLTAGE (1 << 0)
#define COMMAND_SEND_TEMP (1 << 1)
#define COMMAND_BALANCE_ON (1 << 2)

#endif
