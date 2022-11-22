/*
 * debug_uart.c
 *
 *  Created on: Nov 19, 2022
 *      Author: Arek
 */

#include "debug_uart.h"


void debug_message(char *message)
{
	snprintf(buffer, sizeof(buffer), "debug %s\n", message);
	HAL_UART_Transmit_IT(&huart1, buffer, strlen(buffer));
}


