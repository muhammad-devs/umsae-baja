/****************************************************************************** 

Hardware Hookup (if you're not using the eval board):
MLX90614 ------------- Arduino
  VDD ------------------ 3.3V
  VSS ------------------ GND
  SDA ------------------ SDA 
  SCL ------------------ SCL 
  
An LED can be attached to pin 8 to monitor for any read errors.

******************************************************************************/

#include <Wire.h> // I2C library, required for MLX90614-BCC
#include <SparkFunMLX90614.h> 

IRTherm temp_sensor; // Create an IRTherm object to interact with throughout
const int WARNING_LED = 9; //turns on if CVT belt temp is nearing its melting point

void setup() 
{
  Serial.begin(115200); 
  delay(1000); //Waiting for Serial Monitor to open, before displaying any messages
  Wire.begin(); 
  
  if (temp_sensor.begin() == false){ // Initialize thermal IR sensor
    Serial.println("Temperature sensor failed. Check wiring.");
    while(1);
  }
  Serial.println("Temperature sensor online!");
  
  temp_sensor.setUnit(TEMP_C); 
  
  pinMode(WARNING_LED, OUTPUT);
  digitalWrite(WARNING_LED, LOW);
}

void loop() 
{   
  if (temp_sensor.read()) // On success, read() will return 1, on fail 0.
  {
    // Use the object() and ambient() functions to grab the object and ambient temperatures
	// They'll be floats, calculated out to the unit you set with setUnit().
    float object_temp = temp_sensor.object();
    float ambient_temp = temp_sensor.ambient();
    Serial.print("Object: " + String(object_temp, 2));
    Serial.println(" C");
    Serial.print("Ambient: " + String(ambient_temp, 2));
    Serial.println(" ");
    Serial.println();

    if (object_temp >= 110){  //110C is a common value at which belt degradation occurs
      digitalWrite(WARNING_LED, HIGH);
    }
    if (ambient_temp >= 125) {  //85C or 125C gotta check temp sensor manual
      Serial.println("The data being collected after this point may be inaccurate."); //print this in sd card log?
    }
  }
  
  delay(1000); //temporary
}
