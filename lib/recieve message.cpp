/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/


#include <esp_wifi.h>

#include <Arduino.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
float xrotate,xtranslate;
float yrotate,ytranslate;
float zrotate,ztranslate;
int clock;
float temp;
} struct_message;

// Create a struct_message called myData
struct_message myData[128];
int i = 0;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  for(int i = 0; i < 128; i++){
  Serial.print(i);
  Serial.print("rotate x y z: ");
  Serial.print(myData[i].xrotate);
  Serial.print(myData[i].yrotate);
  Serial.println(myData[i].zrotate);
  Serial.print("acel x y z: ");
  Serial.println(myData[i].xtranslate);
  Serial.println(myData[i].ytranslate);
  Serial.println(myData[i].ztranslate);
  }

}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("initializing ESP-NOW");
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {

}
