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
	SIGNALING_INTERNAL_STATE_OFF,
	SIGNALING_INTERNAL_STATE_CONTINUOUS,
	SIGNALING_INTERNAL_STATE_TONE,
	SIGNALING_INTERNAL_STATE_PAUSE
} signaling_internal_state_t;




typedef struct signaling_t {
	uint32_t frequencyCallback_hertz; //callback frequency in [Hz]
	uint32_t fclk_hertz;				//timer clock frequency

	uint16_t toneFrequency_hertz;    //required frequency of a tone burst in Hz
	uint16_t toneOnDuration_milliseconds;   //burst package duration in not yet defined unit
	uint16_t toneOffDuration_milliseconds;  //pause between bursts packages in not yet defined unit

	uint16_t currentStateDuration_milliseconds;   //callback counter for tone duration
	signaling_internal_state_t state;

	TIM_HandleTypeDef *timer;

}signaling_t;

void signalingFrontPanel(signaling_t *signaling, uint8_t pattern);
void signalingCallback(signaling_t *ring);
void signalingInit(signaling_t *ring);


#endif /* INC_SIGNALING_H_ */
