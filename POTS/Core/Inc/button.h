/*
 * button.h
 *
 *  Created on: Sep 22, 2023
 *      Author: Arek
 */

#include "stm32f1xx_hal.h"

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#define BUTTON_DEBOUNCE_TIME_MILISECONDS (100)

typedef enum
{
	BUTTON_STATE_OFF,
	BUTTON_STATE_ON
} button_requested_state_t;

typedef enum
{
	BUTTON_CURRENT_LEVEL_NOT_PROCESSED,
	BUTTON_CURRENT_LEVEL_PROCESED
} button_current_level_processed_t;

typedef enum
{
	BUTTON_ACTION_NO_FLAG,
	BUTTON_ACTION_FLAG_SET
} button_action_flag_t;


typedef struct button_t {
	GPIO_TypeDef *port_BUTTON;
	uint16_t pin_BUTTON;

	GPIO_PinState previousLevel;
	GPIO_PinState currentLevel;

	uint16_t stateDurationMilliseconds;
	uint16_t timeKeepingPeriodMilliseconds;

	button_requested_state_t buttonRequestedState;
	button_current_level_processed_t buttonCurrentLevelProcessed;

	button_action_flag_t actionFlag;

}button_t;

void buttonInit(button_t *myButton);
void buttonTimeKeeping(button_t *myButton);

#endif /* INC_BUTTON_H_ */
