/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
#define JOYBUTTON1 (1<<24)
#define JOYBUTTON2 (1<<25)

void Switch_Init(void){
    IOMUX->SECCFG.PINCM[PA17INDEX] = 0X00040081; // GPIO input
    IOMUX->SECCFG.PINCM[PB16INDEX] = 0X00040081;
}
// return current state of switches
// PA17 (SW1) is bit 0, PB16 (SW2) is bit 1
uint32_t Switch_In(void){
    return (GPIOA->DIN31_0&0x00020000)>>17 | (GPIOB->DIN31_0&0x00010000)>>15;;
}


