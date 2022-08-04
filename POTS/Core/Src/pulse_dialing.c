/*
 * pulse_dialing.c
 *
 *  Created on: 01.08.2022
 *      Author: Arek
 */

#include "pulse_dialing.h"

/**
  * @brief  initialize ringing module
  * @param  pulseDIaling: pointer to a structure of type pulse_dialing_machine_t. This structure contains state of the ringing module
  * @retval None
  */
void pulseDialingInit(pulse_dialing_machine_t *pulseDialing)
{

}


//http://www.gordostuff.com/2011/08/telephone-set-function-2-to-provide.html


void pulseDialingCallback(pulse_dialing_machine_t *pulseDialing)
{
	GPIO_PinState SHK_CurrentLevel; //this is SHK signal level observed in current invocation of the callback
	SHK_state_t SHK_state;		    //this considers previous SHK level and the current SHK level and tells if ther is a slope or a constant level

	SHK_CurrentLevel=HAL_GPIO_ReadPin(pulseDialing->SHK_GPIO_Port, pulseDialing->SHK_Pin);

}
