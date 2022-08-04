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

typedef enum
{
	SHK_LOW = 0u,
	SHK_HIGH,
	SHK_SLOPE_RISING,
	SHK_SLOPE_FALLING
} SHK_state_t;




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

}pulse_dialing_machine_t;


void pulseDialingInit(pulse_dialing_machine_t *pulseDialing);
void pulseDIalingCallback(pulse_dialing_machine_t *pulseDialing);

#endif /* INC_PULSE_DIALING_H_ */
