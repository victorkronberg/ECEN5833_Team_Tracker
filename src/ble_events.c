/*
 * ble_timer.c
 *
 *  Created on: Apr 13, 2020
 *      Author: vkronber
 */


#include <src/ble_events.h>

bool ble_timer_events(struct gecko_cmd_packet* evt)
{
	switch (evt->data.evt_hardware_soft_timer.handle)
	{
		case TIMER_ID_FLIGHT_MODE:
			printLog("Flight timer event\r\n");
			if(my_state_struct.current_state == STATE0_SLEEP)
			{
				printLog("Flight ended, going to sleep\r\n");
				// Clear pending indications and return to sleep
				__disable_irq();
				pending.confirmations &= ~(HTM_PENDING_MASK|ACCEL_X_PENDING_MASK);
				__enable_irq();
			}
			else
			{
				gecko_ble_send_temperature(temp_value);
			}
			break;
		case TIMER_ID_PEDOMETER_MODE:
			printLog("Pedometer timer event\r\n");
			if(my_state_struct.current_state == STATE0_SLEEP)
			{
				printLog("Pedometer ended, going to sleep\r\n");
				// Clear pending indications and return to sleep
				__disable_irq();
				pending.confirmations &= ~(HTM_PENDING_MASK|ACCEL_X_PENDING_MASK);
				__enable_irq();
			}
			else
			{
				gecko_ble_send_accel_data(ACCEL_X_PENDING_MASK);
				gecko_ble_send_steps();
			}
			break;
		case TIMER_ID_PEDOMETER_50HZ:
			icm20948_get_agmt(&imu_dev,ACCELEROMETER);
			z_axis_data[pedometer_counter] = (float) imu_dev.sensor_data.accelerometer_z;
			pedometer_counter++;
			if(pedometer_counter >= SAMPLE_LENGTH)
			{
				printLog("pedometer_overflow\r\n");
				uint32_t steps_since_last_overflow = tracker_thresholding(z_axis_data,z_axis_filtered,7,3.0,0.5);
				position_data.steps_taken += steps_since_last_overflow;
				pedometer_counter = 0;
			}
			break;
		case TIMER_ID_LED_INDICATOR:

			if(led_counter % 2 == 1)
			{
				gpioLed0SetOn();
			}
			else
			{
				gpioLed0SetOff();
			}

			// Set one more blink if counter is out
			if(led_counter == 1)
			{
				BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(450),
																			TIMER_ID_LED_INDICATOR,
							  												1));
			}

			led_counter--;

			break;

		default:
		break;
	}


}

bool ble_characteristic_events(struct gecko_cmd_packet* evt)
{
	/* Check that the characteristic in question is temperature - its ID is defined
	* in gatt.xml as "temperature_measurement". Also check that status_flags = 1, meaning that
	* the characteristic client configuration was changed (notifications or indications
	* enabled or disabled). */
	if ((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement)
		&& (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01))
	{
	  if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02)
	  {
		  __disable_irq();
		  // Set flag to enter pedometer mode
		  my_state_struct.event_bitmask |= FLIGHT_EVENT_MASK;

		  __enable_irq();
		  printLog("Temp timer has been set - entering flight mode\r\n");

	  } else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00)
	  {

		  __disable_irq();
		  // Disable temperature polling
		  my_state_struct.event_bitmask |= EXIT_EVENT_MASK;
		  pending.confirmations &= ~(HTM_PENDING_MASK|ACCEL_X_PENDING_MASK);
		  __enable_irq();

		  printLog("Temp exit event - exiting flight mode\r\n");
	  }
	}

	// Check for Accel indication enabled
	if ((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_accel_x_axis)
		&& (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01))
	{
	  if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02)
	  {

		  __disable_irq();
		  // Set flag to enter pedometer mode
		  my_state_struct.event_bitmask |= PEDOMETER_EVENT_MASK;

		  __enable_irq();
		  printLog("Accel timer has been set - entering pedometer mode\r\n");

	  } else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00)
	  {

		  __disable_irq();
		// Disable temperature polling
		  my_state_struct.event_bitmask |= EXIT_EVENT_MASK;
		  __enable_irq();

		  printLog("Pedometer exit event\r\n");
	  }
	}


	// Check for pending button press confirmation and that the correct confirmation was sent
	if( (((pending.confirmations & ACCEL_X_PENDING_MASK) >> ACCEL_X_PENDING_POS) == 1)
			&& (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_accel_x_axis)
			&& (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x02))
	{
		// Clear pending confirmation mask for button press
		__disable_irq();
		pending.confirmations &= ~ACCEL_X_PENDING_MASK;
		__enable_irq();
		printLog("Accel confirmation\r\n");

		//BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(50000,
		//															TIMER_ID_STATE3_SEND_AGMT,
		//															1));

		if(((pending.indications & HTM_PENDING_MASK) >> HTM_PENDING_POS) == 1)
		{
			gecko_ble_send_temperature(temp_value);
		}
	}	// Check for pending HTM confirmation and that the correct confirmation was sent
	else if( (((pending.confirmations & HTM_PENDING_MASK) >> HTM_PENDING_POS) == 1)
			&& (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement)
			&& (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x02))
	{
		// Clear pending confirmation mask for button press
		__disable_irq();
		pending.confirmations &= ~HTM_PENDING_MASK;
		__enable_irq();
		printLog("Temp confirmation\r\n");
		//BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(50000,
		//														   TIMER_ID_TEMP_MEASUREMENT,
		//														   1));

		// Check for pending indications
		if(((pending.indications & ACCEL_X_PENDING_MASK) >> ACCEL_X_PENDING_POS) == 1)
		{
			gecko_ble_send_accel_data(ACCEL_X_PENDING_MASK);
		}
	}
}
