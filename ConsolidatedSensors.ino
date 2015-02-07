/***********************************************
ConsolidatedSensors.ino

Logs data from temperature, light, sound, and
vibration sensors.

Author: Kevin Greer    December 2014
************************************************/

#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include "Adafruit_TCS34725.h"  //Light sensor
#include "DHT.h"                //Temp sensor

#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN 4     // what pin temp sensor is connected to

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial xBee(2, 3); // RX, TX

const int soundPin = A3;
const int sampleWindow = 2000; // Sample window width in mS for sound sensor (50 mS = 20Hz)
unsigned int sample;

const int chipSelect = 10;
File dataFile;

void setup() 
{   
    Serial.begin(9600);
    xBee.begin(9600);
    
    //Set up SD card
    while (!Serial) {
      ;
    }
    Serial.print("Initializing SD card...");
    pinMode(SS, OUTPUT);
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      while (1);
    }
    Serial.println("card initialzed.");
    dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (!dataFile){
      Serial.println("error opening datalog.txt");
      while (1);
    }
    
    //Set up light sensor
    Serial.print("Initializing light sensor...");
    if (tcs.begin()) {
    Serial.println("Light sensor found");
    } else {
      Serial.println("Light sensor not found ... check your connections");
      while (1);
    }
    
    //Indicate new measurement on datalog
    dataFile.println("<<<<<<<<<<<<NEW MEASUREMENT: ConsolidatedSensors>>>>>>>>>>>>>");
}

void loop(void) {
  String dataString = "";
  
  
  
  //Light sensor data***********
  uint16_t r, g, b, c, colorTemp, lux;
  
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  
  dataString += "Color Temp: "; dataString+= colorTemp; dataString += " K";
  dataString += "\tLight Intensity: "; dataString += lux; dataString += " lm";
  
  
  
  
  
  //Temp sensor data**********************
  float h = dht.readHumidity();
  
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);
  
  dataString += "\t\tHumidity: "; dataString += h; dataString += "%";
  dataString += "\tTemperature: "; dataString += f; dataString += " *F";
  dataString += "\tHeat Index: "; dataString += hi; dataString += " *F";
  
  
  
  
  
  //Sound sensor data*************************
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int vibrationMax = 0;
 
  // collect data for sampleWindow mS
  while (millis() - startMillis < sampleWindow)
  {
     sample = analogRead(soundPin);
     
     if (xBee.available() > 0) {
     // read the incoming byte:
        byte incomingByte = xBee.read();
        if (incomingByte > vibrationMax){
          vibrationMax = incomingByte;
        }
     }
     if (sample < 1024)  // toss out spurious readings
     {
       if (sample > signalMax)
       {
         signalMax = sample;  // save just the max levels
       }
       else if (sample < signalMin)
       {
         signalMin = sample;  // save just the min levels
       }
     }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
  
  dataString += "\tSound Signal: "; dataString += peakToPeak;
  
  //correct spacing
  if (peakToPeak < 10){
    dataString += "\t";
  }
  
   
  //Vibration sensor data*************************
  dataString += "\tVibration Sensor: "; dataString += vibrationMax;
   
  dataFile.println(dataString);
  Serial.println(dataString);
  dataFile.flush();
}
