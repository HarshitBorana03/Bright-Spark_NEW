#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> 
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "CII"
#define WIFI_PASSWORD "gits@cii"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBQiu3uRke16KhV7xzfcbMc95spXCpfUv8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://energysavers-es-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
#define FIREBASE_HOST "https://energysavers-es-default-rtdb.asia-southeast1.firebasedatabase.app/"  
#define FIREBASE_AUTH "N7tOZGv33LigpU4TYcNptiK8eHBaB8Q7RXTCnXTD"  
#define WIFI_SSID "SHAKTIMAN"
#define WIFI_PASSWORD "DSHAKTIMAN"
const uint64_t pipeIn = 0xE8E8F0F0E1LL; 
RF24 radio(D4, D2);
  struct MyData {
    float h; 
    float t;
    float Irms ;
    float Irms1 ;
    float Irms2 ;
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
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
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
  delay(1000);
  firebasedata();
}

  void firebasedata()
  {

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an float number on the database path test/int
    if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Bulb/Power(Watt)", 0.01 + data.Irms*230.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Bulb/Voltage(Volt)",  230)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    
    if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Bulb/Current(A)", 0.01+ data.Irms)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Heater/Power(Watt)", 0.01 + data.Irms1*230.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Heater/Voltage(Volt)",  230)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    
    if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Heater/Current(A)", 0.01+ data.Irms1*1.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/fan/Power(Watt)", 0.01 + data.Irms2*230.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/fan/Voltage(Volt)",  230)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    
    if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/fan/Current(A)", 0.01+ data.Irms2*1.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
     if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Temprature(oC)", 0.01+ data.t*1.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
     if (Firebase.RTDB.setFloat(&fbdo, "Kitchen/Humidity(%)", 0.01+ data.h*1.0)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}
  }