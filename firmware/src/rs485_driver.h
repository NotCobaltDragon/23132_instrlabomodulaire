//-----------------------------------------------------------
// File Name    : rs485_driver.h	                        |
// Project Name : 2313_instrlabomodulaire                   |
// Version      : V1                                        |
// Date         : 13.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#ifndef RS485_DRIVER_H	/* Guard against multiple inclusion */
#define RS485_DRIVER_H
#include <stdio.h>
#include <string.h>
#include "system_definitions.h"

#define DEFAULT_RS485_MODE RECEIVE

#define MAX_NB_COMMANDS 100
#define MAX_NB_MODULES 15

#define RX_TX_BUFFER_SIZE 16

//#define LAST_CMD_CHAR '~'

#define SENDING false
#define RECEIVING true

#define MESSAGE_TIMEOUT 10 //unit in [ms]

typedef struct
{
	char command[5];	// 4 characters for the cmd + '\0' for NULL
	void (*cmdFunctionPtr)(const char* cmdParameter);	//Function pointer
}COMMAND_MAPPING;

typedef struct
{
	char parameter[9];
	void(*moduleTagPtr);
}MODULE_MAPPING;

typedef enum
{
	ID_1 = 1,
	ID_2,
	ID_3,
	ID_4,
	ID_5,
	ID_6,
	ID_7,
}E_ID_MODULES;

typedef struct
{
	uint8_t id;
	char command[5];
	char parameter[9];
	char buffer[RX_TX_BUFFER_SIZE];
}RX_TX_DATA;

typedef struct
{
	DRV_HANDLE usartHandle;
	bool selectedDirection;
	bool isResponseTimeoutReached;
	uint8_t messageDataTimeout;
	E_ID_MODULES id;
	uint8_t parameter;
}RS485_DATA;

bool Init_RS485(bool defaultMode);

bool SendMessage(char txBuffer[8]);

bool GetMessage(char* rxBuffer);

bool RegisterCommand(const char* command, void (*functionPtr)(const char* cmdParameter));

bool IdChecker(uint8_t idToCheck, uint8_t id);

uint8_t ExtractId(char *rxBuffer);

uint8_t ExtractCommand(char *rxBuffer);

uint8_t ExtractParameter(char *rxBuffer);

void ClearBuffer(char* buffer);

void MessageDataTimeoutReset(void);

void MessageDataTimeoutCallback(void);

void RS485_Direction_Mode(bool directionMode);

void RS485_Sending_Mode(void);

void RS485_Receiving_Mode(void);

uint8_t GetID(void);

#endif