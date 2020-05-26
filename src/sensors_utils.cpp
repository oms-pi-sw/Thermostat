#include "sensors_utils.hpp"

SensorsUtils::SensorsUtils(const PeripheralsUtils* peripherals) {
  this->peripherals = peripherals;
  this->bme280 = new BME280(this->peripherals->getI2C(), 0x76);

  this->sample = new Ticker();
  this->measure = new Timeout();

  begin();
}

SensorsUtils::~SensorsUtils() {
  delete this->bme280;
  delete this->sample;
  delete this->measure;
}

void SensorsUtils::begin(void) {
  this->mode = MEAN_CONTINUOUS;

  this->samples = 0;

  this->temperature = ((float)(0xFFFFFFFF));
  this->humidity = ((float)(0xFFFFFFFF));
  this->pressure = ((float)(0xFFFFFFFF));
  this->altitude = ((float)(0xFFFFFFFF));
  this->t_max = ((float)(0xFFFFFFFF));
  this->t_min = ((float)(0xFFFFFFFF));
  this->h_max = ((float)(0xFFFFFFFF));
  this->h_min = ((float)(0xFFFFFFFF));
  this->p_max = ((float)(0xFFFFFFFF));
  this->p_min = ((float)(0xFFFFFFFF));

  this->temperature_queue_idx = 0;
  this->humidity_queue_idx = 0;
  this->pressure_queue_idx = 0;

  this->readyToSample = false;
  this->readyToMeasure = true;

  this->started = false;

  this->reset_samples_arrays();
}

bool SensorsUtils::start(void) {
  if (this->bme280 == nullptr) return (false);
  if (this->started) return (false);

  this->temperature_queue_idx = 0;
  this->humidity_queue_idx = 0;
  this->pressure_queue_idx = 0;

  this->bme280->reset();
  wait_ms(2000);
  this->bme280->init();
  wait_ms(1000);

  sample->attach(callback(this, &SensorsUtils::tickSample), SAMPLE_T);
  measure->attach(callback(this, &SensorsUtils::tickMeasure), 5);

  return (true);
}

void SensorsUtils::measureAndSample(void) {
  if (readyToMeasure) {
    readSensors();
    measure->attach(callback(this, &SensorsUtils::tickMeasure), MEASURE_T);
    readyToMeasure = false;
  }
  if (readyToSample) {
    doSample();
    readyToSample = false;
  }
}

void SensorsUtils::reset_samples_arrays(void) {
  samples = 0;

#ifndef USE_MEMSET
  for (uint8_t i = 0; i < SAMPLES_NUM; i++) {
    this->temperatures[i] = 0.;
    this->humidities[i] = 0.;
    this->pressures[i] = 0.;
  }
#else
  memset(this->temperatures, 0x0, SAMPLES_NUM * sizeof(float));
  memset(this->humidities, 0x0, SAMPLES_NUM * sizeof(float));
  memset(this->pressures, 0x0, SAMPLES_NUM * sizeof(float));
#endif
}

void SensorsUtils::tickSample(void) { this->readyToSample = true; }

void SensorsUtils::tickMeasure(void) { this->readyToMeasure = true; }

void SensorsUtils::doSample(void) {
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

void SensorsUtils::readSensors(void) {
  bme280->trigger();
  while (bme280->busy()) {
    wait_ms(10);
  }
  float _t1 = bme280->getTemperature();
  float _h1 = bme280->getHumidity();
  float _p1 = bme280->getPressure();

  _t1 = Utils::__round2d(_t1);
  _h1 = Utils::__round2d(_h1);
  _p1 = Utils::__round2d(_p1);

  if (isnan(_t1) || isnan(_h1) || isnan(_p1) || isinf(_t1) || isinf(_h1) || isinf(_p1)) return;

  bool t_reg = true;
  bool h_reg = true;
  bool p_reg = true;

  if (this->mode == MEAN_BUFFER) {
    temperature_queue[temperature_queue_idx++] = _t1;
    humidity_queue[humidity_queue_idx++] = _h1;
    pressure_queue[pressure_queue_idx++] = _p1;

    t_reg = (temperature == ((float)(0xFFFFFFFF)) ? true : false);
    if (temperature_queue_idx >= T_QUEUE_SIZE) {
      temperature_queue_idx = 0;
      t_reg = true;
      _t1 = Utils::__mean(temperature_queue, T_QUEUE_SIZE);
    }
    h_reg = (humidity == ((float)(0xFFFFFFFF)) ? true : false);
    if (humidity_queue_idx >= H_QUEUE_SIZE) {
      humidity_queue_idx = 0;
      h_reg = true;
      _h1 = Utils::__mean(humidity_queue, H_QUEUE_SIZE);
    }
    p_reg = (pressure == ((float)(0xFFFFFFFF)) ? true : false);
    if (pressure_queue_idx >= P_QUEUE_SIZE) {
      pressure_queue_idx = 0;
      p_reg = true;
      _p1 = Utils::__mean(pressure_queue, P_QUEUE_SIZE);
    }
  } else if (this->mode == MEAN_CONTINUOUS) {
    if (temperature_queue_idx < T_QUEUE_SIZE) {
      temperature_queue[temperature_queue_idx++] = _t1;
    } else {
      for (uint8_t i = 0; i < (T_QUEUE_SIZE - 1); i++) {
        temperature_queue[i] = temperature_queue[i + 1];
      }
      temperature_queue[T_QUEUE_SIZE - 1] = _t1;
    }
    if (humidity_queue_idx < H_QUEUE_SIZE) {
      humidity_queue[humidity_queue_idx++] = _h1;
    } else {
      for (uint8_t i = 0; i < (T_QUEUE_SIZE - 1); i++) {
        humidity_queue[i] = humidity_queue[i + 1];
      }
      humidity_queue[H_QUEUE_SIZE - 1] = _h1;
    }
    if (pressure_queue_idx < P_QUEUE_SIZE) {
      pressure_queue[pressure_queue_idx++] = _p1;
    } else {
      for (uint8_t i = 0; i < (T_QUEUE_SIZE - 1); i++) {
        pressure_queue[i] = pressure_queue[i + 1];
      }
      pressure_queue[P_QUEUE_SIZE - 1] = _p1;
    }
    _t1 = Utils::__mean(temperature_queue, temperature_queue_idx);
    _h1 = Utils::__mean(humidity_queue, humidity_queue_idx);
    _p1 = Utils::__mean(pressure_queue, pressure_queue_idx);
  } else if (this->mode == MEAN_2_SIGMA) {
  } else {
  }

  _t1 = Utils::__round2d(_t1);
  _h1 = Utils::__round2d(_h1);
  _p1 = Utils::__round2d(_p1);

  if (t_reg && !isnan(_t1)) {
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

  if (h_reg && !isnan(_h1)) {
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

  if (p_reg && !isnan(_p1)) {
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