#include "IPA_MatrixSymbols.h"
#include "Arduino.h"

IPA_MatrixSymbols::IPA_MatrixSymbols(uint8_t pin) {
  _pin = pin;
  init(pin);
}

void IPA_MatrixSymbols::init(uint8_t pin) {
  pinMode(pin, OUTPUT);
}

// void IPA_MatrixSymbols::draw(unsigned int frequency, unsigned long duration = 0){
  
// }