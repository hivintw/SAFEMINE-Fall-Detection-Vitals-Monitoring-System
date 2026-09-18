#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// 1. WI-FI CREDENTIALS
#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "password"

// 2. FIREBASE CREDENTIALS
#define API_KEY "api"
#define DATABASE_URL "databaseurl"

#define USER_EMAIL "deviceemail"
#define USER_PASSWORD "devicepassword"

// 3. LORA PINS FOR ESP32
#define ss 5      
#define rst -1    
#define dio0 2    

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected to Wi-Fi!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback; 
  
  config.fcs.download_buffer_size = 512;
  config.fcs.upload_buffer_size = 512;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase Connected & Authenticated!");

  Serial.println("Initializing LoRa Receiver...");
  LoRa.setPins(ss, rst, dio0);
  
  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed! Check wiring.");
    delay(1000);
  }
  
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSyncWord(0x12); 
  LoRa.enableCrc();
  
  Serial.println("ESP32 Gateway Ready. Async Mode Active.");
}

void loop() {
  // 1. CHECK FOR RESET COMMAND FROM PC
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); 
    if (command == "RESET") {
      LoRa.beginPacket();
      LoRa.print("RESET");
      LoRa.endPacket();
      LoRa.receive(); 
    }
  }

  // 2. FAST LORA READ
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = LoRa.readString();
    incoming.trim(); 
    
    Serial.print("Received: ");
    Serial.println(incoming);

    if (Firebase.ready()) {
      FirebaseJson json;
      bool pushData = false;

      // Parse Fall & Temp Data
      if (incoming.startsWith("F,")) {
        int firstComma = incoming.indexOf(',');
        int secondComma = incoming.indexOf(',', firstComma + 1);
        
        if (firstComma > 0 && secondComma > 0) {
          String fallStatus = incoming.substring(firstComma + 1, secondComma);
          fallStatus.trim();
          
          String tempStr = incoming.substring(secondComma + 1);
          tempStr.trim();
          
          json.set("fallStatus", fallStatus);
          json.set("temperature", (double)tempStr.toFloat()); 
          pushData = true;
        }
      }
      
      // Parse Heart Rate & SpO2 Data
      else if (incoming.startsWith("H,")) {
        int firstComma = incoming.indexOf(',');
        int secondComma = incoming.indexOf(',', firstComma + 1);
        
        if (firstComma > 0 && secondComma > 0) {
          String hrStr = incoming.substring(firstComma + 1, secondComma);
          hrStr.trim();
          
          String spo2Str = incoming.substring(secondComma + 1);
          spo2Str.trim();
          
          json.set("heartRate", (double)hrStr.toFloat());
          json.set("spo2", spo2Str.toInt());
          pushData = true;
        }
      }

      // 3. ASYNC BACKGROUND CLOUD UPLOAD
      if (pushData) {
        json.set("timestamp/.sv", "timestamp");
        
        if (Firebase.RTDB.updateNodeAsync(&fbdo, "/telemetry", &json)) {
          // Upload happens in background, loop continues instantly!
        } else {
          Serial.print("Cloud Error: ");
          Serial.println(fbdo.errorReason());
        }
      }
    }
  }
}