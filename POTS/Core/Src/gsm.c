/*
 * gsm.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Arek
 */

#include "gsm.h"

void gsmInit(gsm_t *myModule)
{
	snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "gsmInit\n");
	HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));

	//initialize structure
	myModule->state=GSM_MODULE_OFF;
	myModule->uart_RX_counter=0;
	myModule->servicePending=0;

	//pull the GSM_WAKE pin high to turn the module on
	HAL_GPIO_WritePin(myModule->port_GSM_WAKE, myModule->pin_GSM_WAKE, GPIO_PIN_SET);
}

void gsmUartReceiver(gsm_t *myModule, uint8_t receivedCharacter)
{
	if(myModule->uart_RX_counter<sizeof(myModule->uart_RX_buffer)){ //if there is space left in a receiver buffer
		myModule->uart_RX_buffer[myModule->uart_RX_counter]=receivedCharacter;
		myModule->uart_RX_counter++;
		myModule->servicePending=1;
	}
	else //no space for a new character - flush the buffer to avoid overrun
	{
		memset(myModule->uart_RX_buffer, '\0', sizeof(myModule->uart_RX_buffer));
		myModule->uart_RX_counter=0;
	}
}


void gsmService(gsm_t *myModule)
{

	if(myModule->servicePending){
		myModule->servicePending=0;

		if(myModule->uart_RX_buffer[myModule->uart_RX_counter-1]=='\n'){//if last receiverd character is a \n then a complete line is received

			//first string from GSM module expected after turning on
			if(strcmp((char*)myModule->uart_RX_buffer, "AT command ready\r\n") == 0){
				HAL_GPIO_WritePin(myModule->port_GSM_WAKE, myModule->pin_GSM_WAKE, GPIO_PIN_SET);  //module is turned on so release WAKE pin
				myModule->state=GSM_MODULE_AT_READY;
			}

			if(strcmp((char*)myModule->uart_RX_buffer, "+SIM READY\r\n") == 0){
				myModule->state=GSM_MODULE_SIM_READY;
			}

			if(strcmp((char*)myModule->uart_RX_buffer, "RING\r\n") == 0){
				myModule->state=GSM_MODULE_RING;
			}

//			snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "gsmService %d\n", myModule->uart_RX_counter);
//			HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));

			memset(myModule->uart_RX_buffer, '\0', sizeof(myModule->uart_RX_buffer));
			myModule->uart_RX_counter=0;

		}
	}
}

void gsmFrontPanel(gsm_t *myModule)
{

}
