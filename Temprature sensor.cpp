 #include <OneWire.h>
   #include <DallasTemperature.h>

    //Download the OneWire and DallasTemperature libraries
   // Data wire is connected to GPIO 4
   #define ONE_WIRE_BUS 4

   OneWire oneWire(ONE_WIRE_BUS);
   DallasTemperature sensors(&oneWire);

   void setup(void) {
     Serial.begin(9600);
     sensors.begin();
   }

   void loop(void) { 
      sensors.requestTemperatures(); 
      
      delay(750); 
      
      float tempC = sensors.getTempCByIndex(0);
      Serial.print("Temperature: ");
      Serial.print(tempC);
      Serial.println("°C");
      delay(1000);
   }