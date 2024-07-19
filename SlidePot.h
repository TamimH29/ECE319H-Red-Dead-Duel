// SlidePot.h
// Runs on MSPM0
// Provide functions that initialize ADC1 channel 5, PB18 and use a slide pot to measure distance
// Created: 12/29/2023
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#ifndef SLIDEPOT_H
#define SLIDEPOT_H
#define ADCVREF_VDDA 0x000
#include <stdint.h>
#include "../ECE319K_Lab9H/Player.h"

class SlidePot{ private:
  uint32_t data;     // raw ADC value
  int32_t flag;      // 0 if data is not valid, 1 if valid
  uint32_t distance; // distance in 0.001cm
// distance = (slope*data+offset)/4096
  uint32_t slope;    // calibration coeffients
  uint32_t offset;
public:
  SlidePot(uint32_t m, uint32_t b); // initialize slide pot
  void Init(ADC12_Regs *adc12,uint32_t channel1,uint32_t channel2, uint32_t reference);                  // initialize ADC1
  void In(ADC12_Regs *adc12,uint32_t *d1, uint32_t *d2);                // return last ADC sample value (0 to 4095)
  void Move(Player* player, uint32_t data); // Moves players according to data from In
  void Save(uint32_t n);            // save ADC, set semaphore
  void Sync(void);                  // wait for semaphore
  uint32_t Convert(uint32_t n);     // convert ADC raw sample to fixed-point distance, 0.001cm
  float FloatConvert(uint32_t n);   // do not use this function
  uint32_t Distance(void);          // return last distance value (0 to 2000), 0.001cm
};
#endif