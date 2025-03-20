// Author: Dale Hetherington
// Net ID:
// Date:
// Assignment:
//
// Description:  Use ADC7 for reading in analog voltage value
//----------------------------------------------------------------------//

#include "hsensor.h"
#include <avr/io.h>
#include <math.h>

//datasheet section 26
void initADC7(){

  // Using AVccc as voltage reference (5 volts)
  ADMUX |= (1 << REFS0);
  ADMUX &= ~(1 << REFS1);

  // determine left or right justified (ADLAR = 0 RIGHT JUSTIFIED)
  ADMUX &= ~(1 << ADLAR);

  // Specify ADC input channel and mode
  //Set ADC0 as single-ended input with MUX[5:0] = 0b000000
  ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0));
  ADMUX &= ~((1 << MUX4) | (1 << MUX3));
  ADCSRB &= ~(1 << MUX5);

  // set Free running
  // ADTS2 = 0 ADST1 = 0 ADST0 = 0
  ADCSRB &= ~(1 << ADTS2) ;
  ADCSRB &= ~(1 << ADTS1);
  ADCSRB &= ~(1 << ADTS0);

  // enable ADC and Auto Trigger
  ADCSRA |= (1 << ADEN)| (1 << ADATE);


  // set the ADC clock frequemcy.  Use a pre-scaler of 128
  // ADC clock frequency is 16 Mhz divided by pre-scaler = 125KHz
  // Sampling rate is 1/ ((1/125K Hz )*(13 clock cycles)) = 9615 Hz
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1<<ADPS0);


  // disable ADC0 pin digital input - pin A0 on board
  DIDR0 |= (1 << ADC0D);

  // start the first ADC conversion
  ADCSRA |= ( 1 << ADSC);
}


unsigned int sendADC(){

    if(ADCSRA & (1 << ADIF)){
    unsigned int result=0;

    result = ADCL;
    result += ((unsigned int)ADCH)<<8;
    return result;
    }
    else{
        return;
    }
}

int convertToPPM(int ADC_val){
    int VOut = (ADC_val * 5)/1024; //10 bit sensor so ADC has 1024 leveles, 5v refrence voltage

    int R0 = 18041; //value caclulated from data sheet based in clean air r0 = (Vcc*RL)/Vout - RL. ADC val in clean air: 146
    int RL = 3000; //based on sensitivity setting

    int RS = ((5*RL) / VOut) - RL; //RS value is resitance value changed based off of gas content

    int ratio = RS/R0; //there is typically 0.5-1ppm of h2 in the air

    int PPM = 2.718*exp((ratio-14.3)/-1.783);
    
}

int getHsensor1(){
    int x;
    x= sendADC();           //Code gets PPM from h2 sensor
    return convertToPPM( x );
}

//True = on False = off
void hsensor_onoff(bool X){
    if(X){
        ADCSRA |= (1 << ADEN);
    }
    else{
          ADCSRA &= ~(1 << ADEN);
    }
}
