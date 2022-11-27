/*
 * gsm.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Arek
 */

#include "gsm.h"

void gsmInit(gsm_t *myModule)
{
	//snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "gsmInit\n");
	//HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));

	//initialize structure
	myModule->logicStateDurationMillisecon=0;
	gsmPowerStateChange(myModule, GSM_POWER_INIT);
	gsmLogicStateChange(myModule, GSM_MODULE_OFF);
	myModule->uart_RX_counter=0;
	myModule->servicePending=0;

	//pull the GSM_WAKE pin high to turn the module on
	//HAL_GPIO_WritePin(myModule->port_GSM_WAKE, myModule->pin_GSM_WAKE, GPIO_PIN_SET);
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

void gsmLogicStateChange(gsm_t *myModule, gsm_module_logic_state_t newState)
{
	myModule->logicState=newState;
	myModule->logicStateDurationMillisecon=0;
}

void gsmPowerStateChange(gsm_t *myModule, gsm_module_power_state_t newState)
{
	myModule->powerState=newState;
	myModule->powerStateDurationMillisecon=0;
}

void gsmService(gsm_t *myModule) //this will be called in main
{
	myModule->LPG_PinState=HAL_GPIO_ReadPin(myModule->port_GSM_LPG, myModule->pin_GSM_LPG);

	switch(myModule->powerState){
	case GSM_POWER_INIT:
		if(myModule->LPG_PinState==GPIO_PIN_SET){ //if LPG high then the module is turned on
			gsmPowerStateChange(myModule, GSM_POWER_TURNED_ON);
		}
		if(myModule->powerStateDurationMillisecon>4000){ //no HIGH state observed for 4 seconds means that the module must be turned on
			gsmPowerStateChange(myModule, GSM_POWER_ASSERTING_POWER_ON);
		}
		break;
	case GSM_POWER_ASSERTING_POWER_ON:
		HAL_GPIO_WritePin(myModule->port_GSM_POWER_ON, myModule->pin_GSM_POWER_ON, GPIO_PIN_SET); //keep the POWER_ON pin asserted until module turns on
		if(myModule->LPG_PinState==GPIO_PIN_SET){ //if LPG high then the module is turned on
			gsmPowerStateChange(myModule, GSM_POWER_TURNED_ON);
		}
		break;

	case GSM_POWER_TURNED_ON:
		HAL_GPIO_WritePin(myModule->port_GSM_POWER_ON, myModule->pin_GSM_POWER_ON, GPIO_PIN_RESET);

		break;
	}



	if(myModule->servicePending){
		myModule->servicePending=0;

		if(myModule->uart_RX_buffer[myModule->uart_RX_counter-1]=='\n'){//if last receiverd character is a \n then a complete line is received

			//first string from GSM module expected after turning on
			if(strcmp((char*)myModule->uart_RX_buffer, "AT command ready\r\n") == 0){
				HAL_GPIO_WritePin(myModule->port_GSM_POWER_ON, myModule->pin_GSM_POWER_ON, GPIO_PIN_RESET);  //module is turned on so release WAKE pin
				myModule->logicState=GSM_MODULE_AT_READY;
			}

			if(strcmp((char*)myModule->uart_RX_buffer, "+SIM READY\r\n") == 0){
				myModule->logicState=GSM_MODULE_SIM_READY;
			}

			if(strcmp((char*)myModule->uart_RX_buffer, "RING\r\n") == 0){
				myModule->logicState=GSM_MODULE_RING;
			}

//			snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "gsmService %d\n", myModule->uart_RX_counter);
//			HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));

			//memset(myModule->uart_RX_buffer, '\0', sizeof(myModule->uart_RX_buffer));
			myModule->uart_RX_counter=0;

		}
	}
}

void gsmTimeKeeping(gsm_t *myModule)  //this will be called in timer interrupt
{
	if(myModule->powerStateDurationMillisecon<(UINT16_MAX-myModule->timeKeepingPeriodMilliseconds)){
		myModule->powerStateDurationMillisecon+=myModule->timeKeepingPeriodMilliseconds;
	}

	if(myModule->logicStateDurationMillisecon<(UINT16_MAX-myModule->timeKeepingPeriodMilliseconds)){
			myModule->logicStateDurationMillisecon+=myModule->timeKeepingPeriodMilliseconds;
		}
}

void gsmFrontPanel(gsm_t *myModule)
{

}
