//-----------------------------------------------------------
// File Name    : buffer_driver.h	                        |
// Project Name : 2313_instrlabomodulaire                   |
// Version      : V1                                        |
// Date         : 22.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "rs485_driver.h"

#define USART_BUFFER_SIZE 8	//Value in bytes
#define SOFTWARE_BUUFER_SIZE 16

typedef struct
{
	uint16_t index;
	char buffer[SOFTWARE_BUUFER_SIZE];
}S_SOFTWARE_BUFFER;