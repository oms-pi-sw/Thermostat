#ifndef _SENSORS_UTILS_H_
#define _SENSORS_UTILS_H_

#include <mbed.h>
#include "constants.h"
#include <BME280.h>
#include "input_utils.hpp"
#include "peripherals.hpp"

extern BME280 *bme280;

extern float temperatures[], humidities[], pressures[];
extern volatile uint8_t samples;

// Temperature, humidity, pressure, altitude
extern float temperature, humidity;
extern float pressure, altitude;
extern float t_max, t_min, h_max, h_min, p_max, p_min;

extern volatile bool readyToSample;
extern volatile bool readyToMeasure;

bool begin_sensors(void);
void reset_samples_arrays(void);
void tickSample(void);
void tickMeasure(void);
void doSample(void);
void readSensors(void);

#endif // _SENSORS_UTILS_H_