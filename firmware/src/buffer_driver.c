//-----------------------------------------------------------
// File Name    : buffer_driver.c	                        |
// Project Name : 2313_instrlabomodulaire                   |
// Version      : V1                                        |
// Date         : 22.09.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------
#include "buffer_driver.h"

extern RS485_DATA rs485Data;

S_SOFTWARE_BUFFER softwareBuffer;

void GetByteFromUsartCallback()
{
	while(DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT & DRV_USART_TransferStatus(rs485Data.usartHandle))
	{
		data = DRV_USART_ReadByte(rs485Data.usartHandle);
		if(softwareBuffer.index < SOFTWARE_BUFFER_SIZE)
		{
			softwareBuffer.buffer = data;
		}
	}

	if(softwareBuffer.index >= SOFTWARE_BUFFER_SIZE)
	{
		//Handle full software buffer error
	}
}

bool GetMessageFromBufferSoft(char* returnBuffer, int length)
{
	bool hasBufferData = false;
	int bytesRead = 0;

	while(bytesRead < length && bytesRead < softwareBuffer.index)
	{
		returnBuffer[bytesRead++] = softwareBuffer.buffer[bytesRead];
	}

	softwareBuffer.index -= bytesRead;
	return bytesRead;

	return hasBufferData;
}