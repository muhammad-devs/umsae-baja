#include <SPI.h>
#include <SD.h>

boolean serialPrint=false; //need to set to true to run
char *logFile = "datalog.csv"; //max 8 characters
int readPushButtonPin = 9;
unsigned long logIntervals=1000; //milliseconds
unsigned long rememberTime=millis();
boolean headerWritten=false;
File myFile;

void writLog(float rpm);
//void readLog(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial){}
  pinMode(readPushButtonPin, INPUT_PULLUP);
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)){
    Serial.println("Initialization failed!");
    while (1);
  }
  Serial.println("Initialization done.");
  SD.remove(logFile); //removes old file every time you reset
  myFile=SD.open(logFile, FILE_WRITE);

}

void loop() {
  // put your main code here, to run repeatedly:
 int sensor=analogRead(A0); //read A0 pin
 float rpm=sensor/20;
 if (digitalRead(readPushButtonPin) == HIGH){
    writLog(rpm);
  }

 void writLog(float rpm){
  if ((millis()-rememberTime) >= logIntervals){
    myFile=SD.open(logFile, FILE_WRITE);
    if (!headerWritten){
      myFile.printIn("Time (s), RPM");
      headerWritten=true;
    }
    myFile.print(millis()); //record time
    myFile.print(","); //write comma
    myFile.print(rpm); //write rpm
    Serial.println(rpm);
    if (serialPrint){
      Serial.print("Time:");
      Serial.print(millis());
      Serial.print("\tRPM:");
      Serial.print(rpm);

    }
    myFile.close();
    if (serialPrint){
      Serial.printIn("done.")
    }
  rememberTime=millis();
  }
 }
}
/*void readLog(){
  myFile=SD.open(logFile);
  if (myFile){
    Serial.print("Reading...");
    Serial.PrintIn(logFile);

  }
  */
}
