#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;    
EnergyMonitor emon2;    
EnergyMonitor emon3;    
      // Create an instance
int CT_pin = 0; //  CT sensor pin connected to A5 pin of Arduino
int CT_pin1 = 1; //  CT sensor pin connected to A5 pin of Arduino
int CT_pin2 = 2; //  CT sensor pin connected to A5 pin of Arduino
const uint64_t pipeOut = 0xE8E8F0F0E1LL; 
#define DHTPIN 2
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
RF24 radio(7, 8); //  CN and CSN  pins of nrf
struct MyData {
  byte h;
  byte t;
  double Irms ;
    double Irms1 ;
    double Irms2 ;
};
MyData data;
void setup()
{
  Serial.begin(9600); 
  dht.begin();
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  emon1.current(CT_pin, 29.8);             // Current: input pin, calibration.
  emon2.current(CT_pin1, 24); 
  emon3.current(CT_pin2, 26.5); 
}
void loop()
{
  // double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  //   double Irms1 = emon2.calcIrms(1480);  // Calculate Irms only
  //   double Irms2 = emon2.calcIrms(1480);  // Calculate Irms only
data.Irms=emon1.calcIrms(1480);  // Calculate Irms only
data.Irms1=emon2.calcIrms(1480);  // Calculate Irms only
data.Irms2=emon3.calcIrms(1480);  // Calculate Irms only
  Serial.print("BulbPower:=");     
  Serial.print(data.Irms*230.0);           // Apparent power
  Serial.print("   ");
  Serial.print(data.Irms); 
  Serial.println("  ");
  Serial.print("heater Power:=");               // Irms
  Serial.print(data.Irms1*230.0);           // Apparent power
  Serial.print(" ");
  Serial.print(data.Irms1); 
  Serial.println("  ");              // Irms
  Serial.print("tubelight:=");               // Irms
  Serial.print(data.Irms2*230.0);           // Apparent power
  Serial.print(" ");
  Serial.print(data.Irms2);
  Serial.println("  ");
  data.h = dht.readHumidity();
  data.t = dht.readTemperature();
  if (isnan(data.h) || isnan(data.t)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(data.h);
  Serial.print("Temperature: ");
  Serial.print(data.t);
    Serial.println("  ");
  radio.write(&data, sizeof(MyData));
}