#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define ss 16   
#define rst -1  
#define dio0 15 
#define BUTTON_PIN 0  

Adafruit_MPU6050 mpu;
PulseOximeter pox; 

const float OFFSET_X = -1.25; 
const float OFFSET_Y = -0.12; 
const float OFFSET_Z = -2.75; 
const float FREEFALL_THRESHOLD = 2.5;  
const float IMPACT_THRESHOLD = 20.0;

unsigned long lastFallTxTime = 0;
unsigned long lastHeartTxTime = 1000; 
unsigned long lastMpuReadTime = 0;
unsigned long freefallTime = 0;
unsigned long lastButtonTime = 0;

char currentFallStatus = 'n'; 
float currentTemp = 0.0; 
bool freefallDetected = false;

void setup() {
  Serial.begin(115200);
  delay(1000); 

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Wire.setClockStretchLimit(200000); 
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  pox.begin();
  
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) { delay(500); }
  
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSyncWord(0x12); 
  LoRa.enableCrc(); 

  Serial.println("Transmitter Ready. Lag-Free I2C Timers Active.");
}

void loop() {
  yield(); 
  
  pox.update(); 

  unsigned long currentMillis = millis();

  // 2. MANUAL RESET BUTTON
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (currentMillis - lastButtonTime > 500) { 
      currentFallStatus = 'n';
      Serial.println("Alert Cleared.");
      lastButtonTime = currentMillis;
    }
  }

  // 3. FALL & TEMP SENSOR 
  if (currentMillis - lastMpuReadTime >= 50) {
    lastMpuReadTime = currentMillis;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    currentTemp = temp.temperature; 

    float calX = a.acceleration.x - OFFSET_X;
    float calY = a.acceleration.y - OFFSET_Y;
    float calZ = a.acceleration.z - OFFSET_Z;
    float accelMag = sqrt(pow(calX, 2) + pow(calY, 2) + pow(calZ, 2));

    if (accelMag < FREEFALL_THRESHOLD) {
      freefallDetected = true;
      freefallTime = currentMillis; 
    }

    if (freefallDetected) {
      if (accelMag > IMPACT_THRESHOLD) {
        currentFallStatus = 'f'; 
        freefallDetected = false; 
      }
      else if (currentMillis - freefallTime > 1500) {
        freefallDetected = false; 
      }
    }
  }

  // 4. TRANSMIT FALL & TEMP 
  if (currentMillis - lastFallTxTime >= 2000) {
    lastFallTxTime = currentMillis;
    
    String fallPayload = "F," + String(currentFallStatus) + "," + String(currentTemp, 1);
    
    LoRa.beginPacket();
    LoRa.print(fallPayload);
    LoRa.endPacket(true); // Non-blocking async transmission

    Serial.print("Sent: ");
    Serial.println(fallPayload);
  }

  // 5. TRANSMIT VITALS (Every 6 Seconds)
  if (currentMillis - lastHeartTxTime >= 6000) {
    lastHeartTxTime = currentMillis;

    float hr = pox.getHeartRate();
    int spo2 = pox.getSpO2();

    // STRICT MEDICAL FILTER: Kills ambient light noise
    if (spo2 < 90 || hr < 45.0) { 
      hr = 0.0;
      spo2 = 0;
    }

    String vitalsPayload = "H," + String(hr, 1) + "," + String(spo2);
    
    LoRa.beginPacket();
    LoRa.print(vitalsPayload);
    LoRa.endPacket(true); // Non-blocking async transmission

    Serial.print("Sent: ");
    Serial.println(vitalsPayload);
  }
}