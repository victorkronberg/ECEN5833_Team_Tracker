/*
 * tracker.h
 *
 *  Created on: May 2, 2020
 *      Author: vkronber
 */

#ifndef SRC_TRACKER_H_
#define SRC_TRACKER_H_

#include "main.h"
#include <string.h>

#define M_PI			3.14159265358979323846
#define SAMPLE_LENGTH	(75)

uint32_t pedometer_counter;
int32_t z_axis_filtered[SAMPLE_LENGTH];
float	z_axis_data[SAMPLE_LENGTH];

struct tracker_data
{
	// Orientation of device
	uint16_t x_angle;
	uint16_t y_angle;
	uint16_t z_angle;

	// Altitude-related metrics
	float air_pressure;
	float starting_altitude;
	float current_altitude;

	// Pedometer data
	uint32_t steps_taken;
	float delta_altitude;
};

void tracker_enter_flight_mode(void);
void tracker_enter_pedometer_mode(void);
void tracker_update_altitude(void);
void tracker_calculate_tilt(void);
uint32_t tracker_thresholding(float *y, int32_t *signals, int32_t lag, float threshold, float influence);
float tracker_mean(float *data, int32_t len);
float tracker_stddev(float *data, int32_t len, float the_mean);

#endif /* SRC_TRACKER_H_ */
