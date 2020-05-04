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
			if( ((state_struct->event_bitmask & FLIGHT_EVENT_MASK) >> FLIGHT_EVENT_MASK_POS) == 1 )
			{
				printLog("Entering flight recorder\r\n");
				scheduler_enter_fight_mode(state_struct);
				// Set next state
				state_struct->next_state = STATE3_FLIGHT_RECORDER;
			}
			else if( ((state_struct->event_bitmask & PEDOMETER_EVENT_MASK) >> PEDOMETER_EVENT_MASK_POS) == 1 )
			{
				printLog("Entering pedometer\r\n");
				scheduler_enter_pedometer_mode(state_struct);
				// Set next state
				state_struct->next_state = STATE2_PEDOMETER;
			}
			break;
		case STATE2_PEDOMETER:
			if( ((state_struct->event_bitmask & EXIT_EVENT_MASK) >> EXIT_EVENT_MASK_POS) == 1 )
			{
				printLog("Exiting pedometer\r\n");
				scheduler_exit_pedometer_mode(state_struct);

				// Set next state
				state_struct->next_state = STATE0_SLEEP;
			}
			break;
		case STATE3_FLIGHT_RECORDER:
			if( ((state_struct->event_bitmask & EXIT_EVENT_MASK) >> EXIT_EVENT_MASK_POS) == 1 )
			{
				printLog("Exiting flight recorder\r\n");
				scheduler_exit_flight_mode(state_struct);

				// Set next state
				state_struct->next_state = STATE0_SLEEP;
			}
			break;
		default:
			break;
	}

	if(state_struct->event_bitmask != 0)
	{
		printLog("Not all events handled, event %d cleared\r\n", state_struct->event_bitmask);
		__disable_irq();
		// Clear event bitmask
		state_struct->event_bitmask = 0;
		__enable_irq();
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

void scheduler_enter_fight_mode(myStateTypeDef *state_struct)
{
	__disable_irq();
	// Clear event bitmask
	state_struct->event_bitmask &= ~FLIGHT_EVENT_MASK;
	__enable_irq();
	// Wake sensors
	icm20948_sleep(&imu_dev,false);
	bmp_wake_mode(&bmp_device);
	// Delay while sensors come online (if needed)

	delay_ms(35);

	tracker_enter_flight_mode();

	// Start repeating timer
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(500),
			  												TIMER_ID_FLIGHT_MODE,
			  												0));

	// Set LED timer for 8 fast blinks
	led_counter = 15;
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(100),
															TIMER_ID_LED_INDICATOR,
															0));
}

void scheduler_enter_pedometer_mode(myStateTypeDef *state_struct)
{
	__disable_irq();
	// Clear event bitmask
	state_struct->event_bitmask &= ~PEDOMETER_EVENT_MASK;
	position_data.steps_taken = 0;
	__enable_irq();
	// Wake sensors
	icm20948_sleep(&imu_dev,false);
	bmp_wake_mode(&bmp_device);
	// Delay while sensors come online (if needed)
	delay_ms(35);

	tracker_enter_pedometer_mode();

	// Start repeating timer
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(500),
															TIMER_ID_PEDOMETER_MODE,
															0));

	// Start 50Hz timer for pedometer data
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(50),
															TIMER_ID_PEDOMETER_50HZ,
															0));

	// Set LED timer for 4 fast blinks
	led_counter = 7;
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(100),
															TIMER_ID_LED_INDICATOR,
															0));
}


void scheduler_exit_flight_mode(myStateTypeDef *state_struct)
{
	__disable_irq();
	// Clear event bitmask
	state_struct->event_bitmask &= ~EXIT_EVENT_MASK;
	__enable_irq();
	// Sleep sensors
	icm20948_sleep(&imu_dev,true);
	bmp_sleep_mode(&bmp_device);
	// End repeating timer
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(500),
															TIMER_ID_FLIGHT_MODE,
															1));

	// Set LED timer for 4 slow blinks
	led_counter = 7;
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(450),
															TIMER_ID_LED_INDICATOR,
															0));
}

void scheduler_exit_pedometer_mode(myStateTypeDef *state_struct)
{
	__disable_irq();
	// Clear event bitmask
	state_struct->event_bitmask &= ~EXIT_EVENT_MASK;
	__enable_irq();
	// Sleep sensors
	icm20948_sleep(&imu_dev,true);
	bmp_sleep_mode(&bmp_device);
	// End repeating timer
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(500),
															TIMER_ID_PEDOMETER_MODE,
															1));
	// End 50Hz timer for pedometer data
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(50),
															TIMER_ID_PEDOMETER_50HZ,
															1));
	// Set LED timer for 2 slow blinks
	led_counter = 3;
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(450),
															TIMER_ID_LED_INDICATOR,
															0));
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

