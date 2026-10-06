
/*
 ============================================================
 SMART HELMET - MAIN ESP32 CAMERA WARNING RECEIVER
 ============================================================
 
 Board:
 ESP32 Dev Module
 
 ESP32-CAM sends:
 "WARNING"
 
 Main ESP32 receives it using ESP-NOW.
 
 Camera warning buzzer:
 GPIO25
 
 Pattern:
 BEEP
 short pause
 BEEP
*/
 
#include <WiFi.h>
#include <esp_now.h>
 
 
// ============================================================
// BUZZER
// ============================================================
 
#define BUZZER_PIN 25
 
 
// ============================================================
// ESP-NOW MESSAGE
// ============================================================
 
typedef struct struct_message {
 
 char text[32];
 
} struct_message;
 
 
struct_message incomingMessage;
 
 
// ============================================================
// CAMERA WARNING BEEP
// ============================================================
 
void cameraBeep() {
 
 // First short beep
 digitalWrite(
   BUZZER_PIN,
   HIGH
 );
 
 delay(150);
 
 
 digitalWrite(
   BUZZER_PIN,
   LOW
 );
 
 delay(120);
 
 
 // Second short beep
 digitalWrite(
   BUZZER_PIN,
   HIGH
 );
 
 delay(150);
 
 
 digitalWrite(
   BUZZER_PIN,
   LOW
 );
}
 
 
// ============================================================
// ESP-NOW RECEIVE CALLBACK
// ============================================================
 
void OnDataRecv(
 
 const esp_now_recv_info_t *info,
 
 const uint8_t *incomingData,
 
 int len
 
) {
 
 
 memcpy(
 
   &incomingMessage,
 
   incomingData,
 
   sizeof(incomingMessage)
 
 );
 
 
 Serial.print(
   "CAM MESSAGE RECEIVED: "
 );
 
 
 Serial.println(
   incomingMessage.text
 );
 
 
 // ==========================================================
 // CHECK WARNING MESSAGE
 // ==========================================================
 
 if (
 
   strcmp(
 
     incomingMessage.text,
 
     "WARNING"
 
   )
 
   == 0
 
 ) {
 
 
   Serial.println(
     "CAMERA WARNING -> BEEP"
   );
 
 
   cameraBeep();
 
 }
}
 
 
// ============================================================
// SETUP
// ============================================================
 
void setup() {
 
 Serial.begin(115200);
 
 delay(1000);
 
 
 // ==========================================================
 // BUZZER INITIALIZATION
 // ==========================================================
 
 pinMode(
   BUZZER_PIN,
   OUTPUT
 );
 
 
 digitalWrite(
   BUZZER_PIN,
   LOW
 );
 
 
 // ==========================================================
 // WIFI STATION MODE
 // ==========================================================
 
 WiFi.mode(
   WIFI_STA
 );
 
 
 delay(1000);
 
 
 Serial.print(
   "Main ESP32 MAC: "
 );
 
 
 Serial.println(
   WiFi.macAddress()
 );
 
 
 // ==========================================================
 // ESP-NOW INITIALIZATION
 // ==========================================================
 
 if (
 
   esp_now_init()
 
   != ESP_OK
 
 ) {
 
 
   Serial.println(
     "ESP-NOW initialization FAILED"
   );
 
 
   return;
 }
 
 
 // ==========================================================
 // REGISTER RECEIVER
 // ==========================================================
 
 esp_now_register_recv_cb(
   OnDataRecv
 );
 
 
 Serial.println(
   "ESP-NOW initialized successfully"
 );
 
 
 Serial.println(
   "MAIN ESP32 READY"
 );
}
 
 
// ============================================================
// LOOP
// ============================================================
 
void loop() {
 
 // Nothing required here.
 // ESP-NOW receive callback handles camera warnings.
 
}
