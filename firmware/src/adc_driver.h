//-----------------------------------------------------------
// File Name    : adc_driver.h                              |
// Project Name : 23132_instrlabomodulaire                  |
// Version      : V1                                        |
// Date         : 11.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#ifndef _ADC_DRIVER_H
#define	_ADC_DRIVER_H
// ------------ Includes ------------ //
#include <stdint.h>
// ---------- Constants ---------- //
#define ADC_SCAN_ADDRESS 0x0030
#define ADC_RESOLUTION 1023	//ADC resolution (10bits - 1)
#define ADC_SCAN_SPEED 10 //value in [ms]

#define V_REF 3 //reference voltage on ADC (pin VREF+)

#define GAIN_RESISTOR_DIVIDER 0.1f	//Gain Resistor divider
#define GAIN_ATTENUATOR 0.13f  //Gain attenuator 

//#define GAIN1 1
//#define GAIN4 4
//#define GAIN16 16
//#define GAIN64 64

// ---------- Structures ---------- //
typedef struct
{
	uint16_t AN4;
	uint16_t AN5;
}S_RawAdcValues;

void InitADC(void);

S_RawAdcValues ReadAllADC(void);

#endif