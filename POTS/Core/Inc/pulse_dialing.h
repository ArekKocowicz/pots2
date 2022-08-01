/*
 * pulse_dialing.h
 *
 *  Created on: 01.08.2022
 *      Author: Arek
 */

#include "stm32f1xx_hal.h"
#include "string.h"

#ifndef INC_PULSE_DIALING_H_
#define INC_PULSE_DIALING_H_

#define PULSE_DIALING_MAX_NUMBER_LENGTH (16)
#define PULSE_DIALING_PULSE_LENGTH_MIN_MILISECOND (20) //pulse is when SHK goes LOW - too short pulses will be ignored
#define PULSE_DIALING_PULSE_LENGTH_MAX_MILISECOND (80)
#define PULSE_DIALING_PAUSE_LENGTH_MIN_MILISECOND (20) //pulse is when SHK goes HIGH again
#define PULSE_DIALING_PAUSE_LENGTH_MAX_MILISECOND (80) //longer pause means a digit has been completelly dialed




/* POTS_SHK pin goes HIGH=GPIO_PIN_SET when off-hook
 * POTS_SHK goes LOW=GPIO_PIN_RESET for approx. 50ms and then HIGH for approx 50ms when digit "1" is dialed
 * a 50ms LOW and 50ms HIGH patterns repeats the number of times corresponding to the digit being dialed
 * after a digit has been dialed, POTS_SHK remains HIGH for a longer time, until the user start dialing next digit
 */


typedef struct pulse_dialing_machine_t {
	GPIO_PinState SHK_PreviousLevel;     					//SHK pin state stored when the pulse dialing callback

	GPIO_TypeDef *SHK_GPIO_Port;							//Port on which SHK pin is connected
	uint16_t SHK_Pin;										//Pin on which SHK pin is connected

	uint16_t callbackFrequencyHertz;						//
	uint16_t SHK_CurrentLevelDurationMiliseconds;
	uint16_t SHK_PreviousLevelDurationMiliseconds;
	uint8_t currentDigitCounter;                           	//pulse-pause periods will be counted here, this will be
	uint8_t dialedNumber[PULSE_DIALING_MAX_NUMBER_LENGTH]; 	//dialed number will be stored as a string here
}pulse_dialing_machine_t;


void pulseDialingInit(pulse_dialing_machine_t *pulseDialing);
void pulseDIalingCallback(pulse_dialing_machine_t *pulseDialing);

#endif /* INC_PULSE_DIALING_H_ */
