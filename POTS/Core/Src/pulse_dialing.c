/*
 * pulse_dialing.c
 *
 *  Created on: 01.08.2022
 *      Author: Arek
 */

#include "pulse_dialing.h"

/**
  * @brief  initialize pulse dialing module
  * @param  pulseDialing: pointer to a structure of type pulse_dialing_machine_t. This structure contains state of the ringing module
  * @retval None
  */
void pulseDialingInit(pulse_dialing_machine_t *pulseDialing)
{
	pulseDialing->Handset_State=HANDSET_ON_HOOK;
	pulseDialing->counter=0;
	pulseDialing->timeSinceLastRisingSlope_milliseconds=0;
	pulseDialing->time_SHK_staticLevelDuration_milliseconds=0;
	pulseDialing->dialedDigit=-1;
	pulseDialing->newEventHandset=0;
}


/**
  * @brief  cyclically called function which handles detection of handset pick up and put back on hook and dialing digits
  * @param  pulseDialing: pointer to a structure of type pulse_dialing_machine_t. This structure contains state of the ringing module
  * @retval None
  * see //http://www.gordostuff.com/2011/08/telephone-set-function-2-to-provide.html for details of pulse dialing
  */
void pulseDialingCallback(pulse_dialing_machine_t *pulseDialing)
{
	GPIO_PinState SHK_CurrentLevel; 														//this is SHK signal level observed in current invocation of the callback

	SHK_CurrentLevel=HAL_GPIO_ReadPin(pulseDialing->SHK_GPIO_Port, pulseDialing->SHK_Pin);	//read in current SHK level
	//and recognize if this is continuously HIGH, LOW, rising or falling slope
	if(SHK_CurrentLevel == GPIO_PIN_SET){
		if(pulseDialing->SHK_PreviousLevel == GPIO_PIN_SET)
			pulseDialing->SHK_State=SHK_HIGH;
		else
			pulseDialing->SHK_State=SHK_SLOPE_RISING;
	}
	else{
		if(pulseDialing->SHK_PreviousLevel == GPIO_PIN_SET)
			pulseDialing->SHK_State=SHK_SLOPE_FALLING;
		else
			pulseDialing->SHK_State=SHK_LOW;
	}

	//currentLevel is stored for next invocation of this function
	pulseDialing->SHK_PreviousLevel=SHK_CurrentLevel;

	//zero timer measuring duration of a static SHK state
	if(pulseDialing->SHK_State==SHK_SLOPE_FALLING || pulseDialing->SHK_State==SHK_SLOPE_RISING){
		pulseDialing->time_SHK_staticLevelDuration_milliseconds=0;
	}
	else{ //measure static SHK state duration
		pulseDialing->time_SHK_staticLevelDuration_milliseconds+=(1000/pulseDialing->frequencyCallback_hertz);
	}

	//recognize lifting the handset and putting it back on hook
	if(pulseDialing->SHK_State==SHK_HIGH
		&& pulseDialing->time_SHK_staticLevelDuration_milliseconds>=PULSE_DIALING_HANDSET_QUALIFICATION_MILLISECONDS
		&& pulseDialing->Handset_State==HANDSET_ON_HOOK){
			pulseDialing->Handset_State=HANDSET_LIFTED;
			pulseDialing->newEventHandset=1;
	}
	else if(pulseDialing->SHK_State==SHK_LOW
			&& pulseDialing->time_SHK_staticLevelDuration_milliseconds>=PULSE_DIALING_HANDSET_QUALIFICATION_MILLISECONDS
			&& pulseDialing->Handset_State==HANDSET_LIFTED){
				pulseDialing->Handset_State=HANDSET_ON_HOOK;
				pulseDialing->newEventHandset=1;
		}

	//if handset is lifted
	if(pulseDialing->Handset_State==HANDSET_LIFTED){
		//recognize dialing numbers
		if(pulseDialing->SHK_State==SHK_SLOPE_RISING){
			pulseDialing->counter++;
			pulseDialing->timeSinceLastRisingSlope_milliseconds=0;
		}

		if(pulseDialing->SHK_State==SHK_HIGH && pulseDialing->counter){
			pulseDialing->timeSinceLastRisingSlope_milliseconds+=(1000/pulseDialing->frequencyCallback_hertz);
			if(pulseDialing->timeSinceLastRisingSlope_milliseconds>PULSE_DIALING_DIGIT_QUALIFICATION_MILLISECONDS){
				//a digit has been completely dialed and is stored in counter here
				//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
				pulseDialing->dialedDigit=pulseDialing->counter;
				pulseDialing->counter=0;
				pulseDialing->timeSinceLastRisingSlope_milliseconds=0;
			}
		}
	}
}
