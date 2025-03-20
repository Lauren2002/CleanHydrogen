// Author:   Ethan Burnett Nguyen      
// Net ID:   eburnettnguyen
// Date:     11/6/2023   
// Assignment:     Lab 3
//----------------------------------------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

void initTimer1();
void delayS(int delay);
void initTimer0();
void delayMs(unsigned int delay); 
void delayUs(unsigned int delay);

#endif
