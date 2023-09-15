/*

  IPA_MatrixSymbols.h - Library for draw symbols on led matrix.

  Created by Peshkov Igor, Aug 19, 2023.

  Released into the public domain.

*/
#ifndef IPA_MatrixSymbols_h
#define IPA_MatrixSymbols_h

#include "Arduino.h"

#define SYMBOL_Z = [];

class IPA_MatrixSymbols
{
  public:
    IPA_MatrixSymbols(uint8_t pin);
     void init(uint8_t pin);
     void play(unsigned int frequency, unsigned long duration = 0);
     void stop();
     void pause();
  private:
    uint8_t _pin;
};

#endif