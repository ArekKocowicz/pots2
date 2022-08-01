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
	pulseDialing->SHK_CurrentLevelDurationMiliseconds=0;
	pulseDialing->SHK_PreviousLevelDurationMiliseconds=0;
	pulseDialing->SHK_PreviousLevel=GPIO_PIN_RESET;
	pulseDialing->currentDigitCounter=0;

	memset(pulseDialing->dialedNumber, '\0', sizeof(pulseDialing->dialedNumber)); //fill this string with \0 so it is empty


}


//http://www.gordostuff.com/2011/08/telephone-set-function-2-to-provide.html


void pulseDialingCallback(pulse_dialing_machine_t *pulseDialing)
{
	GPIO_PinState SHK_CurrentLevel;

	SHK_CurrentLevel=HAL_GPIO_ReadPin(pulseDialing->SHK_GPIO_Port, pulseDialing->SHK_Pin); //read in current level

	//decision making here takes places when an SHK level remains longer than...
	if(SHK_CurrentLevel==pulseDialing->SHK_PreviousLevel){
		//this is the same level as last time
			pulseDialing->SHK_CurrentLevelDurationMiliseconds+=1000/(pulseDialing->callbackFrequencyHertz);  //depending on callback frequency value might be of bad precision
				if(SHK_CurrentLevel==GPIO_PIN_RESET){
					//this is pulse
						if(pulseDialing->SHK_CurrentLevelDurationMiliseconds<PULSE_DIALING_PULSE_LENGTH_MAX_MILISECOND){
							//and there is no timeout
							//do nothing
						}
						else{
							//and there is timeout
							//abort dialing, longer pulse means most probably that the phone is back on-hook
							pulseDialingInit(pulseDialing);
						}
				}
				else{
					//this is pause

						//pause duration is too short to decide

						//pause duration is long enough to recognize a single pulse

						//pause duration is long enough to recognize that there will be no more pulses and digit is dialed completelly






				}


	}
	else{
		//there has been level change since the last time
			//there was a rising slope on SHK pin
				//we might be in the middle of the pulse
				//check timing

			//there was a falling slope on the SHK pin
				//it might have been a single pulse
				//check timing
				//increment pulseDialing->currentDigitCounter

	}




}
