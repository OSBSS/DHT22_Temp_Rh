// OSBSS Temp/Rh using DHT22 - v0.3.2

#include "DHT.h"
#include <SD.h>
#include <Wire.h>
#include <Time.h>
#include <DS3234.h>

DHT sensor;

int ID = 1;
int h = 19, m = 00, s = 0; // set the start time in 24-hr clock
float temp, Rh;

void setup()
{
  delay(500);
  RTC.configure(5,6,7,9);
  pinMode(10, OUTPUT);
  dht.setup(2);
  SD.begin(8);
  PrintHeader();
  CheckTime();
}

void PrintHeader() // prints header to SD card
{
   File datafile = SD.open("berg.csv", FILE_WRITE);
   if (datafile)
   {
      String header = "ID, Date, Time, Temperature (C), Humidity (%)";
      datafile.println(header);
      datafile.close();
   }
}

void CheckTime() // checks if the specified start time has reached
{
   while(1)
   {
    RTC.readDateTime();
    if(RTC.time_h()==h && RTC.time_m()==m && RTC.time_s()==s)
    {
      break;
    }
   }
}

void loop()
{
    RTC.readDateTime();
    
    if(s>=60)
    {
      s = s-60;  //reset s (seconds) to 0 after one minute or more has passed
      m++;  // add one minute after 60 seconds have passed
    }
    if(m>=60)
    {
      m = m-60;  //reset m (minutes) to 0 after one hour or more has passed
    }
    if(h>=24)
    {
      h = h-24;  //if time interval is greater than one hour
    }
    if(RTC.time_h()==h+1 || RTC.time_h()==0)
    {
      h = RTC.time_h();  //change value of h to actual hour after one hour or one day
    }
    
    if(RTC.time_h()==h && RTC.time_m()==m && RTC.time_s()==s)
    {
      GetData();
      //ConvertToF();
      PrintToSD();
      //s = s + 10; // for 10 seconds intervals
      m++; // for one min intervals
      //m = m+5; //for 5 min intervals
      //h = h++;
      ID++;
    }
}

void GetData() // this function will get the sensor data with an Onset HOBO U12
{
  temp = dht.getTemperature();
  Rh = dht.getHumidity();
  //temp = 1.0073 * temp; // sensor calibration with HOBO U12
  //Rh = 0.8218 * Rh;
  //temp = 1.0078 * temp; // sensor calibration with HOBO U12
  //Rh = 0.9356 * Rh;
}

void ConvertToF() // this function will convert temp values from degree Celsius to degree Fahrenheit
{
  float temp_f = (1.8 * temp) + 32;
  temp = temp_f;
}

void PrintToSD()  // this function saves the data to the SD card
{   
        File datafile = SD.open("berg.csv", FILE_WRITE);
        if(datafile)
        {
          datafile.print(ID);
          datafile.print(",");
          datafile.print(RTC.date_m());
          datafile.print("/");
          datafile.print(RTC.date_d());
          datafile.print("/");
          datafile.print(RTC.date_y());
          datafile.print(",");
          datafile.print(h);
          datafile.print(":");
          datafile.print(m);
          datafile.print(":");
          datafile.print(s);
          datafile.print(",");
          datafile.print(temp, 3); // printing temperature up to 3 decimal places
          datafile.print(",");
          datafile.print(Rh, 3); // printing Rh up to 3 decimal places
          datafile.println();
          datafile.close();
        }
        else
          Serial.println("SD card error.");
}
