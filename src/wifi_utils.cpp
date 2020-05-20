#include "wifi_utils.hpp"

ESP01_OMS *wifi = NULL;

Timeout queryTimeout;
volatile uint8_t queryState = 0;
volatile bool querySucceded = false;
volatile bool queryExecute = false;

char schedule[SCHEDULE_MAX_SIZE];

static const char delimit_start[] = "$#";
static const char delimit_end = '$';

void query_state() {
  querySucceded = false;
  queryExecute = true;
}

bool begin_wifi(void) {
  wifi = new ESP01_OMS(uart);
  return (wifi->begin());
}

bool begin_schedule(void) {
  memset(schedule, 0, SCHEDULE_MAX_SIZE * sizeof(char));
}

bool query_flush(void) {
  queryExecute = false;
  wifi->flushBufferAndSerial();
  queryState = STATE_START;
  queryTimeout.attach(&query_state, 0.1);
  return (true);
}
bool query_start(void) {
  queryExecute = false;
  wifi->get_serial_chn()->printf(
      "AT+CIPSTART=\"TCP\",\"minegrado.ovh\",80\r\n");
  queryState = STATE_LINKED;
  queryTimeout.attach(&query_state, 3);
  return (true);
}
bool query_linked(void) {
  queryExecute = false;

  char c = (char)0x00;
  bool f_OK = false, f_Linked = false;
  uint8_t i = 0;
  char buff_OK[] = "OK";
  char buff_Linked[] = "Linked";

  do {
    c = wifi->i_get_char();

    if (!f_OK) {
      if (c == buff_OK[i]) {
        ++i;
      } else {
        i = 0;
      }
      if (i == __strlen(buff_OK)) {
        f_OK = true;
        i = 0;
      }
    } else if (!f_Linked) {
      if (c == buff_Linked[i]) {
        ++i;
      } else {
        i = 0;
      }
      if (i == __strlen(buff_Linked)) {
        f_Linked = true;
        i = 0;
      }
    }
  } while (c);
  querySucceded = (f_OK && f_Linked);
  if (querySucceded) {
    queryState = STATE_SEND;
    queryTimeout.attach(&query_state, 0.5);
  }
  return (querySucceded);
}
bool query_send(void) {
  queryExecute = false;
  wifi->get_serial_chn()->printf("AT+CIPSEND=%d\r\n",
                                 __strlen(ESP01_OMS::http_query));
  queryState = STATE_HTTP;
  queryTimeout.attach(&query_state, 3);
  return (true);
}
bool query_http(void) {
  queryExecute = false;

  char c = (char)0x0;
  do {
    c = wifi->i_get_char();
  } while (c != 0 && c != '>');

  wifi->get_serial_chn()->printf(ESP01_OMS::http_query);
  queryState = STATE_READ;
  queryTimeout.attach(&query_state, 3);
  return (true);
}
bool query_read(void) {
  queryExecute = false;

  bool found_start = false, found_end = false;
  uint16_t i = 0;
  char c = (char)0x0;
  do {
    c = wifi->i_get_char();

    if (!found_start) {
      if (c == delimit_start[i])
        ++i;
      else
        i = 0;
      if (i == __strlen(delimit_start)) {
        found_start = true;
        i = 0;
      }
    } else if (found_start && !found_end) {
      if (c == delimit_end) {
        found_end = true;
        schedule[i++] = (char)0x0;
        schedule[i] = (char)0x0;
      } else {
        schedule[i++] = c;
      }
    }
  } while (wifi->is_pending_data());

  wait_ms(2000);
  querySucceded = true;
  if (querySucceded) {
    queryState = STATE_FLUSH;
    queryTimeout.attach(&query_state, 5);
  }
  return (querySucceded);
}
bool query_fsm(void) {
  if (queryExecute) {
    queryExecute = false;
    switch (queryState) {
      case STATE_FLUSH:
        querySucceded = query_flush();
        break;
      case STATE_START:
        querySucceded = query_start();
        break;
      case STATE_LINKED:
        querySucceded = query_linked();
        break;
      case STATE_SEND:
        querySucceded = query_send();
        break;
      case STATE_HTTP:
        querySucceded = query_http();
        break;
      case STATE_READ:
        querySucceded = query_read();
        break;
      default:
        break;
    }
  }
}

void start_sync(void) {
  queryExecute = false;
  queryTimeout.attach(&query_state, 5);
}