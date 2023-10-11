#include "util.h"
#include <Arduino.h>

int pressBotao(int bt, int keyUp, int *lastStatus, int *btmilis) {
  int status = digitalRead(bt);

  if (status != *lastStatus) {
    *lastStatus = status;
  
    if (status == keyUp) {
      return millis() - *btmilis;
    }

    *btmilis = millis();
  }

  return 0;
}

bool atualizaRele(int pin, int *status) {
  int newStatus = digitalRead(pin);

  if (newStatus != *status) {
    *status = newStatus;

    return true;
  }

  return false;
}
