/*
 * tracker.c
 *
 *  Created on: May 2, 2020
 *      Author: vkronber
 */


#include "tracker.h"


void tracker_enter_flight_mode(void)
{
	float altitude;

	get_sensor_data(&bmp_device);

	position_data.air_pressure = (float) pressure_data.pressure/100.0;

	altitude = ((( ( pow((position_data.air_pressure/101592),0.1902632365) )-1 ) * 290.15)/0.0065) * 3.28084;

	position_data.starting_altitude = (altitude * -1);

	printLog("Starting altitude: %f\r\n", position_data.starting_altitude);

}


//0.1902632365
void tracker_enter_pedometer_mode(void)
{
	float altitude;

	get_sensor_data(&bmp_device);

	position_data.air_pressure = (float) pressure_data.pressure/100.0;

	altitude = ((( ( pow((position_data.air_pressure/101592),0.1902632365) )-1 ) * 290.15)/0.0065) * 3.28084;

	position_data.starting_altitude = (altitude * -1);

	printLog("Starting altitude: %f\r\n", position_data.starting_altitude);

	position_data.steps_taken = 0;
}

void tracker_update_altitude(void)
{
	float altitude;

	position_data.air_pressure = (float) pressure_data.pressure/100.0;

	altitude = ((( ( pow((position_data.air_pressure/101592),0.1902632365) )-1 ) * 290.15)/0.0065) * 3.28084;

	position_data.current_altitude = (altitude * -1);

	position_data.delta_altitude = position_data.current_altitude - position_data.starting_altitude;

}

// Calculate tilt angle from Z=1G
void tracker_calculate_tilt(void)
{
    float XL_x;
    float XL_y;
    float XL_z;
    float Gp;
    float rtn;

	uint8_t scale;

	scale = imu_dev.full_scale.accel;

	XL_x = (float) imu_dev.sensor_data.accelerometer_x/imu_dev.full_scale.accel_divisor[scale];

	XL_y = (float) imu_dev.sensor_data.accelerometer_y/imu_dev.full_scale.accel_divisor[scale];

	XL_z = (float) imu_dev.sensor_data.accelerometer_z/imu_dev.full_scale.accel_divisor[scale];


	// Take 3D Pythagoreon: sqrt(X*X + Y*Y + Z*Z)
    Gp = (XL_x * XL_x) + (XL_y * XL_y) + (XL_z * XL_z);
    Gp = sqrtf(Gp);

	// Calculate tilt angle from Z (vertical)
    rtn = acosf(XL_z/Gp);

    //Convert to degrees and store in accelerometer data
    rtn =  rtn*(180/M_PI);
    imu_dev.sensor_data.accelerometer_z = (uint16_t) rtn;


	// Calculate tilt angle from X (vertical)
    rtn = acosf(XL_x/Gp);

    //Convert to degrees and store in accelerometer data
    rtn =  rtn*(180/M_PI);
    imu_dev.sensor_data.accelerometer_x = (uint16_t) rtn;

	// Calculate tilt angle from Y (vertical)
    rtn = acosf(XL_y/Gp);

    //Convert to degrees and store in accelerometer data
    rtn =  rtn*(180/M_PI);
    imu_dev.sensor_data.accelerometer_y = (uint16_t) rtn;
    printLog("Z Angle: %d\r\n", imu_dev.sensor_data.accelerometer_z);


}

/* The following functions were based off of David C's C implementation of @Jean_Paul's peak detection algorithm
 *
 * J.-P. van Brakel, "Smoothed z-score algorithm", 2016, [online]
 * Available: http://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data.
 */
uint32_t tracker_thresholding(float *y, int32_t *signals, int32_t lag, float threshold, float influence)
{
	float filteredY[SAMPLE_LENGTH];
	uint32_t counter = 0;
    //memset(signals, 0, sizeof(float) * SAMPLE_LENGTH);
    //memcpy(filteredY, y, sizeof(float) * SAMPLE_LENGTH);

	// Pass accerometer data through simple LPF
	y[1] = (y[0] + y[1])/2.0;

    for(int j = 2; j < SAMPLE_LENGTH; j++)
    {
    	y[j] = (y[j] + y[j-1] + y[j-2])/3.0;
    }

    for(int j = 0; j < SAMPLE_LENGTH; j++)
	{
		filteredY[j] = y[j];
	}

    float avgFilter[SAMPLE_LENGTH];
    float stdFilter[SAMPLE_LENGTH];

    avgFilter[lag - 1] = tracker_mean(y, lag);
    stdFilter[lag - 1] = tracker_stddev(y, lag, avgFilter[lag - 1]);

    for (int i = lag; i < SAMPLE_LENGTH; i++)
    {
        if (fabsf(y[i] - avgFilter[i-1]) > threshold * stdFilter[i-1])
        {
            if (y[i] > avgFilter[i-1]) {
                signals[i] = 1;
                if(signals[i-1]<=0 && stdFilter[i-1] > 100)
                {
                	counter++;
                }
            } else {
                signals[i] = -1;
                if(signals[i-1]<=0 && stdFilter[i-1] > 100)
				{
					counter++;
				}
            }
            filteredY[i] = influence * y[i] + (1 - influence) * filteredY[i-1];
        }
        else
        {
            signals[i] = 0;
        }
        avgFilter[i] = tracker_mean(filteredY + i-lag, lag);
        stdFilter[i] = tracker_stddev(filteredY + i-lag, lag, avgFilter[i]);
    }

    printLog("Counted steps in last section: %d\r\n",counter);

    return counter;
}

float tracker_mean(float *data, int32_t len)
{
    float sum = 0.0, mean = 0.0;

    int i;
    for(i=0; i<len; ++i)
    {
        sum += data[i];
    }

    mean = sum/len;
    return mean;


}

float tracker_mean_update(float *data, int32_t len, float avg_mean, int32_t prev_len)
{
	float sum = 0.0, mean = 0.0;

	int i;
	for(i=0; i<len; ++i)
	{
		sum += data[i];
	}

	mean = (sum/(len + prev_len)) + (avg_mean * (len/(len+prev_len)));

	return mean;
}

float tracker_stddev(float *data, int32_t len, float the_mean)
{
    float standardDeviation = 0.0;

    int i;
    for(i=0; i<len; ++i)
    {
        standardDeviation += pow(data[i] - the_mean, 2);
    }

    return sqrt(standardDeviation/len);
}
