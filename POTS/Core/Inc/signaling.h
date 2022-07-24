/*
 * signaling.h
 *
 *  Created on: 24.07.2022
 *      Author: Arek
 */

#include "stm32f1xx_hal.h"

#ifndef INC_SIGNALING_H_
#define INC_SIGNALING_H_

typedef enum
{
	SIGNALING_STATE_OFF,
	SIGNALING_STATE_TONE,
	SIGNALING_STATE_PAUSE
} signaling_state_t;


typedef struct signaling_t {
	uint32_t callBackFrequency; //callback frequency in [Hz]

	uint16_t toneFrequency;    //required frequency of a call burst in Hz
	uint16_t toneOnDuration;   //burst package duration in not yet defined unit
	uint16_t toneOffDuration;  //pause between bursts packages in not yet defined unit

	uint16_t callBackCounterDuration;   //callback counter for tone duration
	signaling_state_t state;

	TIM_HandleTypeDef *timer;

}signaling_t;

void signalingCallback(signaling_t *ring);
void signalingInit(signaling_t *ring);


#endif /* INC_SIGNALING_H_ */
