#include <LiquidCrystal.h>

const int ledPin = 13;   // LED connected to pin 13 TEMPPPP

const int doutPin = 2;      // Digital Hall sensor output -> pin 2 (interrupt)

const unsigned long MIN_PULSE_INTERVAL_MICROS = 3000UL; // 6 ms

volatile unsigned int pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

unsigned long lastMillis = 0; // stores the time where the board was last on    ---
unsigned int rpm = 0;

// initializing the lcd
LiquidCrystal lcd(12,11,7,6,5,4); // RS, E, D4, D5, D6, D7

// ISR
void anotherRevolution() {
  unsigned long currentTime = micros();
  if (currentTime - lastPulseTime >= MIN_PULSE_INTERVAL_MICROS) { // debounce so that any unwanted noise is not considered a pulseCount
    pulseCount++;
    digitalWrite(ledPin, HIGH);  // Turn LED on 
    lastPulseTime = currentTime;
  }
}

void setup() {
  pinMode(doutPin, INPUT);
  pinMode(ledPin, OUTPUT);
  //pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  lcd.begin(16,2);
  lcd.setCursor(0,0); lcd.print("Initializing");
  lcd.setCursor(0,1); lcd.print("RPM Meter...");

  attachInterrupt(digitalPinToInterrupt(doutPin), anotherRevolution, FALLING);
}

void loop() {
  unsigned long millisNow = millis();
  digitalWrite(ledPin, LOW);   // Turn LED off

  // updates every 1 second
  if (millisNow - lastMillis >= 1000) {
    lastMillis = millisNow;

    noInterrupts();
    // takes pulse count, reset for next second
    unsigned int savedPulses = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = savedPulses * 20; //simplified -->(savedPulses / 3.0) * 60 and 3.0 because we have 3 magnets on the primary

    // LCD print
    lcd.setCursor(0,0);
    lcd.print("                ");  //overwriting old text instead of lcd.clear() because it tends to make screens flicker
    lcd.setCursor(3,0);
    lcd.print("BAJA PINK");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("RPM");
    lcd.setCursor(6,1);
    lcd.print(rpm);

    // printing to serial
    Serial.print("Pulses: ");
    Serial.print(savedPulses); Serial.print(", ");
    Serial.print("RPM: ");
    Serial.println(rpm);

    /*
    // so if shaft is rotating the LED wil turn on
    if (savedPulses > 0) {
      digitalWrite(LED_BUILTIN, HIGH);
    } 
    else { // if shaft doesn't spin then no voltage disrupted due to magnet hence LED off to indicate that the shaft ain't spinnin'
      digitalWrite(LED_BUILTIN, LOW);  
    }
    */
  }
}