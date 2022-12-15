/*
 * gsm.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Arek
 */

#include "gsm.h"

void gsmInit(gsm_t *myModule)
{
	//initialize structure
	myModule->logicStateDurationMillisecon=0;
	gsmPowerStateChange(myModule, GSM_POWER_INIT);
	gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_INIT);
	myModule->uart_RX_counter=0;
//	myModule->servicePending=0;
}

void gsmUartReceiver(gsm_t *myModule, uint8_t receivedCharacter)
{
	if(myModule->uart_RX_counter<sizeof(myModule->uart_RX_buffer)-1){ 		//if there is space left in a receiver buffer ( -1, because we will ad \0 at the end
		myModule->uart_RX_buffer[myModule->uart_RX_counter]=receivedCharacter;
		myModule->uart_RX_counter++;
//		myModule->servicePending=1;
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
	case GSM_POWER_TURNING_OFF:
		snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "AT+MRST\r\n");
		HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));
		gsmPowerStateChange(myModule, GSM_POWER_TURNED_OFF);
		break;
	case GSM_POWER_TURNED_OFF:

		break;
	}


//	if(myModule->servicePending){
//		myModule->servicePending=0;

		if(myModule->uart_RX_buffer[myModule->uart_RX_counter-1]=='\n'){	//if last received character is a \n then a complete line is received
			myModule->uart_RX_buffer[myModule->uart_RX_counter]='\0';					//append \0 so we can treat it as a string

			//first string from GSM module expected after turning on
			if(strcmp((char*)myModule->uart_RX_buffer, "AT command ready\r\n") == 0){
				gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_AT_READY);

			}

			if(strcmp((char*)myModule->uart_RX_buffer, "+SIM READY\r\n") == 0){
				gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_SIM_READY);

			}

			if(strcmp((char*)myModule->uart_RX_buffer, "RING\r\n") == 0){
				gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_RING);

			}

			if(strcmp((char*)myModule->uart_RX_buffer, "NO CARRIER\r\n") == 0){
				gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_SIM_READY);

			}


			myModule->uart_RX_counter=0;

		}
//	}
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

void gsmAnswerIncomingCall(gsm_t *myModule)
{
	if(myModule->logicState==GSM_LOGIC_MODULE_RING)
	{
		gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_CALL_ONGOING);
		snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "ATA\r\n");
		HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));
	}
}

void gsmEndCall(gsm_t *myModule)
{
	if(myModule->logicState==GSM_LOGIC_MODULE_CALL_ONGOING)
	{
		gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_SIM_READY);
		snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "ATH\r\n");
		HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));
	}
}

void gsmStartCall(gsm_t *myModule, char *number)
{
	gsmLogicStateChange(myModule, GSM_LOGIC_MODULE_CALL_ONGOING);
	snprintf(myModule->uart_TX_buffer, sizeof(myModule->uart_TX_buffer), "ATD%s;\r\n", number);
	HAL_UART_Transmit_IT(myModule->huart, myModule->uart_TX_buffer, strlen(myModule->uart_TX_buffer));
}

