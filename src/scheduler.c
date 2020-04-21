/*
 * scheduler.c
 *
 * State-machine for handling event-driven processes external to BLE
 *
 *  Created on: Sep 21, 2019
 *      Author: Victor Kronberg
 */


#include "scheduler.h"

void my_scheduler(myStateTypeDef *state_struct)
{

	// Handle state-driven events
	printLog("State event\r\n");

	switch (state_struct->current_state)
	{
		case STATE0_SLEEP:
			if( ((state_struct->event_bitmask & BUTTON_EVENT_MASK) >> BUTTON_EVENT_MASK_POS) == 1 )
			{
				__disable_irq();
				// Clear event bitmask
				state_struct->event_bitmask &= ~BUTTON_EVENT_MASK;
				__enable_irq();

				//Enable advertising
				scheduler_toggle_advertising(true);

				// Set next state
				state_struct->next_state = STATE1_ADVERTISING;
			}
			break;
		case STATE1_ADVERTISING:
			if( ((state_struct->event_bitmask & BUTTON_EVENT_MASK) >> BUTTON_EVENT_MASK_POS) == 1 )
			{
				__disable_irq();
				// Clear event bitmask
				state_struct->event_bitmask &= ~BUTTON_EVENT_MASK;
				__enable_irq();

				//Disable advertising
				scheduler_toggle_advertising(false);

				// Set next state
				state_struct->next_state = STATE0_SLEEP;
			}
			break;
		default:
			break;
	}

	if(state_struct->current_state != state_struct->next_state)
	{
		//LOG_INFO("State transitioned from state %d to %d",state_struct->current_state,state_struct->next_state);
		state_struct->current_state = state_struct->next_state;
	}

}


void init_scheduler(void)
{
	my_state_struct.current_state = STATE0_SLEEP;
	my_state_struct.next_state = STATE0_SLEEP;
	my_state_struct.event_bitmask = 0;
}

void scheduler_toggle_advertising(bool advertising)
{
	if(advertising)
	{
		printLog("Starting advertisement\r\n");
		/* Set Advertising parameters */
		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, ADVERTISE_INTERVAL_250MS, ADVERTISE_INTERVAL_250MS, 0, 0));
		/* Start advertising */
		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	}
	else
	{
		printLog("Halting advertisement\r\n");
		/* Send one advertisement and allow timeout */
		/* Set Advertising parameters */
		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, ADVERTISE_INTERVAL_250MS, ADVERTISE_INTERVAL_250MS, 1, 1));
		/* Start advertising */
		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	}
}

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

