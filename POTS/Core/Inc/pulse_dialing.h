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

#define PULSE_DIALING_HANDSET_QUALIFICATION_MILLISECONDS (200)
#define PULSE_DIALING_DIGIT_QUALIFICATION_MILLISECONDS (100)

typedef enum
{
	SHK_LOW = 0u,
	SHK_HIGH,
	SHK_SLOPE_RISING,
	SHK_SLOPE_FALLING
} SHK_state_t;


typedef enum
{
	HANDSET_ON_HOOK = 0u,
	HANDSET_LIFTED,
//	HANDSET_OUTGOING_CALL
} Handset_state_t;




/* POTS_SHK pin goes HIGH=GPIO_PIN_SET when off-hook
 * POTS_SHK goes LOW=GPIO_PIN_RESET for approx. 50ms and then HIGH for approx 50ms when digit "1" is dialed
 * a 50ms LOW and 50ms HIGH patterns repeats the number of times corresponding to the digit being dialed
 * after a digit has been dialed, POTS_SHK remains HIGH for a longer time, until the user start dialing next digit
 */


typedef struct pulse_dialing_machine_t {


	GPIO_TypeDef *SHK_GPIO_Port;							//Port on which SHK pin is connected
	uint16_t SHK_Pin;										//Pin on which SHK pin is connected
	GPIO_PinState SHK_PreviousLevel;     					//SHK pin state stored when the pulse dialing callback

	uint16_t frequencyCallback_hertz;						//callback frequency expressed in Hertz

	SHK_state_t SHK_State;									//state of the SHK pin recognized in the invocation of the pulse dialing machine
	Handset_state_t Handset_State;							//current state of the handset

	uint8_t counter;
	uint16_t time_SHK_staticLevelDuration_milliseconds;
	uint16_t timeSinceLastRisingSlope_milliseconds;

	int8_t dialedDigit;
	int8_t newEventHandset;

}pulse_dialing_machine_t;


void pulseDialingInit(pulse_dialing_machine_t *pulseDialing);
void pulseDialingCallback(pulse_dialing_machine_t *pulseDialing);

#endif /* INC_PULSE_DIALING_H_ */
