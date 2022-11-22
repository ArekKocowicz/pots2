/*
 * gsm.h
 *
 *  Created on: Nov 21, 2022
 *      Author: Arek
 */

#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"

#ifndef INC_GSM_H_
#define INC_GSM_H_

typedef enum
{
	GSM_MODULE_OFF = 0u,
	GSM_MODULE_AT_READY,
	GSM_MODULE_SIM_READY,
	GSM_MODULE_RING
}gsm_module_state_t;

typedef struct gsm_t {
	GPIO_TypeDef *port_GSM_WAKE;		//Port on which GSM_WAKE pin is connected
	uint16_t pin_GSM_WAKE;				//Pin on which GSM_WAKE pin is connected
	UART_HandleTypeDef *huart;			//pointer to handler of associated UART
	gsm_module_state_t state;
	uint8_t uart_TX_buffer[16];

	uint8_t uart_RX_counter;
	uint8_t uart_RX_buffer[32];

	uint8_t servicePending;

}gsm_t;

void gsmInit(gsm_t *myModule);
void gsmUartReceiver(gsm_t *myModule, uint8_t receivedCharacter);
void gsmService(gsm_t *myModule);
void gsmFrontPanel(gsm_t *myModule);

#endif /* INC_GSM_H_ */
