/*
 * signaling.c
 *
 *  Created on: Jul 12, 2022
 *      Author: Arek
 */

#include "signaling.h"



void signalingCallback(signaling_t *signaling){



}






void signalingInit(signaling_t *signaling){
	//I'd like to find an elegant way for passing name of the timer as an argument
	TIM3->PSC=(signaling->fclk)/(signaling->toneFrequency);
	TIM3->CCR4=(TIM3->PSC)/2;
	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}
