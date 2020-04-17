/*
 * scheduler.c
 *
 * State-machine for handling event-driven processes external to BLE
 *
 *  Created on: Sep 21, 2019
 *      Author: Victor Kronberg
 */


#include "scheduler.h"

// void my_scheduler(myStateTypeDef *state_struct)
// {
//
// 	// Handle events not tied to a given state first
//
// 	// Check for RSSI check event - does not impact state
// 	if( ((state_struct->event_bitmask & ONE_HZ_EVENT_MASK) >> ONE_HZ_EVENT_MASK_POS) == 1 )
// 	{
// 		__disable_irq();
// 		// Clear event bitmask
// 		state_struct->event_bitmask &= ~ONE_HZ_EVENT_MASK;
// 		__enable_irq();
//
// 		scheduler_one_hz_event_handler();
//
// 	}
//
//
// 	// Handle state-driven events
//
// 	switch (state_struct->current_state)
// 	{
// 		case STATE0_WAIT_FOR_BLE:
// 			if( ((state_struct->event_bitmask & BLE_EVENT_MASK) >> BLE_EVENT_MASK_POS) == 1 )
// 			{
// 				__disable_irq();
// 				// Clear event bitmask
// 				state_struct->event_bitmask &= ~BLE_EVENT_MASK;
// 				__enable_irq();
//
// 				// Enter sensor measurement loop
// 				scheduler_enter_temperature_polling_loop();
//
// 				// Set next state
// 				state_struct->next_state = STATE1_WAIT_FOR_TIMER;
// 			}
// 			break;
// 		default:
// 			break;
// 	}
//
// 	if(state_struct->current_state != state_struct->next_state)
// 	{
// 		//LOG_INFO("State transitioned from state %d to %d",state_struct->current_state,state_struct->next_state);
// 		state_struct->current_state = state_struct->next_state;
// 	}
//
// }

/*

void scheduler_exit_temperature_polling_loop(myStateTypeDef *state_struct)
{
	// Disable timer interrupts
	disable_timer_interrupts();

	__disable_irq();
	// Clear any pending external event bitmasks
	state_struct->event_bitmask = 0;
	__enable_irq();

	// Find out if sleep block needs to be removed
	SLEEP_EnergyMode_t deepest_sleep = SLEEP_LowestEnergyModeGet();

	// Remove block if needed
	if(deepest_sleep == sleepEM1)
	{
		SLEEP_SleepBlockEnd(sleepEM2);
	}

	// Set state to waiting for BLE connection
	state_struct->current_state = STATE0_WAIT_FOR_BLE;
	state_struct->next_state = STATE0_WAIT_FOR_BLE;
}


*/
