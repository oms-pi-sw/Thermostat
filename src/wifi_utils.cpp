#include "wifi_utils.hpp"

ESP01_OMS *wifi = NULL;

bool begin_wifi(void) {
  wifi = new ESP01_OMS(uart);
  return (wifi->begin());
}