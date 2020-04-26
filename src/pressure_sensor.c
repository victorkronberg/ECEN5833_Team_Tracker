/*
 * pressure_sensor.c
 *
 *  Created on: Apr 13, 2020
 *      Author: vkronber
 */


#include "pressure_sensor.h"

int8_t bmp_set_normal_mode(struct bmp3_dev *dev)
{
    int8_t rslt;
    /* Used to select the settings user needs to change */
    uint16_t settings_sel;

    /* Select the pressure and temperature sensor to be enabled */
    dev->settings.press_en = BMP3_ENABLE;
    dev->settings.temp_en = BMP3_ENABLE;
    /* Select the output data rate and oversampling settings for pressure and temperature */
    dev->settings.odr_filter.press_os = BMP3_NO_OVERSAMPLING;
    dev->settings.odr_filter.temp_os = BMP3_NO_OVERSAMPLING;
    dev->settings.odr_filter.odr = BMP3_ODR_25_HZ;
    /* Assign the settings which needs to be set in the sensor */
    settings_sel = BMP3_PRESS_EN_SEL | BMP3_TEMP_EN_SEL | BMP3_PRESS_OS_SEL | BMP3_TEMP_OS_SEL | BMP3_ODR_SEL;
    rslt = bmp3_set_sensor_settings(settings_sel, dev);

    /* Set the power mode to normal mode */
    dev->settings.op_mode = BMP3_NORMAL_MODE;
    rslt = bmp3_set_op_mode(dev);

    return rslt;
}

int8_t bmp_wake_mode(struct bmp3_dev *dev)
{
	int8_t rslt;
	/* Set the power mode to normal mode */
	dev->settings.op_mode = BMP3_NORMAL_MODE;
	rslt = bmp3_set_op_mode(dev);

	return rslt;
}

int8_t bmp_sleep_mode(struct bmp3_dev *dev)
{
	int8_t rslt;
	/* Set the power mode to sleep mode */
	dev->settings.op_mode = BMP3_SLEEP_MODE;
	rslt = bmp3_set_op_mode(dev);

	return rslt;
}

int8_t get_sensor_data(struct bmp3_dev *dev)
{
    int8_t rslt;
    /* Variable used to select the sensor component */
    uint8_t sensor_comp;

    /* Sensor component selection */
    sensor_comp = BMP3_PRESS | BMP3_TEMP;
    /* Temperature and Pressure data are read and stored in the bmp3_data instance */
    rslt = bmp3_get_sensor_data(sensor_comp, &pressure_data, dev);

    /* Print the temperature and pressure data */
    printLog("Temperature in deg celsius\t Pressure in Pascal\t\n");
	/* for fixed point the compensated temperature and pressure output has a multiplication factor of 100 */
    printLog("%lld\t\t %llu\t\t\n",pressure_data.temperature, pressure_data.pressure);

    return rslt;
}
