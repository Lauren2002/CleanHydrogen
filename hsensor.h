#ifndef HSENSOR_H
#define HSENSOR_H

void initADC7();
unsigned int sendADC();
void hsensor_onoff(bool X);
int convertToPPM(int ADC_val);
int getHsensor1();

#endif
