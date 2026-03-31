#include <LiquidCrystal.h> //for the lcd display
#include <Wire.h>         //I2C library, for MLX90614-BCC
#include <SparkFunMLX90614.h> //for the temp sensor
#include <SPI.h>         //SPI library, for the sd card module
#include <SD.h>         //for the sd card 

unsigned long lastMillis = 0; // stores the time where the board was last on

//RPM
const unsigned long MIN_PULSE_INTERVAL_MICROS = 6000UL; // 6 ms
const int hallOutPin = 2; 
volatile unsigned int pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
unsigned int rpm = 0;

//CVT-TEMP
IRTherm temp_sensor;       // Create an IRTherm object to interact with throughout
const int WARNING_LED = 9; //turns on if CVT belt temp is nearing its melting point

//SD CARD
boolean serialPrint = false;   //need to set to true to run
char *logFile = "LOG00001.csv"; //name must be 8characters.csv
int readPushButtonPin = 9;
boolean headerWritten = false;

//initializing the lcd
LiquidCrystal lcd(12,11,7,6,5,4); //RS, E, D4, D5, D6, D7 

//ISR
void anotherRevolution() {
  unsigned long currentTime = micros();
  if (currentTime - lastPulseTime >= MIN_PULSE_INTERVAL_MICROS) { // debounce so that any unwanted noise is not considered a pulseCount
    pulseCount++;
    lastPulseTime = currentTime;
  }
}

void setup() {
  pinMode(hallOutPin, INPUT_PULLUP);
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WARNING_LED, OUTPUT);
  digitalWrite(WARNING_LED, LOW);
  Serial.begin(115200);
  delay(1000); //Waiting for Serial Monitor to open, before displaying any messages

  lcd.begin(16,2);
  lcd.setCursor(0,0); lcd.print("Initializing");
  lcd.setCursor(0,1); lcd.print("R/T Monitor...");

  Wire.begin(); 
  if (temp_sensor.begin() == false){ // Initializing thermal IR sensor
    Serial.println("Temperature sensor failed. Check wiring.");
    while(1);
  }
  Serial.println("Temperature sensor online!");
  temp_sensor.setUnit(TEMP_C); 

  attachInterrupt(digitalPinToInterrupt(hallOutPin), anotherRevolution, FALLING);
}

void loop() {
  unsigned long millisNow = millis();

  // updates every 1 second
  if (millisNow - lastMillis >= 1000) {
    lastMillis = millisNow;

    noInterrupts();
    // takes pulse count, reset for next second
    unsigned int savedPulses = pulseCount;
    pulseCount = 0;
    interrupts();

    if (temp_sensor.read()){ // On success, read() will return 1, on fail 0.
      // Use the object() and ambient() functions to grab the object and ambient temperatures
	    // They'll be floats, calculated out to the unit you set with setUnit().
      float object_temp = temp_sensor.object();
      float ambient_temp = temp_sensor.ambient();
      Serial.print("Object: " + String(object_temp, 2));
      Serial.println(" C");
      Serial.print("Ambient: " + String(ambient_temp, 2));
      Serial.println(" C");
      Serial.println();
    }
    if (object_temp >= 110){  //110C is a common value at which belt degradation occurs
      digitalWrite(WARNING_LED, HIGH);
    }
    if (ambient_temp >= 125) {  //85C or 125C gotta check temp sensor manual
      Serial.println("The data being collected after this point may be inaccurate."); //print this in sd card log?
    }

    rpm = savedPulses * 20; //simplified -->(savedPulses / 3.0) * 60 and 3.0 because we have 3 magnets on the primary

    //LCD print
    /*
    lcd.setCursor(0,0);
    lcd.print("                ");  //overwriting old text instead of lcd.clear() because it tends to make screens flicker
    lcd.setCursor(3,0);
    lcd.print("BAJA PINK");
    */
    lcd.setCursor(0,0);
    lcd.print("                ");      
    lcd.setCursor(0,0);
    lcd.print("RPM");
    lcd.setCursor(6,0);
    lcd.print(rpm);

    lcd.setCursor(0,1);                      //<-------OPTIMIZE
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("CVT");
    lcd.setCursor(6,0);
    lcd.print(object_temp);
    lcd.setCursor()

    //printing to serial
    Serial.print("Pulses: ");
    Serial.print(savedPulses); Serial.print(", ");
    Serial.print("RPM: ");
    Serial.println(rpm);

    /*
    //so if shaft is rotating the LED wil turn on
    if (savedPulses > 0) {
      digitalWrite(LED_BUILTIN, HIGH);
    } 
    else { //if shaft doesn't spin then no voltage disrupted due to magnet hence LED off to indicate that the shaft ain't spinnin'
      digitalWrite(LED_BUILTIN, LOW);  
    }
    */
  }
}