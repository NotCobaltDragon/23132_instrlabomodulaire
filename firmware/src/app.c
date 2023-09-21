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

extern COMMAND_MAPPING commandMapping[];

RX_TX_DATA sending;

RX_TX_DATA received;

CURRENT_MODE currentMode;

S_RawAdcValues rawResult;

S_RawAdcValues converted;

//char txBuffer[RX_TX_BUFFER_SIZE];
//char rxBuffer[RX_TX_BUFFER_SIZE];

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
	appData.gainSelected = 1.0;
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
			appData.coolDownActive = false;

			rs485Data.id = GetID();

			SetVoltmeterDefault();

			appData.isUsartOpened = Init_RS485(RECEIVING);

			if(appData.isUsartOpened == false)
				ErrorHandler();

			InitADC();

			InitCommands();

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
			//else if(appData.needSendCommand == true && appData.cmdReadyToSend == true)
			//{
			//	appData.state = APP_STATE_SEND_COMMAND;
			//}
			//if(appData.flagCooldownReached == true)
			//{
			//	RS485_Direction_Mode(RECEIVING);
			//	appData.flagCooldownReached = false;
			//	appData.canReceiveCommand = true;
			//}
			break;
		}
		case APP_STATE_RECEIVE_COMMAND:
		{
			uint8_t counter;
			LED2On();

			GetMessage(received.buffer);
			sscanf(received.buffer, "ID%u%4s%s", &received.id, received.command, received.parameter); //Get individual parameters
			if(IdChecker(received.id, rs485Data.id) == true)	//Is the module concerned by the command
			{
				for(counter = 0; counter < MAX_NB_COMMANDS; counter++)	//Checking for a matching command
				{
					if(strcmp(received.command, commandMapping[counter].command) == 0)
					{
						commandMapping[counter].cmdFunctionPtr(received.parameter);
						ClearBuffer(received.buffer);
						appData.cmdReadyToSend = true;
						appData.canReceiveCommand = false;
						//appData.needSendCommand = true;
						appData.state = APP_STATE_SEND_COMMAND;
						LED2Off();
						break;
					}
				}
			}
			break;
		}

		case APP_STATE_SEND_COMMAND:
		{
			LED3On();
			
			appData.coolDownActive = true;
			if(appData.flagCooldownReached == true)
			{
				appData.flagCooldownReached = false;
				RS485_Direction_Mode(SENDING);
				SendMessage(sending.buffer);
				ClearBuffer(sending.buffer);
				appData.needSendCommand = false;
				appData.cmdReadyToSend = false;
				appData.canReceiveCommand = true;
				RS485_Direction_Mode(RECEIVING);
				LED3Off();
				//appData.coolDownActive = true;
				appData.state = APP_STATE_SERVICE_TASKS;
			}
			break;
		}
		/*case APP_STATE_APPLY_SETTINGS:
		{
			switch(appData.receivedCommand)
			{
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

					sprintf(sending.buffer, "ID%d%s%d.%d", rs485Data.id, cmdData[appData.receivedCommand], appData.valueVolt, appData.valueVoltTenth);*/

		/* The default state should never be executed. */
		default:
		{
			ErrorHandler();
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

void InitCommands()
{
	//General Commands (GC_xxx)
	RegisterCommand(GC_GETID_CMD, SendModuleId);
	//Voltmeter Commands (VM_xxx)
	RegisterCommand(VM_SET_GAIN_CMD, SetVoltmeterGain);	//VoltMeter Set Gain
	RegisterCommand(VM_SET_CURRENT_MODE_CMD, SetVoltmeterMode);	//VoltMeter Current Mode
	RegisterCommand(VM_READ_VOLTAGE_CMD, ReadVoltmeterValue);	//VoltMeter Read Voltage
}

void SendModuleId(const char* cmdParameter)
{
	sprintf(sending.buffer, "ID%d%s%s", rs485Data.id, received.command, 23132);
}

void SetVoltmeterDefault()
{
	Relay_AC_Off();

	//SetVoltmeterGain(DEFAULT_GAIN);	//Minimum gain
}

//void SetVoltmeterMode(bool mode)
void SetVoltmeterMode(const char* cmdParameter)
{
	if(*cmdParameter == '0')
	{
		Relay_AC_Off();
	}
	else if(*cmdParameter == '1')
	{
		Relay_AC_On();
	}
   sprintf(sending.buffer, "ID%d%s%s", rs485Data.id, received.command, received.parameter);
}

//void SetVoltmeterGain(GAIN_SELECT gain)
void SetVoltmeterGain(const char* cmdParameter)
{
	switch(*cmdParameter - '0')
	{
		case GAIN_1:
			Scale_1Off();
			Scale_2Off();
			Scale_3Off();
			appData.gainSelected = 1.0;
			break;
		case GAIN_4:
			Scale_2Off();
			Scale_3Off();
			Scale_1On();
			appData.gainSelected = 4.0;
			break;
		case GAIN_16:
			Scale_1Off();
			Scale_3Off();
			Scale_2On();
			appData.gainSelected = 16.0;
			break;
		case GAIN_64:
			Scale_1Off();
			Scale_2Off();
			Scale_3On();
			appData.gainSelected = 64.0;
			break;
		default:
			break;
	}
}

void ReadVoltmeterValue(const char* cmdParameter)
{


	sprintf(sending.buffer, "ID%d%s%s", rs485Data.id, received.command, "50.00");
}

void CoolDownCallback()
{
	static uint8_t counterCooldown = 0;

	if(appData.coolDownActive == true)
		counterCooldown++;
	else
		counterCooldown = 0;
		appData.flagCooldownReached = false;

	if(counterCooldown >= COOLDOWN_TIME)
	{
		appData.flagCooldownReached = true;
		appData.coolDownActive = false;
		counterCooldown = 0;
	}
}

void StatusLEDCallback()
{
	static uint8_t counterStatusLed = 0;
	counterStatusLed++;

	if(counterStatusLed > STATUS_LED_BLINK_SPEED)
	{
		LED1Toggle();
		counterStatusLed = 0;
	}
}

void ErrorHandler()
{
	LED4On();
	while(1){};
}

void ADC_Callback()
{
	static uint8_t counterAdcScan = 0;
    float testFloat = (float)(GAIN_ATTENUATOR * GAIN_RESISTOR_DIVIDER);
	//float totalGainFixed = (float)(GAIN_ATTENUATOR * (float)appData.gainSelected * GAIN_RESISTOR_DIVIDER);
    float totalGainFixed = (testFloat * (float)appData.gainSelected);
	counterAdcScan++;

	if(counterAdcScan > ADC_SCAN_SPEED)
	{
		rawResult = ReadAllADC();

		appData.valueVoltmeterDc = ((rawResult.AN4*(V_REF/ADC_RESOLUTION)*(-1)-(-1.5))/totalGainFixed);
		//appData.valueVoltmeterAc =

		//AN4 = ((float)3/1024) * rawResult.AN4 - 1.5;
		//AN5 = ((float)3/1024) * rawResult.AN5 - 1.5;

		counterAdcScan = 0;
	}
}

/*******************************************************************************
 End of File
 */
