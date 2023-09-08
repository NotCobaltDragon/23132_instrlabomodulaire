//-----------------------------------------------------------
// File Name    : adc_driver.h                              |
// Project Name : TP4 EIND Alimentation uC                  |
// Version      : V1                                        |
// Date         : 19.06.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#ifndef _ADC_DRIVER_H
#define	_ADC_DRIVER_H
// ------------ Includes ------------ //
#include <stdint.h>
// ---------- Constants ---------- //
#define ADC_SCAN_ADDRESS 0x0030
#define ADC_RESOLUTION 1023	//10bits

#define GAIN_R_DIVIDER 0.1
#define GAIN_ATTENUATOR 0.13

#define VREF 3000 //voltage on VREF pin in [mV]

#define GAIN1 1
#define GAIN4 4
#define GAIN16 16
#define GAIN64 64

// ---------- Structures ---------- //
typedef struct
{
	uint16_t AN4;
	uint16_t AN5;
}S_RawAdcValues;

typedef struct
{
	uint16_t vdc; //Voltage in direct current mode
	uint16_t vac; //Voltage in alternative current mode
}S_ConvertedAdcValues;


void InitADC(void);

S_RawAdcValues ReadAllADC(void);

#endif