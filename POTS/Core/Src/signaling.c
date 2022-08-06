/*
 * signaling.c
 *
 *  Created on: Jul 12, 2022
 *      Author: Arek
 */

#include "signaling.h"



void signalingCallback(signaling_t *signaling){
	signaling->callBackCounterDuration++;

	switch(signaling->state)
	{
	case SIGNALING_STATE_TONE: //in this state a tone is beeing generated
		if(signaling->callBackCounterDuration>=signaling->toneOnDuration){ //if the tone has been generated long enough
			HAL_TIM_PWM_Stop(signaling->timer, TIM_CHANNEL_4); 	//stop generating tone
			signaling->state=SIGNALING_STATE_PAUSE;				//change state to SIGNALING_STATE_PASUE
			signaling->callBackCounterDuration=0; 				//reset counter
			}

		break;
	case SIGNALING_STATE_PAUSE:
		if(signaling->callBackCounterDuration>=signaling->toneOffDuration){ //if the pause has been generated long enough
			HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4); 	//start generating tone
			signaling->state=SIGNALING_STATE_TONE;				//change state to SIGNALING_STATE_PASUE
			signaling->callBackCounterDuration=0; 				//reset counter
			}
		break;
	case SIGNALING_STATE_OFF:
	default:
		signaling->callBackCounterDuration=0;
		break;
	}
}


//currently this function assumes that PWM channel 4 is used
//it is not reconfigurable on the fly for other PWM channels
void signalingInit(signaling_t *signaling){

	uint32_t psc; //psc is prescaler value needed for PWM channel to generate required frequency
	uint16_t ccr; //ccr is defining duty cycle of generated PWM

	psc=(signaling->fclk)/(1023*(signaling->toneFrequency));	//calculate necessary prescaler value
	if(psc<UINT16_MAX){ //timer can accept only 16 bit results
		ccr=512;
		signaling->timer->Instance->PSC=psc;
		signaling->timer->Instance->CCR4=ccr; //50% duty cycle of generated square signal
		HAL_TIM_PWM_Start(signaling->timer, TIM_CHANNEL_4);
		//return 0;
	}
	//return 1;

}
