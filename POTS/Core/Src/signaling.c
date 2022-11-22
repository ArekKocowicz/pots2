/*
 * signaling.c
 *
 *  Created on: Jul 12, 2022
 *      Author: Arek
 */

#include "signaling.h"


/**
  * @brief  "public" interface for changing signalling pattern
  * @param  signaling: pointer to a structure of type signaling_t.
  * @param pattern - requested pattern: 0 for no tone, 1 for continuous tone, 2 for dialing tone etc...
  * @retval None
  */
void signalingFrontPanel(signaling_t *signaling, uint8_t pattern){
	static uint8_t previousRequestedPattern=0;

	if(pattern!=previousRequestedPattern){
		switch(pattern)
			{
			case 0: // quiet
				signaling->state=SIGNALING_INTERNAL_STATE_OFF;
				HAL_TIM_PWM_Stop(signaling->timer, TIM_CHANNEL_4); 				//stop generating tone
				break;
			case 1: // continuous tone = line ready
				signaling->state=SIGNALING_INTERNAL_STATE_CONTINUOUS;
				HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4);
				break;
			case 2: // pulsed tone = number dialing
				signaling->toneOnDuration_milliseconds=200;
				signaling->toneOffDuration_milliseconds=200;
				signaling->state=SIGNALING_INTERNAL_STATE_TONE;
				HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4);
				break;
			case 3: // ringing
				signaling->toneOnDuration_milliseconds=1000;
				signaling->toneOffDuration_milliseconds=2000;
				signaling->state=SIGNALING_INTERNAL_STATE_TONE;
				HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4);
				break;
			default:

				break;
			}
	}

	previousRequestedPattern=pattern;
}

void signalingCallback(signaling_t *signaling){
	signaling->currentStateDuration_milliseconds+=1000/signaling->frequencyCallback_hertz;

	switch(signaling->state)
	{
	case SIGNALING_INTERNAL_STATE_TONE: //in this state a tone is being generated
		if(signaling->currentStateDuration_milliseconds>=signaling->toneOnDuration_milliseconds){ //if the tone has been generated long enough
			HAL_TIM_PWM_Stop(signaling->timer, TIM_CHANNEL_4); 				//stop generating tone
			signaling->state=SIGNALING_INTERNAL_STATE_PAUSE;							//change state to SIGNALING_STATE_PASUE
			signaling->currentStateDuration_milliseconds=0; 				//reset counter
			}

		break;
	case SIGNALING_INTERNAL_STATE_PAUSE:
		if(signaling->currentStateDuration_milliseconds>=signaling->toneOffDuration_milliseconds){ //if the pause has been generated long enough
			HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4); 			//start generating tone
			signaling->state=SIGNALING_INTERNAL_STATE_TONE;							//change state to SIGNALING_STATE_PASUE
			signaling->currentStateDuration_milliseconds=0; 				//reset counter
			}
		break;
	case SIGNALING_INTERNAL_STATE_CONTINUOUS:
	case SIGNALING_INTERNAL_STATE_OFF:
	default:
		signaling->currentStateDuration_milliseconds=0;
		break;
	}
}


//currently this function assumes that PWM channel 4 is used
//it is not reconfigurable on the fly for other PWM channels
void signalingInit(signaling_t *signaling){

	uint32_t psc; //psc is prescaler value needed for PWM channel to generate required frequency
	uint16_t ccr; //ccr is defining duty cycle of generated PWM

	psc=(signaling->fclk_hertz)/(1023*(signaling->toneFrequency_hertz));	//calculate necessary prescaler value
	if(psc<UINT16_MAX){ //timer can accept only 16 bit results
		ccr=512;
		signaling->timer->Instance->PSC=psc;
		signaling->timer->Instance->CCR4=ccr; //50% duty cycle of generated square signal
		HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4);
		//return 0;
	}
	//return 1;
}
