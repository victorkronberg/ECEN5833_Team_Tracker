/***************************************************************************//**
 * @file
 * @brief Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "src/main.h"

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_ADVERTISERS
#define MAX_ADVERTISERS 1
#endif

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif

//#define FEATURE_PA_INPUT_FROM_VBAT 1

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

/* Bluetooth stack configuration parameters (see "UG136: Silicon Labs Bluetooth C Application Developer's Guide" for details on each parameter) */
static gecko_configuration_t config = {
  .config_flags = 0,                                   /* Check flag options from UG136 */
#if defined(FEATURE_LFXO) || defined(PLFRCO_PRESENT) || defined(LFRCO_PRECISION_MODE)
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,        /* Sleep is enabled */
#else
  .sleep.flags = 0,
#endif
  .bluetooth.max_connections = MAX_CONNECTIONS,        /* Maximum number of simultaneous connections */
  .bluetooth.max_advertisers = MAX_ADVERTISERS,        /* Maximum number of advertisement sets */
  .bluetooth.heap = bluetooth_stack_heap,              /* Bluetooth stack memory for connection management */
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap), /* Bluetooth stack memory for connection management */
#if defined(FEATURE_LFXO)
  .bluetooth.sleep_clock_accuracy = 100,               /* Accuracy of the Low Frequency Crystal Oscillator in ppm. *
                                                       * Do not modify if you are using a module                  */
#elif defined(PLFRCO_PRESENT) || defined(LFRCO_PRECISION_MODE)
  .bluetooth.sleep_clock_accuracy = 500,               /* In case of internal RCO the sleep clock accuracy is 500 ppm */
#endif
  .gattdb = &bg_gattdb_data,                           /* Pointer to GATT database */
  .ota.flags = 0,                                      /* Check flag options from UG136 */
  .ota.device_name_len = 3,                            /* Length of the device name in OTA DFU mode */
  .ota.device_name_ptr = "OTA",                        /* Device name in OTA DFU mode */
  .pa.config_enable = 1,                               /* Set this to be a valid PA config */
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT,               /* Configure PA input to VBAT */
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,               /* Configure PA input to DCDC */
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
  .rf.flags = GECKO_RF_CONFIG_ANTENNA,                 /* Enable antenna configuration. */
  .rf.antenna = GECKO_RF_ANTENNA,                      /* Select antenna path! */
};

/**
 * @brief  Main function
 */
int main(void)
{

	eIMU_ERRORS rslt = 0;
	uint8_t bmp_rslt = BMP3_OK;


	bmp_device.dev_id = BMP388_DEV_ID;
	bmp_device.read = &spi_read;
	bmp_device.write = &spi_write;
	bmp_device.delay_ms = &delay_ms;
	bmp_device.intf = BMP3_SPI_INTF;

	imu_dev.dev_id = ICM_DEV_ID;
	imu_dev.read = &spi_read;
	imu_dev.write = &spi_write;
	imu_dev.delay_ms = &delay_ms;
	sensors = ACCELEROMETER | GYROSCOPE | THERMOMETER;

  /* Initialize device */
  initMcu();
  /* Initialize board */
  initBoard();

  gpioInit();
  init_letimer();
  initUSART1();

  /* Initialize debug prints. Note: debug prints are off by default. See DEBUG_LEVEL in app.h */
   initLog();

   init_scheduler();

  icm20948_init(&imu_dev);

  bmp_rslt = bmp3_init(&bmp_device);
  printLog("BMP initialization result: %d\r\n", bmp_rslt);

  bmp_rslt = bmp_set_normal_mode(&bmp_device);
  printLog("BMP set normal mode result: %d\r\n", bmp_rslt);
  /* Initialize application */
  //initApp();
  //initVcomEnable();

  uint32_t i;

  delay_ms(500);
  gpioLed0SetOn();
  rslt = icm20948_get_agmt(&imu_dev,sensors);
  bmp_rslt = get_sensor_data(&bmp_device);


  printLog("Accel data - X: %d  Y: %d  Z: %d\n\r",imu_dev.sensor_data.accelerometer_x,imu_dev.sensor_data.accelerometer_y,imu_dev.sensor_data.accelerometer_z);
  printLog("Gyro data - X: %d  Y: %d  Z: %d\n\r",imu_dev.sensor_data.gyroscope_x,imu_dev.sensor_data.gyroscope_y,imu_dev.sensor_data.gyroscope_y);
  printLog("Temperature: %d C\n\r",imu_dev.sensor_data.temperature);

  delay_ms(500);
  bmp_rslt = get_sensor_data(&bmp_device);
  rslt = icm20948_get_agmt(&imu_dev,sensors);
  gpioLed0SetOff();

  printLog("Accel data - X: %d  Y: %d  Z: %d\n\r",imu_dev.sensor_data.accelerometer_x,imu_dev.sensor_data.accelerometer_y,imu_dev.sensor_data.accelerometer_z);
  printLog("Gyro data - X: %d  Y: %d  Z: %d\n\r",imu_dev.sensor_data.gyroscope_x,imu_dev.sensor_data.gyroscope_y,imu_dev.sensor_data.gyroscope_y);
  printLog("Temperature: %d C\n\r",imu_dev.sensor_data.temperature);


  /* Start application */
  appMain(&config);


}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
