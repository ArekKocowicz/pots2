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
	GSM_MODULE_LPG_ON,
	GSM_MODULE_AT_READY,
	GSM_MODULE_SIM_READY,
	GSM_MODULE_RING,
	GSM_MODULE_REQUEST_TURNING_OFF

}gsm_module_logic_state_t;

typedef enum
{
	GSM_POWER_INIT = 0u,  					//remain in this state 4 seconds and check if GSM_LPG indicates, that the module is on
	GSM_POWER_ASSERTING_POWER_ON,			//pull POWER_ON pin of the module down for 1 second
	GSM_POWER_TURNED_ON						//module turned on

}gsm_module_power_state_t;

typedef struct gsm_t {
	GPIO_TypeDef *port_GSM_POWER_ON;		//Port on which GSM_WAKE pin is connected
	uint16_t pin_GSM_POWER_ON;				//Pin on which GSM_WAKE pin is connected

	GPIO_TypeDef *port_GSM_LPG;			//Port on which GSM_LPG pin is connected
	uint16_t pin_GSM_LPG;				//Pin on which GSM_LPG pin is connected



	UART_HandleTypeDef *huart;			//pointer to handler of associated UART

	uint16_t timeKeepingPeriodMilliseconds; //what is the time period between calling time keeping routine

	gsm_module_logic_state_t logicState;
	uint16_t logicStateDurationMillisecon;	//


	gsm_module_power_state_t powerState;
	uint16_t powerStateDurationMillisecon;
	GPIO_PinState LPG_PinState;

	uint8_t uart_TX_buffer[16];

	uint8_t uart_RX_counter;
	uint8_t uart_RX_buffer[32];

	uint8_t servicePending;



}gsm_t;

void gsmInit(gsm_t *myModule);
void gsmUartReceiver(gsm_t *myModule, uint8_t receivedCharacter);
void gsmPowerStateChange(gsm_t *myModule, gsm_module_power_state_t newState);
void gsmLogicStateChange(gsm_t *myModule, gsm_module_logic_state_t newState);
void gsmService(gsm_t *myModule);
void gsmTimeKeeping(gsm_t *myModule);
void gsmFrontPanel(gsm_t *myModule);

#endif /* INC_GSM_H_ */
