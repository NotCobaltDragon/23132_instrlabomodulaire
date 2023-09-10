//-----------------------------------------------------------
// File Name    : adc_driver.c                              |
// Project Name : 23132_instrlabomodulaire                  |
// Version      : V1                                        |
// Date         : 11.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#include "adc_driver.h"
#include "system_config.h"
#include "peripheral/adc/plib_adc.h"


void InitADC(void)
{
	PLIB_ADC_InputScanMaskAdd(ADC_ID_1, ADC_SCAN_ADDRESS);
	// Configure l'ADC en mode alterné
	PLIB_ADC_ResultFormatSelect(ADC_ID_1, ADC_RESULT_FORMAT_INTEGER_16BIT);
	//Mode alterné
	PLIB_ADC_ResultBufferModeSelect(ADC_ID_1, ADC_BUFFER_MODE_TWO_8WORD_BUFFERS);
	//mode multiplexage
	PLIB_ADC_SamplingModeSelect(ADC_ID_1, ADC_SAMPLING_MODE_MUXA);

	//la lecture des ADC est cadensée par le timer interne
	PLIB_ADC_ConversionTriggerSourceSelect(ADC_ID_1, ADC_CONVERSION_TRIGGER_INTERNAL_COUNT);
	PLIB_ADC_VoltageReferenceSelect(ADC_ID_1, ADC_REFERENCE_VREFPLUS_TO_AVSS);
	PLIB_ADC_SampleAcquisitionTimeSet(ADC_ID_1, 0x1F);
	PLIB_ADC_ConversionClockSet(ADC_ID_1, SYS_CLK_FREQ, 32);

	//ADC fait 3 mesures par interruption (car 3 canaux utillises) 
	PLIB_ADC_SamplesPerInterruptSelect(ADC_ID_1, ADC_3SAMPLES_PER_INTERRUPT);
	//active le scan en mode multiplexage des entrées AN
	PLIB_ADC_MuxAInputScanEnable(ADC_ID_1);

	// Enable the ADC module
	PLIB_ADC_Enable(ADC_ID_1);
}


S_RawAdcValues ReadAllADC(void)
{
	//structure de valeurs brutes des ADCs
	volatile S_RawAdcValues rawResult;
	ADC_RESULT_BUF_STATUS BufStatus;    
//	S_ADCResults meanedResult;

	//stop sample/convert
	PLIB_ADC_SampleAutoStartDisable(ADC_ID_1);

	// traitement avec buffer alterné
	BufStatus = PLIB_ADC_ResultBufferStatusGet(ADC_ID_1);

	if (BufStatus == ADC_FILLING_BUF_0TO7) {
		rawResult.AN4  = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 0);
		rawResult.AN5  = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 1);  
	}
	else
	{
		rawResult.AN4  = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 8);
		rawResult.AN5  = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 9);
	}

	// Retablit Auto start sampling
	PLIB_ADC_SampleAutoStartEnable(ADC_ID_1);
        
    return rawResult;
}

S_ConvertedAdcValues ConvertAdcValues(void)
{
	S_RawAdcValues rawValues;
	S_ConvertedAdcValues convertedValues;

	rawValues = ReadAllADC();

	//Direct current conversion

	//Alternative current conversion
	return(convertedValues);
}