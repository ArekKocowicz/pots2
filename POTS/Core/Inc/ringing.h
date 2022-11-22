/*
 * ringing.h
 *
 *  Created on: Jul 12, 2022
 *      Author: Arek
 */

#include "stm32f1xx_hal.h"

#ifndef INC_RINGING_H_
#define INC_RINGING_H_

#define RINGING_BURST_FREQUENCY (25)

typedef enum
{
	RINGER_OFF,
	RINGER_RINGING_BURST,
	RINGER_RINGING_PAUSE
} ringer_state_t;


typedef struct ringer_t {
	uint16_t callBackFrequency; //callback frequency in [Hz]

	GPIO_TypeDef *FR_GPIO_Port;		//Port on which FR pin is connected
	uint16_t FR_Pin;				//Pin on which FR pin is connected
	GPIO_TypeDef *RM_GPIO_Port;		//Port on which RM pin is connected
	uint16_t RM_Pin;				//Pin on which RM pin is connected

	uint16_t callBackCounterBurst;      //callback counter for generating burst base frequency [Hz]
	uint16_t callBackCounterDuration_milliseconds;   //callback counter for keeping burst on and off duration [ms]

	ringer_state_t state;

}ringer_t;

void ringFrontPanel(ringer_t *ring, uint8_t ringing);
void ringCallback(ringer_t *ring);
void ringInit(ringer_t *ring);

#endif /* INC_RINGING_H_ */
