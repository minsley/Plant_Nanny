
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
char time[6];

/***************************************
Set up the SD
***************************************/

#include <SD.h>

#define mosi 11
#define miso 12
#define sclk 13
#define sd_cs  4

File diary;
String record;

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
Set up time keeping
***************************************/

#include <Time.h>
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

// T1262347200  //noon Jan 1 2010

time_t t;

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
  TFTscreen.text("Time", 104, 120);
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
  if(Serial.available())
  {
    processSyncMessage();
  }
  if(timeStatus() == timeNotSet)
    Serial.println("waiting for sync message");
  
  t = now();
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
  TFTscreen.text(time, 70, 130);
  
  dtostrf(f,4,2,temp);
  dtostrf(h,4,2,humid);
  String(l).toCharArray(light, 4);
  String(m).toCharArray(moist, 4);
  if (timeStatus()!= timeNotSet) 
  {  
    formatTime().toCharArray(time, 6);
    record = String(t) + "\t"; 
  }
  else 
  { 
    String timeString = "00:00";
    timeString.toCharArray(time, 6);
    record = "0000000000\t"; 
  }
  
  TFTscreen.stroke(255,255,255);
  TFTscreen.text(temp, 0, 10);
  TFTscreen.text(humid, 0, 50);
  TFTscreen.text(light, 0, 90);
  TFTscreen.text(moist, 0, 130);
  TFTscreen.text(time, 70, 130);
  
  record +=
     String(temp)  + "\t" +
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

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
    char c = Serial.read() ;
    Serial.print(c);  
    if( c == TIME_HEADER ) {      
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){  
        c = Serial.read();          
        if( c >= '0' && c <= '9'){  
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }  
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }  
  }
}

String formatTime()
{
  String prettyTime;
  
  if(hour() < 10) prettyTime += "0";
  prettyTime += String(hour()) + ":";
  
  if(minute() < 10) prettyTime += "0";
  prettyTime += String(minute());
  
  return prettyTime;
}
