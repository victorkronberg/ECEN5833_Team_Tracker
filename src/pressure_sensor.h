/*
 * pressure_sensor.h
 *
 *  Created on: Apr 13, 2020
 *      Author: vkronber
 */

#ifndef SRC_PRESSURE_SENSOR_H_
#define SRC_PRESSURE_SENSOR_H_

#include "main.h"

int8_t bmp_set_normal_mode(struct bmp3_dev *dev);
int8_t bmp_wake_mode(struct bmp3_dev *dev);
int8_t bmp_sleep_mode(struct bmp3_dev *dev);
int8_t get_sensor_data(struct bmp3_dev *dev);

/* Variable used to store the compensated data */
struct bmp3_data pressure_data;

#endif /* SRC_PRESSURE_SENSOR_H_ */
