/*
Plant_Nanny

Watches your plants to make sure they have adequate 
soil moisture, light exposure, temperature and humidity.

*/

/***************************************
Set up the LCD
***************************************/

#include <SPI.h>
#include <TFT.h>

#define cs  10
#define dc  9
#define rst 8

TFT TFTscreen = TFT(cs,dc,rst);

char temp[6];
char humid[6];
char light[4];
char moist[4];

/***************************************
Set up the SD
***************************************/

#include <SD.h>

#define mosi 11
#define sclk 13
#define sd_cs  4

File diary;

/***************************************
Set up the HT sensor
***************************************/

#include <DHT.h>
#include <stdlib.h>

#define DHTPIN 2
#define DHTTYPE DHT11 

int cycleTime = 5000;

DHT dht(DHTPIN, DHTTYPE);

float h;
float f;

/***************************************
Set up the light sensor
***************************************/

#define LSPIN 0

int l;

/***************************************
Set up the moisture sensor
***************************************/

#define MSDATAPIN 1
#define MSPWRPIN 3

int m;
int timeSinceMRead;

#define MSReadTime 100

// num cycles to skip moisture readings (slows sensor oxidation)
#define MSWaitCycles 12

/***************************************
Main Setup()
***************************************/

void setup()
{
  Serial.begin(9600);
  
  TFTscreen.begin();
  TFTscreen.setRotation(0);
  TFTscreen.background(0,0,0);
  TFTscreen.stroke(255,255,255);
  TFTscreen.setTextSize(1);
  TFTscreen.text("Air Temp (F)",0,0);
  TFTscreen.text("Humidity (%)",0,40);
  TFTscreen.text("Luminosity",0,80);
  TFTscreen.text("Moisture",0,120);
  TFTscreen.setTextSize(2);
  Serial.println("TFT Screen initialized.");
  
  //pinMode(10, OUTPUT);
  SD.begin(sd_cs);
  Serial.println("SD Card initialized.");
  
  dht.begin();
  Serial.println("DHT11 initialized.");
  
  pinMode(MSPWRPIN, OUTPUT);
  timeSinceMRead = 0;
  Serial.println("Hygrometer initialized.");
}

/***************************************
Main Loop()
***************************************/

void loop()
{
  f = dht.readTemperature(true);
  h = dht.readHumidity();
  l = analogRead(LSPIN);
  
  if(timeSinceMRead <= 0)
  {
    digitalWrite(MSPWRPIN, HIGH);
    delay(MSReadTime);  // sensor needs some time to power up
    m = analogRead(MSDATAPIN);
    digitalWrite(MSPWRPIN, LOW);
    timeSinceMRead = MSWaitCycles;
  } else { timeSinceMRead--; }
  
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(temp, 0, 10);
  TFTscreen.text(humid, 0, 50);
  TFTscreen.text(light, 0, 90);
  TFTscreen.text(moist, 0, 130);
  
  dtostrf(f,4,2,temp);
  dtostrf(h,4,2,humid);
  String(l).toCharArray(light, 4);
  String(m).toCharArray(moist, 4);
  
  TFTscreen.stroke(255,255,255);
  TFTscreen.text(temp, 0, 10);
  TFTscreen.text(humid, 0, 50);
  TFTscreen.text(light, 0, 90);
  TFTscreen.text(moist, 0, 130);
  
  String record = String(temp)  + "\t" +
             String(humid) + "\t" +
             String(light) + "\t" +
             String(moist);
  
  Serial.println(record);
  
  writeToLog(record);
  
  delay(cycleTime);
}

void writeToLog(String text)
{
  diary = SD.open("log.txt", FILE_WRITE);
  if(diary)
  {
    diary.println(text);
  }
  diary.close();
}
