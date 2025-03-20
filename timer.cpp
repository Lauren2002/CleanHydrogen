// Author: Eli Jacobson
// Net ID:  jacobsone
// Date:  10/23/23
// Assignment:     Lab 3
//----------------------------------------------------------------------//

#include "timer.h"
//You many use any timer you wish for the microsecond delay and the millisecond delay


/* Initialize timer 1, you should not turn the timer on here. Use CTC mode  .*/
void initTimer1(){
	//CTC mode: WGM10 = 0, WGM11 = 0, WGM12 = 1, WGM13 = 0
    TCCR1A &= ~(1<<WGM10);
    TCCR1A &= ~(1<<WGM11);
    TCCR1B |= (1<<WGM12);
    TCCR1B &= ~(1<<WGM13);

}

void delayS(int delay){

    for(int i=0;i <delay; i++){
        delayMs(1000);
    }

}

/* This delays the program an amount of microseconds specified by unsigned int delay.
*/
void delayUs(unsigned int delay){
    int count = 0;
    
    //CS12 = 0, CS11 = 0, CS10=1; for prescaler of 1 (no prescaling)
    TCCR1B &= ~(1<<CS12);
    TCCR1B &= ~(1<<CS11);
    TCCR1B |= (1<<CS10);
    //#counts = (td*fCPU)/Prescaler - 1 = (1u*16M)/1 - 1 = 15 counts
    OCR1A = 15;

    while (count < delay){
        //Set output compare match flag (OCF1A) to 1, to rest the flag to 0 
        TIFR1 |= (1<<OCF1A);
        //clear count register
        TCNT1 = 0;

        while(!(TIFR1 &(1<<OCF1A)));    //wait
    
        count++;    //count number of times we count 1us
    }
}

/* Initialize timer 0, you should not turn the timer on here.
* You will need to use CTC mode */
void initTimer0(){
    //initialize CTC mode of operation (Datasheet table 16.8 for info) 
    //WGM00 = 0, WGM01 = 1, WGM02 = 0 for CTC mode

    TCCR0A &= ~(1<<WGM00); // WGM00 = 0
    TCCR0A |= (1<<WGM01);  // WGM01 = 1
    TCCR0B &= ~(1<<WGM02); // WGM02 = 0
}

/* This delays the program an amount specified by unsigned int delay.
* Use timer 0. Keep in mind that you need to choose your prescalar wisely
* such that your timer is precise to 1 millisecond and can be used for
* 100-2000 milliseconds
*/

void delayMs(unsigned int delay){
   //funtion takes # of miliseconds of delay that is wanted, then delays for that many miliseconds
    //does this by counting to 1ms the # of times they put in the function
    
    int count=0;
    //Setting prescaler to 64 see table 16.9 for details
    TCCR0B |= (1<<CS00) | (1<<CS01);
    TCCR0B &= ~(1<<CS02);
    //#counts = (td*fCPU)/Prescaler - 1 = (1m*16M)/64 - 1 = 249
    OCR0A = 249;

    while(count < delay){
        //Set output compare match flag (OCF0A) to 1, to rest the flag to 0 (weird, but the way it works)
        TIFR0 |= (1<<OCF0A);
        //clear count register
        TCNT0 = 0;

        //Poll OCF0A flag until it is set/is logic 1 
        while(!(TIFR0 &(1<<OCF0A))); // this code loops until flag is raised, this is the delaying code
        count++;
    }
}