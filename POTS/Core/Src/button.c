/*
 * button.c
 *
 *  Created on: Sep 22, 2023
 *      Author: Arek
 */

#include "button.h"

void buttonInit(button_t *myButton){
	myButton->stateDurationMilliseconds=0;
	myButton->previousLevel=HAL_GPIO_ReadPin(myButton->port_BUTTON, myButton->pin_BUTTON);
	myButton->buttonRequestedState=BUTTON_STATE_OFF;
	myButton->buttonCurrentLevelProcessed=BUTTON_CURRENT_LEVEL_NOT_PROCESSED;
	myButton->actionFlag=BUTTON_ACTION_NO_FLAG;
}

void buttonTimeKeeping(button_t *myButton)  //this will be called in timer interrupt
{
	myButton->previousLevel=myButton->currentLevel;
	myButton->currentLevel=HAL_GPIO_ReadPin(myButton->port_BUTTON, myButton->pin_BUTTON);

	if(myButton->currentLevel==myButton->previousLevel){	//state didn't change
		if(myButton->stateDurationMilliseconds<(UINT16_MAX-myButton->timeKeepingPeriodMilliseconds)){
				myButton->stateDurationMilliseconds+=myButton->timeKeepingPeriodMilliseconds;
			}
		if(myButton->stateDurationMilliseconds>=BUTTON_DEBOUNCE_TIME_MILISECONDS && myButton->currentLevel==GPIO_PIN_RESET && myButton->buttonCurrentLevelProcessed==BUTTON_CURRENT_LEVEL_NOT_PROCESSED){	//if pressed long enough
			myButton->buttonCurrentLevelProcessed=BUTTON_CURRENT_LEVEL_PROCESED;
			//so this is a recognized, long pressed low state, it means we have to change the state to the opposite
			if(myButton->buttonRequestedState==BUTTON_STATE_OFF){
				myButton->buttonRequestedState=BUTTON_STATE_ON;
				myButton->actionFlag=BUTTON_ACTION_FLAG_SET;
			}
			else{
				myButton->buttonRequestedState=BUTTON_STATE_OFF;
				myButton->actionFlag=BUTTON_ACTION_FLAG_SET;
			}
		}
	}
	else	//state of the button line changed
	{
		myButton->buttonCurrentLevelProcessed=BUTTON_CURRENT_LEVEL_NOT_PROCESSED;
		myButton->stateDurationMilliseconds=0;
	}
}
