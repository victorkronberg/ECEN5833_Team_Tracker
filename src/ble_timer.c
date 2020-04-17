/*
 * ble_timer.c
 *
 *  Created on: Apr 13, 2020
 *      Author: vkronber
 */


#include "ble_timer.h"

bool ble_timer_events(struct gecko_cmd_packet* evt)
{
	switch (evt->data.evt_hardware_soft_timer.handle)
	{
		case TIMER_ID_TEMP_MEASUREMENT:
			printLog("Temp timer event\r\n");
			gecko_ble_send_temperature(temp_value);
			temp_value++;
			printLog("Temp sent\r\n");
		case TIMER_ID_ACCEL_MEASUREMENT:
			printLog("Accel timer event\r\n");
			gecko_ble_send_accel_data();
		break;

		default:
		break;
	}


}
