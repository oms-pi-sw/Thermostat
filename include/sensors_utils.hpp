#ifndef _SENSORS_UTILS_H_
#define _SENSORS_UTILS_H_

#include <BME280.h>
#include <mbed.h>

#include "constants.h"
#include "peripherals.hpp"
#include "utils.hpp"

#define T_QUEUE_SIZE ((uint8_t)5)
#define H_QUEUE_SIZE ((uint8_t)5)
#define P_QUEUE_SIZE ((uint8_t)5)

class SensorsUtils {
 public:
  SensorsUtils(const PeripheralsUtils* peripherals);
  virtual ~SensorsUtils();
  bool start(void);
  void cycle(void) { this->measureAndSample(); }
  uint8_t getSamples(void) { return this->samples; }

  /**
   *
   * GETTER SECTION
   * 
   **/
  float getTemperature(void) { return this->temperature; }
  float getHumidity(void) { return this->humidity; }
  float getPressure(void) { return this->pressure; }
  const float* getTemperatures(void) { return this->temperatures; }
  const float* getHumidities(void) { return this->humidities; }
  const float* getPressures(void) { return this->pressures; }
  float getTMax(void) { return this->t_max; }
  float getTMin(void) { return this->t_min; }
  float getHMax(void) { return this->h_max; }
  float getHMin(void) { return this->h_min; }
  float getPMax(void) { return this->p_max; }
  float getPMin(void) { return this->p_min; }

 protected:
 private:
  Ticker* sample;
  Timeout* measure;

  void begin(void);
  bool begin_sensors(void);
  void reset_samples_arrays(void);
  void tickSample(void);
  void tickMeasure(void);
  void doSample(void);
  void readSensors(void);
  void measureAndSample(void);

  volatile bool started;

  float temperatures[SAMPLES_NUM], humidities[SAMPLES_NUM], pressures[SAMPLES_NUM];
  volatile uint8_t samples;

  float temperature_queue[T_QUEUE_SIZE], humidity_queue[H_QUEUE_SIZE], pressure_queue[P_QUEUE_SIZE];
  uint8_t temperature_queue_idx, humidity_queue_idx, pressure_queue_idx;

  float temperature, humidity;
  float pressure, altitude;
  float t_max, t_min, h_max, h_min, p_max, p_min;

  volatile bool readyToSample;
  volatile bool readyToMeasure;

  const PeripheralsUtils* peripherals;
  BME280* bme280;

  typedef enum {
    NO_MEAN,
    MEAN_BUFFER,
    MEAN_CONTINUOUS,
    MEAN_2_SIGMA
  } mode_t;

  mode_t mode;
};

#endif  // _SENSORS_UTILS_H_