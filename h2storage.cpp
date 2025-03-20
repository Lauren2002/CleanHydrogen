# include "h2storage.h"

void initH2storage(){
    //intializes pin XX to output 1 and pin XX to expect a signal

    DDRH  |= (1<< DDH5); //Initalize Port h5 or pin 8 for output

    DDRH  &= ~(1<< DDH6); //Initalize Port h6 or pin 9 for input

    PORTH |= (1<<PORTH5); //turn on signal at Port h5 or pin 8 

    PORTH |= (1<<PORTH6); //enable pull up register for Port h6 or Pin 9

}

void checkH2storageLevel(int &flag){
    char val;

    val = PINH6; //sets value of Port h6 or Pin 9 on the board to val

    if(val != 1){   //Not sure if PINH6 returns exactly 1s and 0s. Need to test
        flag++;
    }
    else{
        flag=0;
    }
}

