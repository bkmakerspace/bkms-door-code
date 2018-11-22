#include "Arduino.h"

long millis() {
  return duration_cast< milliseconds >(
      system_clock::now().time_since_epoch()
  ).count();
}
