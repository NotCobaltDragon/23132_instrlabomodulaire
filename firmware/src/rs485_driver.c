//-----------------------------------------------------------
// File Name    : rs485_driver.c	                        |
// Project Name : 2313_instrlabomodulaire                   |
// Version      : V1                                        |
// Date         : 13.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#include "rs485_driver.h"

#define MESS_SIZE

RS485_DATA rs485Data;

COMMAND_MAPPING commandMapping[MAX_NB_COMMANDS];

MODULE_MAPPING moduleMapping[MAX_NB_MODULES];


bool Init_RS485(bool defaultDirection)
{
	//PLIB_USART_ReceiverInterruptModeSelect(USART_ID_1, USART_RECEIVE_FIFO_HALF_FULL);
	RS485_Direction_Mode(defaultDirection);
	rs485Data.selectedDirection = defaultDirection;

	rs485Data.usartHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_NONBLOCKING);
    
	if(rs485Data.usartHandle == DRV_HANDLE_INVALID)
		return false; //UASRT isn't opened
	else
		return true; //USART opened
}

//Funcion to register a command and its associated function
bool RegisterCommand(const char* command, void (*functionPtr)(const char* cmdParameter))
{
	static uint16_t numCommands = 0;

	if(numCommands < MAX_NB_COMMANDS)
	{
		strcpy(commandMapping[numCommands].command, command);	
		commandMapping[numCommands].cmdFunctionPtr = functionPtr;
		numCommands++;
		return true;	//Cmd created correctly
	}
	else
	{
		return false;	//Number of commands created over the limit
	}
}

bool RegisterModule(const char* moduleTag, void(*functionPtr))
{
	static uint8_t numModules = 0;

	if(numModules < MAX_NB_MODULES)
	{
		strcpy(moduleMapping[numModules].parameter, moduleTag);
		moduleMapping[numModules].moduleTagPtr = functionPtr;
		numModules++;
		return true;	//Module Registered
	}
	else
	{
		return false;	//Number of possible modules exeeded (should never happen)
	}
}

bool SendMessage(char txBuffer[RX_TX_BUFFER_SIZE])
{
	//bool needSendCommand = true;
	int nbByteWritten = 0;
	uint8_t bufferSize = strlen(txBuffer);

	while(nbByteWritten < bufferSize)
	{
		if(!DRV_USART_TransmitBufferIsFull(rs485Data.usartHandle))
			DRV_USART_WriteByte(rs485Data.usartHandle, txBuffer[nbByteWritten++]);
	}
	while(!(DRV_USART_TRANSFER_STATUS_TRANSMIT_EMPTY & DRV_USART_TransferStatus(rs485Data.usartHandle))){}
	return false;
}

bool GetMessage(char* rxBuffer)
{
	int nbByteReceived = 0;
	MessageDataTimeoutReset();
	do
	{
		if(DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT & DRV_USART_TransferStatus(rs485Data.usartHandle))
		{
			rxBuffer[nbByteReceived++] = DRV_USART_ReadByte(rs485Data.usartHandle);
			MessageDataTimeoutReset();
		}
	}while((nbByteReceived < 8) && (rs485Data.isResponseTimeoutReached != true));
	//if(rs485Data.isResponseTimeoutReached)
	return true;
}

bool IdChecker(uint8_t idToCheck, uint8_t id)
{
	if(idToCheck == id)
		return true;	//Correct ID
	else
		return false;	//ID incorrect or not concerned
}

void ClearBuffer(char* buffer)
{
	char dummy __attribute__((unused));
	uint8_t clearBufferCounter = 0;

	while(DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT & DRV_USART_TransferStatus(rs485Data.usartHandle))
	{
		dummy = DRV_USART_ReadByte(rs485Data.usartHandle);
	}
	for (clearBufferCounter = 0; clearBufferCounter < RX_TX_BUFFER_SIZE; clearBufferCounter++)
	{
		buffer[clearBufferCounter] = '\0';
	}
}

void ClearReceiveBuffer()
{
	
}

void MessageDataTimeoutReset()
{
	rs485Data.messageDataTimeout = 0;
	rs485Data.isResponseTimeoutReached = false;
}

void MessageDataTimeoutCallback()
{
	rs485Data.messageDataTimeout++;

	if(rs485Data.messageDataTimeout >= MESSAGE_TIMEOUT)
		rs485Data.isResponseTimeoutReached = true;
}

void RS485_Sending_Mode()
{
	RS485_RE_On();
	RS485_DE_On();
}

void RS485_Receiving_Mode()
{
	RS485_RE_Off();
	RS485_DE_Off();
}

void RS485_Direction_Mode(bool directionMode)
{
	if(directionMode == SENDING)
	{
		RS485_Sending_Mode();
	}
	else if(directionMode == RECEIVING)
	{
		RS485_Receiving_Mode();
	}
}


//void PutCharInFifo
//{
//
//}
//
//
//void GetCharFromFifo()
//{
//	
//}


