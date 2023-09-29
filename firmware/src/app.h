/*******************************************************************************
	MPLAB Harmony Application Header File

	Company:
		Microchip Technology Inc.

	File Name:
		app.h

	Summary:
		This header file provides prototypes and definitions for the application.

	Description:
		This header file provides function prototypes and data type definitions for
		the application.  Some of these are required by the system (such as the
		"APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
		internally by the application (such as the "APP_STATES" definition).  Both
		are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "rs485_driver.h"
//#include "../../../23132_instrlabomodulaire/firmware/src/rs485_commands.h"
#include "voltmeter_cmd.h"
#include "adc_driver.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#define DEFAULT_GAIN 1

#define STATUS_LED_BLINK_SPEED 100 //value in [ms]
#define COOLDOWN_TIME 100 //value in [ms]

// *****************************************************************************
/* Application states

	Summary:
		Application states enumeration

	Description:
		This enumeration defines the valid application states.  These states
		determine the behavior of the application at various times.
*/

typedef enum
{
	/* Application's state machine's initial state. */
	APP_STATE_INIT=0,
	APP_STATE_SERVICE_TASKS,
		APP_STATE_RECEIVE_COMMAND,
		APP_STATE_SEND_COMMAND,
		APP_STATE_WAIT,
	/* TODO: Define states used by the application state machine. */

} APP_STATES;

typedef enum 
{
	DC_MODE = 0,
	AC_MODE,
} CURRENT_MODE;

typedef enum
{
		GAIN_1 = 0,
		GAIN_4,
		GAIN_16,
		GAIN_64,
} GAIN_SELECT;

//Threshold values for auto gain ajust, values in volts
#define GAIN_1_MIN_THRSH	20
#define GAIN_1_MAX_THRSH	110
#define GAIN_4_MIN_THRSH 	6 
#define GAIN_4_MAX_THRSH	28
#define GAIN_16_MIN_THRSH	1.5
#define GAIN_16_MAX_THRSH	7
#define GAIN_64_MIN_THRSH	0
#define GAIN_64_MAX_THRSH	1.8
// *****************************************************************************
/* Application Data

	Summary:
		Holds application data

	Description:
		This structure holds the application's data.

	Remarks:
		Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
		/* The application's current state */
		APP_STATES state;
		uint8_t idValue;

		bool needSendCommand;
		bool canReceiveCommand;
		bool cmdReadyToSend;
		bool isUsartOpened;

		bool coolDownActive;
		bool flagCooldownReached;

		uint8_t receivedCommand;
		uint8_t receivedParameter;

		bool currentMode;
		GAIN_SELECT gainSelectEnum;

		float valueVoltmeterDc;
		float valueVoltmeterAc;

		uint8_t gainSelected;
		uint8_t modeSelected;

		/* TODO: Define any additional data used by the application. */

} APP_DATA;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/
	
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
	Function:
		void APP_Initialize ( void )

	Summary:
		 MPLAB Harmony application initialization routine.

	Description:
		This function initializes the Harmony application.  It places the 
		application in its initial state and prepares it to run so that its 
		APP_Tasks function can be called.

	Precondition:
		All other system initialization routines should be called before calling
		this routine (in "SYS_Initialize").

	Parameters:
		None.

	Returns:
		None.

	Example:
		<code>
		APP_Initialize();
		</code>

	Remarks:
		This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
	Function:
		void APP_Tasks ( void )

	Summary:
		MPLAB Harmony Demo application tasks function

	Description:
		This routine is the Harmony Demo application's tasks function.  It
		defines the application's state machine and core logic.

	Precondition:
		The system and application initialization ("SYS_Initialize") should be
		called before calling this.

	Parameters:
		None.

	Returns:
		None.

	Example:
		<code>
		APP_Tasks();
		</code>

	Remarks:
		This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks(void);

uint8_t GetID(void);

void InitCommands(void);

void SendModuleId(const char* cmdParameter);

void SetVoltmeterMode(const char* cmdParameter);

void SetVoltmeterGain(const char* cmdParameter);

void SetVoltmeterDefault(void);

void ReadVoltmeterValue(const char* cmdParameter);

void StatusLEDCallback(void);

void CoolDownCallback(void);

void ADC_Callback(void);

void UpdateAnalogGain(uint8_t setGain);

bool AutoGainSelect(void);

void ErrorHandler(void);

float convertRawToVoltage(uint16_t rawResult, uint8_t gainSelected);

void UpdateAnalogGain(uint8_t setGain);



#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

