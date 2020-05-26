#ifndef _WIFI_UTILS_H_
#define _WIFI_UTILS_H_

#include <mbed.h>

#include "ESP-01_OMS.hpp"
#include "display_utils.hpp"
#include "peripherals.hpp"
#include "utils.hpp"

#define SCHEDULE_MAX_SIZE 512

class WifiUtils {
 public:
  WifiUtils(const PeripheralsUtils* periferals, DisplayUtils* display);
  virtual ~WifiUtils();
  bool start(void);
  void cycle(void);

 protected:
 private:
  void begin(void);

  const PeripheralsUtils* periferals;
  DisplayUtils* display;

  char schedule[SCHEDULE_MAX_SIZE];

  class Query {
   public:
    Query() {
      this->state = IDLE;
      this->execute = false;
      this->succeded = false;
    }
    virtual ~Query() {}

    typedef enum STATES {
      IDLE,
      FLUSH,
      START,
      LINKED,
      SEND,
      QUERY,
      READ
    } states;

    states getState() { return this->state; }
    bool isSucceded() { return this->succeded; }
    bool canExecute() { return this->execute; }

   protected:
   private:
    states state;
    bool succeded;
    bool execute;
  };

  class Samples {
   public:
    Samples() {
      this->state = IDLE;
      this->execute = false;
      this->succeded = false;
    }
    virtual ~Samples() {}

    typedef enum STATES {
      IDLE,
      FLUSH,
      START,
      LINKED,
      SEND,
      COMMAND,
      READ
    } states;

    states getState() { return this->state; }
    bool isSucceded() { return this->succeded; }
    bool canExecute() { return this->execute; }

   protected:
   private:
    states state;
    bool succeded;
    bool execute;
  };
};

#endif  // _WIFI_UTILS_H_