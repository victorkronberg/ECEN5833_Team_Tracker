/*
 * main.h
 *
 *  Created on: Apr 8, 2020
 *      Author: vkronber
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_letimer.h"
#include "em_device.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/* Application header */
#include "app.h"
#include "gpio.h"
#include "spi.h"
#include "imu.h"
#include "timer.h"
#include "gecko_ble_errors.h"
#include "scheduler.h"
#include "gecko_ble_server.h"
#include "ble_events.h"
#include "bmp3.h"
#include "pressure_sensor.h"

// Event bitmasks
#define TIMER_EVENT_MASK			(0x0001)
#define TIMER_EVENT_MASK_POS		(0)
#define DELAY_EVENT_MASK			(0x0002)
#define DELAY_EVENT_MASK_POS		(1)
#define PEDOMETER_EVENT_MASK		(0x0004)
#define PEDOMETER_EVENT_MASK_POS	(2)
#define BLE_EVENT_MASK				(0x0008)
#define BLE_EVENT_MASK_POS			(3)
#define EXIT_EVENT_MASK				(0x0010)
#define EXIT_EVENT_MASK_POS			(4)
#define FLIGHT_EVENT_MASK			(0x0020)
#define FLIGHT_EVENT_MASK_POS		(5)
#define PASSKEY_CONFIRM_MASK		(0x0040)
#define PASSKEY_CONFIRM_POS			(6)
#define BUTTON_EVENT_MASK			(0x0080)
#define BUTTON_EVENT_MASK_POS		(7)

uint32_t event_bitmask;
uint32_t interrupt_event_bitmask;

struct imu_dev imu_dev;
uint8_t sensors;
struct bmp3_dev bmp_device;
struct bmp3_data data;

#endif /* SRC_MAIN_H_ */
