/*
 * scheduler.h
 *
 * Header file for scheduler
 *
 *  Created on: Sep 21, 2019
 *      Author: Victor Kronberg
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include "main.h"

#define ZERO_DELAY	(0)

typedef enum states  {
	STATE0_SLEEP,
	STATE1_ADVERTISING,
	STATE2_PEDOMETER,
	STATE3_FLIGHT_RECORDER,
	MY_NUM_STATES
} myState;


typedef struct	StateStruct {
	myState current_state;
	myState next_state;
	uint32_t event_bitmask;
	uint32_t periodic_timer0;
} myStateTypeDef;


// Global state structure
myStateTypeDef my_state_struct;

// Global temperature value
uint32_t temp_value;

/**
 * [my_scheduler]
 * @description: Interrupt driven scheduler which controls server state machine. Handles
 * 							changes between states and calling functions relevant to state
 * 							operations.
 * @param        state_struct [Struct containing context for the state machine]
 */
void my_scheduler(myStateTypeDef *state_struct);

/**
 * [scheduler_power_up_si7021]
 * @description: Reset periodice timer, enable power to the si7021,
 * 							and set delay to wait for si7021 power-up. To be called in STATE0_WAIT_FOR_TIMER
 */
void scheduler_power_up_si7021(void);

void init_scheduler(void);

void scheduler_toggle_advertising(bool advertising);

void scheduler_enter_fight_mode(myStateTypeDef *state_struct);

void scheduler_enter_pedometer_mode(myStateTypeDef *state_struct);

void scheduler_exit_flight_mode(myStateTypeDef *state_struct);

void scheduler_exit_pedometer_mode(myStateTypeDef *state_struct);

#endif /* SRC_SCHEDULER_H_ */
