/*
 * LED.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table

// initialize your LEDs
void LED_Init(void){
    IOMUX->SECCFG.PINCM[PB17INDEX] = 0X00000081; // GPIO output
    IOMUX->SECCFG.PINCM[PA16INDEX] = 0X00000081;

    GPIOB->DOE31_0 |= (1<<17); // Enable output pins
    GPIOA->DOE31_0 |= (1<<16);
    GPIOB->DOUT31_0 &= ~(1<<17); // Deactivate upon start
    GPIOA->DOUT31_0 &= ~(1<<16);
}
// data specifies which LED to turn on
// bit 0 is red, bit 1 is green
void LED_On(uint32_t data){
    if(data&0x1){
        GPIOB->DOUTSET31_0 = (1<<17);
    }
    if(data&0x2){
        GPIOA->DOUTSET31_0 = (1<<16);
    }
    // use DOUTSET31_0 register so it does not interfere with other GPIO
  
}

// data specifies which LED to turn off
void LED_Off(uint32_t data){
    if(data&0x1){
        GPIOB->DOUTCLR31_0 = (1<<17);
    }
    if(data&0x2){
        GPIOA->DOUTCLR31_0 = (1<<16);
    }
    // use DOUTCLR31_0 register so it does not interfere with other GPIO
 
}

// data specifies which LED to toggle
void LED_Toggle(uint32_t data){
    if(data&0x1){
        GPIOB->DOUTTGL31_0 = (1<<17);
    }
    if(data&0x2){
        GPIOA->DOUTTGL31_0 = (1<<16);
    }
    // use DOUTTGL31_0 register so it does not interfere with other GPIO
 
}
