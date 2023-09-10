/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
	Microchip Technology Inc.
  
  File Name:
	app.c

  Summary:
	This file contains the source code for the MPLAB Harmony application.

  Description:
	This file contains the source code for the MPLAB Harmony application.  It 
	implements the logic of the application's state machine and it may call 
	API routines of other MPLAB Harmony modules in the system, such as drivers,
	system services, and middleware.  However, it does not call any of the
	system interfaces (such as the "Initialize" and "Tasks" functions) of any of
	the modules in the system or make any assumptions about when those functions
	are called.  That is the responsibility of the configuration-specific system
	files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
	Holds application data

  Description:
	This structure holds the application's data.

  Remarks:
	This structure should be initialized by the APP_Initialize function.
	
	Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

extern RS485_DATA rs485Data;

CURRENT_MODE currentMode;

S_RawAdcValues rawResult;

S_RawAdcValues converted;

char txBuffer[8];
char rxBuffer[8];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
	void APP_Initialize ( void )

  Remarks:
	See prototype in app.h.
 */

void APP_Initialize ( void )
{
	/* Place the App state machine in its initial state. */
	appData.state = APP_STATE_INIT;
	appData.idValue = 0;
	appData.receivedCommand = 0;
	//appData.currentMode = DC_MODE;
	appData.gainSelect = GAIN_1;
	appData.canReceiveCommand = true;
}


/******************************************************************************
  Function:
	void APP_Tasks ( void )

  Remarks:
	See prototype in app.h.
 */

void APP_Tasks ( void )
{

	/* Check the application's current state. */
	switch ( appData.state )
	{
		/* Application's initial state. */
		case APP_STATE_INIT:
		{
			rs485Data.id = GetID();
			//SetVoltmeterMode(DC_MODE);
			Relay_AC_Off();
			SetVoltmeterGain(appData.gainSelect);

			appData.isUsartOpened = Init_RS485(RECEIVING);

			if(appData.isUsartOpened == false)
				LED4On();	//Error LED

			InitADC();

			DRV_TMR0_Start();

			appData.state = APP_STATE_SERVICE_TASKS;
			break;
		}
		case APP_STATE_SERVICE_TASKS:
		{
			if((!DRV_USART_ReceiverBufferIsEmpty(rs485Data.usartHandle))&&(appData.canReceiveCommand == true))
			{
				appData.state = APP_STATE_RECEIVE_COMMAND;
			}
			else if(appData.needSendCommand == true && appData.cmdReadyToSend == true)
			{
				appData.state = APP_STATE_SEND_COMMAND;
			}
			break;
		}
		case APP_STATE_RECEIVE_COMMAND:
		{
			char receivedCommand[4];
			char receivedParameter;
			LED2On();

			GetMessage(rxBuffer);
			sscanf(rxBuffer, "ID%u%3s%1s", &appData.idValue, receivedCommand, &receivedParameter); //Get individual parameters

			if(appData.idValue == rs485Data.id)
			{
				for(appData.receivedCommand = 0; appData.receivedCommand < NB_CMD; appData.receivedCommand++)
				{
					if(strcmp(cmdData[appData.receivedCommand], receivedCommand) == 0)
					{
						break;
					}
				}
				appData.receivedParameter = atoi(&receivedParameter);
				appData.cmdReadyToSend = false;
				appData.canReceiveCommand = false;
				appData.needSendCommand = true;
				appData.state = APP_STATE_APPLY_SETTINGS;
			}
			else
			{
				appData.state = APP_STATE_SERVICE_TASKS;
			}
			LED2Off();
			break;
		}
		case APP_STATE_APPLY_SETTINGS:
		{
			switch(appData.receivedCommand)
			{
				case E_CMD_IDQUESTION:
					sprintf(txBuffer, "ID%d%s%d", rs485Data.id, cmdData[appData.receivedCommand], 0);
					appData.cmdReadyToSend = true;
					break;
				case E_CMD_VOLTMGAIN:
				{
					SetVoltmeterGain(appData.receivedParameter);
					sprintf(txBuffer, "ID%d%s%d", rs485Data.id, cmdData[appData.receivedCommand], appData.receivedParameter);
					appData.cmdReadyToSend = true;
					break;
				}					
				case E_CMD_VOLTMMODE:
					SetVoltmeterMode(appData.receivedParameter);
					sprintf(txBuffer, "ID%d%s%d", rs485Data.id, cmdData[appData.receivedCommand], appData.receivedParameter);
					appData.cmdReadyToSend = true;
					break;
				case E_CMD_VOLTMREAD:
				{
					switch(appData.currentMode)
					{
						case DC_MODE:
							//Read ADC DC
							//Make conversion
							break;
						case AC_MODE:
							//Read ADC AC
							//Make converion
							break;
						default:
							break;
					}
					appData.valueVolt = 10;
					appData.valueVoltTenth = 33;

					sprintf(txBuffer, "ID%d%s%d.%d", rs485Data.id, cmdData[appData.receivedCommand], appData.valueVolt, appData.valueVoltTenth);
				}
				default:
					break;
			}
			appData.state = APP_STATE_SERVICE_TASKS;
			break;
		}
		case APP_STATE_SEND_COMMAND:
		{
			LED3On();
			RS485_Direction_Mode(SENDING);
			SendMessage(txBuffer);
			appData.needSendCommand = false;
			appData.cmdReadyToSend = false;
			RS485_Direction_Mode(RECEIVING);
			appData.canReceiveCommand = true;
			LED3Off();
			appData.state = APP_STATE_SERVICE_TASKS;
			break;
		}
		/* The default state should never be executed. */
		default:
		{
			/* TODO: Handle error in application's state machine. */
			break;
		}
	}
}

uint8_t GetID()
{
    uint8_t idValue = 0;

    // Read each ID pin and construct the 4-bit value
    idValue |= (ID_8StateGet() & 0x01) << 3;
    idValue |= (ID_4StateGet() & 0x01) << 2;
    idValue |= (ID_2StateGet() & 0x01) << 1;
    idValue |= (ID_1StateGet() & 0x01) << 0;

    return idValue;
}

void SetVoltmeterMode(bool mode)
{
	if(mode = DC_MODE)
	{
		Relay_AC_Off();
	}
	else if(mode = AC_MODE)
	{
		Relay_AC_On();
	}
}

void SetVoltmeterGain(GAIN_SELECT gain)
{
	switch(gain)
	{
		case GAIN_1:
			Scale_1Off();
			Scale_2Off();
			Scale_3Off();
			break;
		case GAIN_4:
			Scale_2Off();
			Scale_3Off();
			Scale_1On();
			break;
		case GAIN_16:
			Scale_1Off();
			Scale_3Off();
			Scale_2On();
			break;
		case GAIN_64:
			Scale_1Off();
			Scale_2Off();
			Scale_3On();
			break;
		default:
			break;
	}
}

void StatusLEDCallback(void)
{
	static uint8_t counterStatusLed = 0;
	counterStatusLed++;

	if(counterStatusLed > STATUS_LED_BLINK_SPEED)
	{
		LED1Toggle();
		counterStatusLed = 0;
	}
}

void ADC_Callback(void)
{
//	static uint8_t counterAdcScan = 0;
//	counterAdcScan++;
//
//	if(counterAdcScan > ADC_SCAN_SPEED)
//	{
//		rawResult = ReadAllADC();
//		static float AN4;
//		static float AN5;
//
//		AN4 = ((float)3/1024) * rawResult.AN4 - 1.5;
//		AN5 = ((float)3/1024) * rawResult.AN5 - 1.5;
//
//		counterAdcScan = 0;
//	}
}

/*******************************************************************************
 End of File
 */
