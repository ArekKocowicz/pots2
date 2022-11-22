/*
 * debug_uart.h
 *
 *  Created on: Nov 19, 2022
 *      Author: Arek
 */

#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"

#ifndef INC_DEBUG_UART_H_
#define INC_DEBUG_UART_H_


UART_HandleTypeDef huart1;
char buffer[128];


void debug_message(char *message);

#endif /* INC_DEBUG_UART_H_ */
