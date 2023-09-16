//-----------------------------------------------------------
// File Name    : rs485_commands.h	                        |
// Project Name : 2313_instrlabomodulaire                   |
// Version      : V1                                        |
// Date         : 13.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#ifndef RS485_COMMANDS_H	/* Guard against multiple inclusion */
#define RS485_COMMANDS_H

#include <stdio.h>
#include <string.h>

#define NB_CMD 5 	//Should match the umber of enumerations bellow
#define NB_CHAR_CMD 4 //Nb caracter + 1

extern const char cmdData[NB_CMD][NB_CHAR_CMD];

extern const char nameModulesData[2][12];	//11 is the maximum characters on screen + 1

//Voltmeter 23132 specific commands
typedef enum
{
	E_CMD_ERROR = 0,
	E_CMD_IDQUESTION = 1,
	E_CMD_VOLTMGAIN,
	E_CMD_VOLTMMODE,
	E_CMD_VOLTMREAD,
}E_Command;

#endif