/*
 * ble_timer.h
 *
 *  Created on: Apr 13, 2020
 *      Author: vkronber
 */

#ifndef SRC_BLE_EVENTS_H_
#define SRC_BLE_EVENTS_H_

#include "main.h"

#define TIMER_ID_PEDOMETER_MODE				(0x02)
#define TIMER_ID_FLIGHT_MODE				(0x03)
#define TIMER_ID_LED_INDICATOR				(0x05)

uint32_t led_counter;

bool ble_timer_events(struct gecko_cmd_packet* evt);

bool ble_characteristic_events(struct gecko_cmd_packet* evt);

bool ble_handle_confirmations(struct gecko_cmd_packet* evt);

#endif /* SRC_BLE_EVENTS_H_ */
