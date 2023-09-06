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

RS485_DATA rs485Data;

CURRENT_MODE currentMode;

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
	appData.currentMode = AC_MODE;
	appData.canReceiveCommand = true;
	SetVoltmeterMode(0);
	SetVoltmeterGain(appData.currentMode);

	
	/* TODO: Initialize your application's state machine and other
	 * parameters.
	 */
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
			Init_RS485(RECEIVING);

			if(!DRV_USART_TransmitBufferIsFull(rs485Data.usartHandle))
			DRV_USART_WriteByte(rs485Data.usartHandle, 'A');
			DRV_TMR0_Start();

			//Init ADC
			//Init TMR
			appData.idValue = GetID();
			rs485Data.id = appData.idValue;
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
			switch(appData.receivedCommand)
			{
				case E_CMD_IDQUESTION:
					sprintf(txBuffer, "ID%d%s%d", rs485Data.id, cmdData[appData.receivedCommand], 0);
					break;
				case E_CMD_VOLTMGAIN:
				{
					SetVoltmeterGain(appData.receivedParameter);
					sprintf(txBuffer, "ID%d%s%d", rs485Data.id, cmdData[appData.receivedCommand], appData.receivedParameter);
					break;
				}					
				case E_CMD_VOLTMMODE:
					SetVoltmeterMode(appData.receivedParameter);
					sprintf(txBuffer, "ID%d%s%d", rs485Data.id, cmdData[appData.receivedCommand], appData.receivedParameter);
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
			break;
		}
		case APP_STATE_RECEIVE_COMMAND:
		{
			LED2On();
			LED3Off();

			rxBuffer[0] = GetMessage(rxBuffer);

			appData.receivedCommand = ExtractCommand(rxBuffer);

			if(ExtractId(rxBuffer) == appData.idValue || appData.receivedCommand == E_CMD_IDQUESTION)
			{
				appData.receivedParameter = ExtractParameter(rxBuffer);

				appData.cmdReadyToSend = false;
				appData.canReceiveCommand = false;
				appData.needSendCommand = true;
			}
			else
			{
				//clear receive buffer
			}
			appData.state = APP_STATE_SERVICE_TASKS;	
			break;
		}
		case APP_STATE_SEND_COMMAND:
		{
			LED2Off();
			LED3On();
			RS485_Direction_Mode(SENDING);
			appData.needSendCommand = SendMessage(txBuffer);
			appData.canReceiveCommand = true;
			RS485_Direction_Mode(RECEIVING);
			appData.state = APP_STATE_SERVICE_TASKS;
			break;
		}

		case APP_STATE_WAIT:
		{
			//Do nothing
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
uint8_t ExtractId(char rxBuffer)
{
	uint8_t id;
	char tempBuffer[3];

	strncpy(tempBuffer, rxBuffer, 3);
	id = atoi(tempBuffer);

	return id;
}


uint8_t ExtractCommand(char rxBuffer)
{
	uint8_t commandPosition;
	for(commandPosition = 0; commandPosition < NB_CMD; commandPosition++)
	{
		if(strcmp(cmdData[commandPosition], rxBuffer) == 0)
		{
			return commandPosition;
		}
	}
	return E_CMD_ERROR;
}

uint8_t ExtractParameter(char rxBuffer)
{
	//uint8_t startIndex = 6;
	uint8_t parameter;
	char tempChar[10];
	tempChar[0] = rxBuffer;

	//tempChar = tempChar[6];
	parameter = tempChar[6] - '0';

	/*while((rxBuffer[startIndex] >= '0') && (rxBuffer[startIndex] <= '9'))
	{
		
		startIndex++;
	}*/
	return parameter;
}

void SetVoltmeterMode(uint8_t mode)
{
	if(mode = DC_MODE)
	{
		Relay_ACOff();
		appData.currentMode = DC_MODE;
	}
	else
	{
		Relay_ACOn();
		appData.currentMode = AC_MODE;
	}
}

void SetVoltmeterGain(uint8_t gain)
{
	switch(gain)
	{
		case 0:
			Scale_1Off();
			Scale_2Off();
			Scale_3Off();
			break;
		case 1:
			Scale_2Off();
			Scale_3Off();
			Scale_1On();
			break;
		case 2:
			Scale_1Off();
			Scale_3Off();
			Scale_2On();
			break;
		case 3:
			Scale_1Off();
			Scale_2Off();
			Scale_3On();
			break;
		default:
			break;
	}
}

/*******************************************************************************
 End of File
 */
