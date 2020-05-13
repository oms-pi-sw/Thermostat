#include "sensors_utils.hpp"

BME280 *bme280 = NULL;

float temperatures[100], humidities[100], pressures[100];
volatile uint8_t samples = 0;

// Temperature, humidity, pressure, altitude
float temperature = 0., humidity = 0.;
float pressure = 0., altitude = 0.;
float t_max = 0, t_min = 0, h_max = 0, h_min = 0, p_max = 0, p_min = 0;

volatile bool readyToSample = false;
volatile bool readyToMeasure = true;

void begin_sensors(void) {
  bme280 = NULL;
  samples = 0;
  temperature = 0., humidity = 0.;
  pressure = 0.;
  altitude = 0.;
  t_max = 0;
  t_min = 0;
  h_max = 0;
  h_min = 0;
  p_max = 0;
  p_min = 0;

  readyToSample = false;
  readyToMeasure = true;

  bme280 = new BME280(i2c, 0x76);

  bme280->reset();
  wait_ms(2000);
  bme280->init();
  wait_ms(1000);
}

void reset_samples_arrays(void) {
  for (uint8_t i = 0; i < SAMPLES_NUM; i++) {
    temperatures[i] = 0.;
    humidities[i] = 0.;
    pressures[i] = 0.;
  }
}

void tickSample(void) { readyToSample = true; }

void tickMeasure(void) { readyToMeasure = true; }

void doSample(void) {
  if (samples < SAMPLES_NUM) {
    if (!first && !std::isnan(temperature) && !std::isinf(temperature) &&
        !std::isnan(humidity) && !std::isinf(humidity) &&
        !std::isnan(pressure) && !std::isinf(pressure)) {
      temperatures[samples] = temperature;
      humidities[samples] = humidity;
      pressures[samples] = pressure;
      ++samples;
    }
  } else {
    for (uint8_t i = 0; i < SAMPLES_NUM - 1; i++) {
      temperatures[i] = temperatures[i + 1];
      humidities[i] = humidities[i + 1];
      pressures[i] = pressures[i + 1];
    }
    temperatures[SAMPLES_NUM - 1] = temperature;
    humidities[SAMPLES_NUM - 1] = humidity;
    pressures[SAMPLES_NUM - 1] = pressure;
  }
}

void readSensors(void) {
  statusLed = !statusLed;
  bme280->trigger();
  while (bme280->busy()) {
    wait_ms(10);
  }
  float _t1 = bme280->getTemperature();
  float _h1 = bme280->getHumidity();
  float _p1 = bme280->getPressure();

  if (!isnan(_t1)) {
    temperature = _t1;
    if (first) {
      t_min = temperature;
      t_max = temperature;
    } else {
      if (temperature < t_min) {
        t_min = temperature;
      }
      if (temperature > t_max) {
        t_max = temperature;
      }
    }
  }

  if (!isnan(_h1)) {
    humidity = _h1;
    if (first) {
      h_min = humidity;
      h_max = humidity;
    } else {
      if (humidity < h_min) {
        h_min = humidity;
      }
      if (humidity > h_max) {
        h_max = humidity;
      }
    }
  }

  if (!isnan(_p1)) {
    pressure = _p1;
    if (first) {
      p_min = pressure;
      p_max = pressure;
    } else {
      if (pressure < p_min) {
        p_min = pressure;
      }
      if (pressure > p_max) {
        p_max = pressure;
      }
    }
  }
}