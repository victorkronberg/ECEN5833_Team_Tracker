/*
 * gecko_ble_server.h
 *
 * BLE interface header for Server
 *
 *  Created on: Oct 1, 2019
 *      Author: vkronber
 */

#ifndef SRC_GECKO_BLE_SERVER_H_
#define SRC_GECKO_BLE_SERVER_H_

#include "main.h"
#include "gecko_ble_errors.h"

#define ADVERTISE_INTERVAL_250MS	 		(400)
#define CONNECTION_INTERVAL_75MS	 		(60)
#define LATENCY_300MS				      	(3)
#define CONNECTION_TIMEOUT			   		(350)	// Set to 3.5 second
#define MAX_CE_LENGTH				       	(0xffff)

/** Timer Frequency used. */
#define TIMER_CLK_FREQ ((uint32)32768)
/** Convert msec to timer ticks. */
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)


// TXPOWER defines
// Ranges based on UG103.14: Bluetooth LE Fundamentals by Silicon Labs
// and Bluetoooth Low Energy: A Developer's Hanbook by Robin Heydon (p.56-57)
#define TXPOWER_MIN					(-200)
#define TXPOWER_NEG_20DB			(-200)
#define TXPOWER_NEG_15DB			(-150)
#define TXPOWER_NEG_5DB				(-50)
#define TXPOWER_0DB					(0)
#define TXPOWER_POS_5DB				(50)
#define TXPOWER_MAX					(100)

// Pending Indication flags
#define HTM_PENDING_MASK			(0x0001)
#define HTM_PENDING_POS				(0)
#define BUTTON_PENDING_MASK			(0x0002)
#define BUTTON_PENDING_POS			(1)
#define ACCEL_X_PENDING_MASK		(0x0004)
#define ACCEL_X_PENDING_POS			(2)
#define ACCEL_Y_PENDING_MASK		(0x0008)
#define ACCEL_Y_PENDING_POS			(3)
#define ACCEL_Z_PENDING_MASK		(0x0010)
#define ACCEL_Z_PENDING_POS			(4)
#define ALTITUDE_PENDING_MASK		(0x0020)
#define ALTITUDE_PENDING_POS		(5)

// Global variable containing connection handle
uint8_t conn_handle;
struct indications
{
	uint32_t indications;
	uint32_t confirmations;
} pending;
uint8_t htmTempBuffer[5]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
uint8_t pressureBuffer[8]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
uint8_t x_buffer[2];
uint8_t y_buffer[2];
uint8_t z_buffer[2];
uint8_t steps_buffer[4];
uint8_t elevation_buffer[4];
uint8_t altitude_buffer[2];
uint8_t button_state;

/**
 * [gecko_ble_init_LCD_status]
 * @description: Initializes LCD with BT address and server/client status of device
 */
void gecko_ble_init_LCD_status(void);

/**
 * [gecko_ble_update]
 * @description:  Handles BLE-triggered events
 * @param        evt [Command packet containing event ID and associated data]
 * @return           [True: Triggering event was handled
 *                    False: Triggering event was not handled]
 */
bool gecko_ble_server_update(struct gecko_cmd_packet* evt);

/**
 * [gecko_update]
 * @description:  Handles BLE-triggered events related to system boot,
 *                dropped connection, and OTA events.  Called from within
 *                gecko_ble_update. Based on Silicon Labs empty Blue Gecko
 *                Bluetoot project.
 * @param        evt [Command packet containing event ID and associated data]
 * @return           [True: Triggering event was handled
 *                    False: Triggering event was not handled]
 */
bool gecko_update(struct gecko_cmd_packet* evt);

/**
 * [gecko_ble_send_temperature]
 * @description:  Converts uint32_t temperature to bitstream for BLE
 *                packet.  Sends temperature "indication" to all listening
 *                clients. Based on Silicon Labs Health Thermometer Blue
 *                Gecko example.
 * @param        temperature [Uint32_t temperature in milli-degrees C]
 */
void gecko_ble_send_temperature(uint32_t temperature);

void gecko_ble_send_steps(void);

/**
 * [gecko_ble_send_button_state]
 * @description:  Sends indication containing current state of button
 *                Pressed     = 0x01
 *                Not pressed = 0x00
 */
void gecko_ble_send_button_state(void);

void gecko_ble_send_accel_data(uint8_t accel_pending_mask);

void gecko_ble_send_altitude_data(void);

/**
 * [gecko_ble_get_rssi]
 * @description:  Initiates RSSI retrieval using global connection handle
 */
void gecko_ble_get_rssi(void);

/**
 * [gecko_ble_dynamic_tx_power_update]
 * @description:  Updates the global TX power based on given RSSI value.
 *                First, it calculates transmit power from RSSI in 0.1dBm units,
 *                then uses that value to call gecko_ble_update_tx_power.
 * @param        rssi [RSSI value - signal strength]
 */
void gecko_ble_dynamic_tx_power_update(int8_t rssi);

/**
 * [gecko_ble_update_tx_power]
 * @description:  Updates max system TX power.  Halts system prior to updating
 *                power based on given TX power, then resumes system.
 * @param        power [TX Power in units of 0.1dBm]
 */
void gecko_ble_update_tx_power(int16_t power);


#endif /* SRC_GECKO_BLE_SERVER_H_ */
