#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define PRESSED 1
#define NOT_PRESSED 0
#define YES 1
#define NO 0

typedef enum {temp_state, h2sensor_state, pwr_state, lcdScreen_state,
 emergency_state, waterStorage_state, h2storage_state, initial_state} device_state;
volatile device_state current_state = initial_state; 

typedef enum {temp_screen, h2sensor_screen, pwr_screen,
 emergency_screen, waterStorage_screen, h2storage_screen, idle} screen_state;
volatile screen_state screen_current_state = idle; 

typedef enum {e_wait_press, e_p_debounce, e_wait_release, e_r_debounce, e_alarm} emergency_button_state; 
volatile emergency_button_state emergency_button = e_wait_press; 
volatile int stopButtonState = NOT_PRESSED; 

typedef enum {wait_press, p_debounce, wait_release, r_debounce, alarm} lcd_button_state; 
volatile lcd_button_state lcd_button = wait_press; 

volatile int emergency = NO;

//General function to check sensor thresholds
bool check_sensor(int sensor_value, int limit, int &flag) {
    if (sensor_value > limit) {
        flag++;
    } else {
        flag = 0;
    }
    return flag > 1;
}

int main(){

  //Initalizations of sensors and timers


  //Initialize software functionality
  sei(); //initalizes global intturupts for buttons

  //initalize variables
    /*Flags will chagne value when a problem is detected, value of 1 = check sesnor 
    again after brief pause, value = 2 means go to emergency state */
  int tempFlag = 0; 
  int h2sensorFlag = 0;
  int pwrFlag = 0;
  int waterFlag = 0;
  int h2storageFlag = 0;

    //sesnor readings
  int tempVal1 = 0;
  int tempVal2 = 0;
  int pwrVal = 0;
  int h2sensorVal1 = 0; 
  int h2sensorVal2 = 0;

    //sesnor limits
  int tempLimit = 85; 
  int h2Limit = 100; //value based off of calculations for sensor
  int pwrLimit = 100; 

  //Main loop that runs code
  while(1){


    //State machine that will determine what the device is doing
    switch(current_state){
      case(initial_state):
        //start power flowing to electrolyzer

        current_state = temp_state;
      break;
      
      case(temp_state):
        //read both temp sesnor. maybe can expand state to individualize two sensors for better analysis when temp exeeds limit
        //insert reading temp


        if (check_sensor(tempVal1, tempLimit, tempFlag) || check_sensor(tempVal2, tempLimit, tempFlag)) {
          current_state = emergency_state; //temp sensor is flagged twice 
        } else if (tempFlag == 1) {
          // Insert delay here

          current_state = temp_state; // Remain in temp state to confirm reading
        } else {
          current_state = h2sensor_state; // Move to the next sensor check
        }
      break;
      
      case(h2sensor_state):
        // read both h2 sensors. Could make it idivual per sensor later
        //insert h2 reading
        //originally was: if((tempVal1 || tempVal2) > tempLimit), but the left side will evaulate to true (1) or false (0) and that will mess up the intended logic of comparing to the limit
        if (check_sensor(h2sensorVal1, h2Limit, h2sensorFlag) || check_sensor(h2sensorVal2, h2Limit, h2sensorFlag)) {
          current_state = emergency_state; //temp sensor is flagged twice 
        } else if (h2sensorFlag == 1) {
          // Insert delay here

          current_state = h2sensor_state; // Remain in temp state to confirm reading
        } else {
          current_state = h2storage_state; // Move to the next sensor check
        }
      break;


      case(h2storage_state):
        //looks to see if it has a current which means the h2 storage is getting full
        //insert checking logic/function to change flag

        if(h2storageFlag == 1){
          //insert shorter delay

          current_state = h2storage_state; //remain in h2 storage state state to confirm reading
        }
        else if(h2storageFlag > 1){
          current_state = emergency_state; //h2 storage state is confirmed and device moves to shut down
        }
        else{
          current_state = waterStorage_state; //Next sesnor to check
        }
      break;

      case(waterStorage_state):
        //checks to see if water into electrolyzer is close to empty, will alert user if it is
        //insert checking function/logic

        if(waterFlag == 1){
          //insert short delay

          current_state = waterStorage_state; //remain in water storage state state to confirm reading
        }
        else if((waterFlag > 1) && (waterFlag < 5)){// change 5 later 
          screen_current_state = waterStorage_screen; //updates screen to alert user to fill water
          current_state = pwr_state; // On first alert goes to next sesnor to check

          //could start a timer here as well, instead of use 5 below, may be difficult. device has limited clocks
        }
        else if(waterFlag >= 5){ //5 is artbitrary for now, eventually change to something. Once system timing is figured out
          current_state = emergency_state;
        }
        else{
          current_state = pwr_state; //Next sesnor to check
        }
      break;

      case(pwr_state): //may change are remove becasue fuel cell seems to have this function
        //makes sure output is not over limit
        //insert function to check power output

        if (check_sensor(pwrVal, pwrLimit, pwrFlag)) {
          current_state = emergency_state;
        } else if (pwrFlag == 1) {
          // Insert delay here
          current_state = pwr_state;
        } else {
          current_state = lcdScreen_state;
        }
      break;

      case(lcdScreen_state):
        //State where the lcd screen updates based on state and button pushing

        switch(screen_current_state){ //State machine for lcd screen to decide what to display
          //switching states is done by inturrupt button, some by sesnors, will default to idle after XX amount of time
          case(idle):
            //code to keep screen powered off
          break;

          case(temp_screen):
            //display message from temp meters
            //insert function to turn in values to string for lcd screen

            //insert timing function to go to idle eventually
          break;

          case(h2sensor_screen):
            //display message from h2 sesnor 
            //insert function to turn in values to string for lcd screen

            //insert timing function to go to idle eventually
          break;

          case(h2storage_screen):
            //display value based on if flag has been tripped or not

            //insert timing function to go to idle eventually
          break;

          case(waterStorage_screen):
            //Alert user to put more water into electrolyzer if flagg is tripped

            //insert timing function to go to idle eventually
          break;

          case(pwr_screen):
            //display message from pwr sesnor 
            //insert function to turn in values to string for lcd screen

            //insert timing function to go to idle eventually
          break;

          case(emergency_screen):
            //alert user that device has been shut off and what the reason was
            //Possible reasons: Which sesnor or manual button press

            //insert timing function to go to idle eventually
          break;

          default: 
            screen_current_state = idle;
          break;

        }

        if(emergency == YES){
          current_state = emergency_state; // stays in emergency state if that is the case
          screen_current_state = emergency_screen; //stay in emergency screen
        }
        else{
          current_state = temp_state; //restart cycle through sensors
        }


      break;

      case(emergency_state):
        emergency = YES;
        //insert function to turn off electrolyzer and shut solenoid value
        //insert function/logic to determine reason for emergency
        
        
        //insert logic to determine if button is pressed again then leave from emergency state
        screen_current_state = emergency_screen; // update lcd to alert user to problem
          //insert: go to lcd screen state to update it once issue is identified, then stay in emergency state and wait to leave
      
      break;

      default:
        current_state = temp_state;
      break;
      //end device state machine
    }
    
    //Button state machines

    //lcd button
    switch(lcd_button){ //insert logic and finish button code
      case(wait_press):
      break;

      case(p_debounce): //~1ms delay in this state for debouncing
        // while (count < DEBOUNCE_TIME); //waits for 1ms
        // count = 0;  //set count back to 0 for the next blinking light
        // alarm_count=0;
        // lcd_button = wait_release;
      break;

      case(wait_release):
        // if(alarm_count >= ALARM_ACTIVATE){
        //   lcd_button=alarm;
        // }
      break;

      case(r_debounce): //~1ms delay in this state for debouncing
        // while (count < DEBOUNCE_TIME); //waits for 1ms
        // count = 0;  //set count back to 0 for the next blinking light
        // lcd_button = wait_press;
      break;

      case(alarm):
        // alarmPress();
        // lcd_button=r_debounce;
      break;
    }

    //emergency button
    switch(emergency_button){ //insert logic and finish button code
      case(wait_press):
      break;

      case(p_debounce): //~1ms delay in this state for debouncing
        // while (count < DEBOUNCE_TIME); //waits for 1ms
        // count = 0;  //set count back to 0 for the next blinking light
        // alarm_count=0;
        // lcd_button = wait_release;
      break;

      case(wait_release):
        // if(alarm_count >= ALARM_ACTIVATE){
        //   lcd_button=alarm;
        // }
      break;

      case(r_debounce): //~1ms delay in this state for debouncing
        // while (count < DEBOUNCE_TIME); //waits for 1ms
        // count = 0;  //set count back to 0 for the next blinking light
        // lcd_button = wait_press;
      break;

      case(alarm):
        // alarmPress();
        // lcd_button=r_debounce;
      break;
    }
  


  }
}

//Button inturrupts

/* could maybe use this timing code for something
ISR(TIMER0_COMPB_vect){
  alarm_count++;
}*/

//lcd button
//will need to update the vector for out case
ISR(PCINT1_vect){ //We are using PCINT8, so we need the vector for PCINT1 (Need to look at data shee to figure it out)
  switch(lcd_button){
    case(wait_press): //activated when pin change interrupt occurs and goes to debounce state
      lcd_button = p_debounce;
      //insert change logic

    break;

    case(wait_release): //activated when pin change interrupt occurs and goes to debounce state
      lcd_button = r_debounce;
    break;
    
  }
}

//emergency button
//will need to update the vector for out case
ISR(PCINT1_vect){ //We are using PCINT8, so we need the vector for PCINT1 (Need to look at data shee to figure it out)
  switch(emergency_button){
    case(e_wait_press): //activated when pin change interrupt occurs and goes to debounce state
      emergency_button = e_p_debounce;
      //insert change logic

    break;

    case(e_wait_release): //activated when pin change interrupt occurs and goes to debounce state
      emergency_button = e_r_debounce;
    break;
    
  }
}

//Need to somehow merge the two ISRs, having two with the same vector could cause issues
//Maybe do this instead? 
/*ISR(PCINT1_vect) {
    // Handle LCD button logic
    switch(lcd_button) {
        case(wait_press): // activated when a pin change interrupt occurs
            lcd_button = p_debounce;
            // Insert additional LCD button handling logic here
            break;
        case(wait_release):
            lcd_button = r_debounce;
            // Insert additional LCD button handling logic here
            break;
        default:
            break;
    }

    // Handle Emergency button logic
    switch(emergency_button) {
        case(e_wait_press): // activated when a pin change interrupt occurs
            emergency_button = e_p_debounce;
            // Insert additional emergency button handling logic here
            break;
        case(e_wait_release):
            emergency_button = e_r_debounce;
            // Insert additional emergency button handling logic here
            break;
        default:
            break;
    }
}
*/
//Or we could try this to reduce the amount of code:
/*ISR(PCINT1_vect) { 
    // Check LCD button state transitions
    if (condition for lcd button press) {  
        if (lcd_button == wait_press) lcd_button = p_debounce;  
        else if (lcd_button == wait_release) lcd_button = r_debounce;
    }  

    // Check Emergency button state transitions
    if (condition for emergency button press) {  
        if (emergency_button == e_wait_press) emergency_button = e_p_debounce;  
        else if (emergency_button == e_wait_release) emergency_button = e_r_debounce;
    }  
}*/