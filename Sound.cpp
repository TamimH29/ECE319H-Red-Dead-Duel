// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jonathan Valvano
// 11/15/2021 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

const static uint8_t* sound2play;
static uint32_t soundCount;
static uint32_t I;

void SysTick_IntArm(uint32_t period, uint32_t priority){
    SysTick->CTRL = 0;         // disable SysTick during setup
    SysTick->LOAD = period-1;  // reload value
    SysTick->VAL = 0;          // any write to current clears it
    SCB->SHP[1] = SCB->SHP[1]&(~0xC0000000)|(priority << 29); // set priority
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
    // 80M/7256 = 11.025kHz
    SysTick_IntArm(7256, 1);
    DAC5_Init();
}
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
      DAC5_Out(sound2play[I]); // output one
      I++;
      if(I >= soundCount) {
          SysTick->CTRL = 0;  // disable SysTick
      }
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){
    sound2play = pt;
    soundCount = count;
    I = 0;
    SysTick->CTRL = 0x0007;    // enable SysTick with core clock and interrupts
  
}
void Sound_Shoot(void){
    Sound_Start(Shoot, 2282);
}

void Sound_Hit(void){
    Sound_Start(hit, 2195);
}

void Sound_Powerup(void){
    Sound_Start(healthboost, 2098);
}
