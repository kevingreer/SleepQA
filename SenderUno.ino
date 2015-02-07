/***************************************
SenderUno.ino

Gathers data from a vibration sensor and
sends it to a receiver Arduino
 
Author: Kevin Greer  December 2014
***************************************/

#include <SoftwareSerial.h>

SoftwareSerial xBee(3, 2); // RX, TX
int sensorPin = A2;    
int sensorValue = 0;

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // set the data rate for the SoftwareSerial port
  xBee.begin(9600);

}

void loop() // run over and over
{ 
  
  sensorValue = analogRead(sensorPin); 
 
  xBee.write(sensorValue);
  Serial.print("Serial: "); Serial.println(sensorValue);
  if (xBee.available()){
   Serial.println(xBee.read());
  }
  delay(100);
}
