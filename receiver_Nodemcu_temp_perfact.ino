#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> 

const uint64_t pipeIn = 0xE8E8F0F0E1LL; 
RF24 radio(D4, D2);
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
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();
  //lcd.println("Receiver ");
}
void recvData()
{
  if ( radio.available() ) {
    radio.read(&data, sizeof(MyData));
    }
}
void loop()
{
  recvData();
  Serial.print("Humidity: ");
  Serial.print(data.h);
  Serial.println("");
  Serial.print("Temperature: ");
  Serial.print(data.t);
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
  
 
}