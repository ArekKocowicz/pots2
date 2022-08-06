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
	pulseDialing->counter=0;
	pulseDialing->timeSinceLastRisingSlope=0;
	pulseDialing->dialedDigit=-1;
}


//http://www.gordostuff.com/2011/08/telephone-set-function-2-to-provide.html


void pulseDialingCallback(pulse_dialing_machine_t *pulseDialing)
{
	GPIO_PinState SHK_CurrentLevel; //this is SHK signal level observed in current invocation of the callback

	//read in current SHK level
	SHK_CurrentLevel=HAL_GPIO_ReadPin(pulseDialing->SHK_GPIO_Port, pulseDialing->SHK_Pin);
	//and recognize if this is continuously HIGH LOW or a slope
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

	//here we know if this was rising or falling slope
	if(pulseDialing->SHK_State==SHK_SLOPE_RISING){
		pulseDialing->counter++;
		pulseDialing->timeSinceLastRisingSlope=0;
	}
	if(pulseDialing->SHK_State==SHK_HIGH && pulseDialing->counter){
		pulseDialing->timeSinceLastRisingSlope++;
		if(pulseDialing->timeSinceLastRisingSlope>200){
			//a digit has been completely dialed and is stored in counter here
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			pulseDialing->dialedDigit=pulseDialing->counter;
			pulseDialing->counter=0;
			pulseDialing->timeSinceLastRisingSlope=0;
		}
	}


	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	//now use that information about slope...
	/////////////
	/////////////
	/////////////



	//currentLevel is stored for next invocation of this function
	pulseDialing->SHK_PreviousLevel=SHK_CurrentLevel;
}
