#include <Arduino.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xA0, 0xDD, 0x6C, 0x03, 0xF1, 0xC4};
//a0:dd:6c:03:f1:c4
int i = 0;

float offsetX = 0.4;
float offsetY = 0.2;
float offsetZ = 0.2;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
float xrotate,xtranslate;
float yrotate,ytranslate;
float zrotate,ztranslate;
unsigned long clock;
float temp;
} struct_message;

Adafruit_MPU6050 mpu;
// Create a struct_message called myData
struct_message myData[128];
unsigned long tempo = micros();
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   if (!mpu.begin()) {
  Serial.println("Sensor init failed");
  while (1)
    yield();
}
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
   // Set values to send

   /*
   strcpy(myData.a, "THIS IS A CHAR");
   myData.b = random(1,20);
   myData.c = 1.2;
   myData.d = false;
   */
  	   
    	// Get new data from the sensor

      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      // Print out the data
      
      myData[i].clock = micros() - tempo;
      myData[i].xrotate = g.gyro.x;
      myData[i].xtranslate = a.acceleration.x - offsetX;
      myData[i].yrotate = g.gyro.y;
      myData[i].ytranslate = a.acceleration.y - offsetY;
      myData[i].zrotate = g.gyro.z;
      myData[i].ztranslate = a.acceleration.z - offsetZ;
   
   
   
   // Send message via ESP-NOW
    if (i == 127) {
      Serial.println("Data too large");
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      if (result == ESP_OK) {
          Serial.println("Sent with success");
      }
      else {
          Serial.println("Error sending the data");
      }
      return;
    }
  i = i++;
  if (i == 128) {
    Serial.println(myData[0].clock - myData[127].clock + " Tempo decorridos");
    i = 0;
  }
}


