/*
 * ringing.c
 *
 *  Created on: Jul 12, 2022
 *      Author: Arek
 */

#include "ringing.h"



/**
  * @brief  initialize ringing module
  * @param  ring: pointer to a structure of type ringer_t. This structure contains state of the ringing module
  * @retval None
  */
void ringInit(ringer_t *ring)
{
	ring->callBackCounterBurst=0;
	HAL_GPIO_WritePin(ring->FR_GPIO_Port, ring->FR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ring->RM_GPIO_Port, ring->RM_Pin, GPIO_PIN_RESET);
}


/**
  * @brief  callback for ringer module - This function must be called with frequency specified in ring->callBackFrequency
  * @param  ring: pointer to a structure of type ringer_t. This structure contains state of the ringing module
  * @retval None
  */
void ringCallback(ringer_t *ring)
{
	//generating ringing base frequency
	ring->callBackCounterBurst++;
	if(ring->callBackCounterBurst>((ring->callBackFrequency)/(2*(ring->burstFrequency)))) //two times because there are two slopes in one period, this way generated frequency will be really expressed in Hz
	{
		ring->callBackCounterBurst=0;
		ring->callBackCounterDuration++;
		switch(ring->state)
			{
				case RINGER_RINGING_BURST:
					if(ring->callBackCounterDuration<40)
					{
						HAL_GPIO_WritePin(ring->RM_GPIO_Port, ring->RM_Pin, GPIO_PIN_SET);// ringing mode should be already enabled here but let's make sure
						HAL_GPIO_TogglePin(ring->FR_GPIO_Port, ring->FR_Pin); //toggle FR pin - this generates a tone in the attached telephone

					}
					else
					{
						HAL_GPIO_WritePin(ring->FR_GPIO_Port, ring->FR_Pin, GPIO_PIN_RESET); //ensure FR pin goes back to its default state
						HAL_GPIO_WritePin(ring->RM_GPIO_Port, ring->RM_Pin, GPIO_PIN_RESET); //disable ringing mode in the KS0835 moduleHAL_GPIO_WritePin(ring->FR_GPIO_Port, ring->FR_Pin, GPIO_PIN_RESET);
						ring->callBackCounterDuration=0;
						ring->state=RINGER_RINGING_PAUSE;
					}
					break;
				case RINGER_RINGING_PAUSE:
					if(ring->callBackCounterDuration>80)
						{
							ring->callBackCounterDuration=0;
							ring->state=RINGER_RINGING_BURST;
							HAL_GPIO_WritePin(ring->RM_GPIO_Port, ring->RM_Pin, GPIO_PIN_SET);// enable ringing mode
						}
					break;
				case RINGER_OFF:
				default:
					HAL_GPIO_WritePin(ring->FR_GPIO_Port, ring->FR_Pin, GPIO_PIN_RESET); //ensure line goes into default polarity
					HAL_GPIO_WritePin(ring->RM_GPIO_Port, ring->RM_Pin, GPIO_PIN_RESET); //disable ringing mode in the KS0835 module
					ring->callBackCounterDuration=0;

					break;
			}



	}


	//generating burst duration and pause


}
