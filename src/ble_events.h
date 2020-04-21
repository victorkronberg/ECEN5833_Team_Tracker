/*
 * ble_timer.h
 *
 *  Created on: Apr 13, 2020
 *      Author: vkronber
 */

#ifndef SRC_BLE_EVENTS_H_
#define SRC_BLE_EVENTS_H_

#include "main.h"

#define TIMER_ID_TEMP_MEASUREMENT			(0x01)
#define TIMER_ID_STATE1_WAKE_AGMT			(0x02)
#define TIMER_ID_STATE2_GET_AGMT			(0x03)
#define TIMER_ID_STATE3_SEND_AGMT			(0x04)

bool ble_timer_events(struct gecko_cmd_packet* evt);

bool ble_characteristic_events(struct gecko_cmd_packet* evt);

bool ble_handle_confirmations(struct gecko_cmd_packet* evt);

#endif /* SRC_BLE_EVENTS_H_ */
