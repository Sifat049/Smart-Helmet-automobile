/*
 ============================================================
 SMART HELMET - ESP32-CAM OBJECT / MOVEMENT WARNING
 ============================================================
 
 Board:
 AI Thinker ESP32-CAM
 
 Features:
 - RGB565 camera capture
 - QQVGA 160x120 resolution
 - Central Region of Interest (ROI)
 - Frame difference detection
 - 12% movement threshold
 - 3 consecutive frames required
 - ESP-NOW wireless WARNING transmission
 
 Receiver:
 Main ESP32 DevKit
 
 IMPORTANT:
 Replace receiverMAC if using another main ESP32.
*/
 
#include "esp_camera.h"
#include <WiFi.h>
#include <esp_now.h>
 
 
// ============================================================
// MAIN ESP32 MAC ADDRESS
// ============================================================
 
// Your tested Main ESP32 MAC:
// 00:70:07:19:FB:20
 
uint8_t receiverMAC[] = {
 0x00, 0x70, 0x07, 0x19, 0xFB, 0x20
};
 
 
// ============================================================
// AI THINKER ESP32-CAM PINS
// ============================================================
 
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
 
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
 
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
 
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
 
 
// ============================================================
// CAMERA DETECTION SETTINGS
// ============================================================
 
// Check every 4th pixel
#define SAMPLE_STEP 4
 
// Required brightness difference
#define PIXEL_DIFF_THRESHOLD 25
 
// Required percentage of changed pixels
#define MOVEMENT_THRESHOLD 12.0
 
// Number of consecutive high-change frames required
#define CONFIRM_FRAMES 3
 
 
// ============================================================
// CENTRAL DANGER ZONE
// ============================================================
 
// Camera resolution:
// 160 x 120
 
#define ROI_X_MIN 40
#define ROI_X_MAX 120
 
#define ROI_Y_MIN 30
#define ROI_Y_MAX 90
 
// Approximately 300 samples,
// extra space provided for safety
#define MAX_SAMPLES 400
 
 
// ============================================================
// VARIABLES
// ============================================================
 
uint8_t previousFrame[MAX_SAMPLES];
 
bool firstFrame = true;
 
int dangerCount = 0;
 
 
// Prevent sending continuous warnings too quickly
unsigned long lastWarningTime = 0;
 
const unsigned long warningCooldown = 1500;
 
 
// ============================================================
// ESP-NOW MESSAGE STRUCTURE
// ============================================================
 
typedef struct struct_message {
 
 char text[32];
 
} struct_message;
 
struct_message message;
 
 
// ============================================================
// ESP-NOW SEND CALLBACK
// ============================================================
 
// Callback signature for the newer ESP32 Arduino core
void OnDataSent(
 const wifi_tx_info_t *info,
 esp_now_send_status_t status
) {
 
 Serial.print("ESP-NOW STATUS: ");
 
 if (status == ESP_NOW_SEND_SUCCESS) {
 
   Serial.println("SUCCESS");
 
 }
 
 else {
 
   Serial.println("FAILED");
 
 }
}
 
 
// ============================================================
// RGB565 TO BRIGHTNESS
// ============================================================
 
uint8_t rgb565ToBrightness(uint16_t pixel) {
 
 uint8_t r = (pixel >> 11) & 0x1F;
 uint8_t g = (pixel >> 5)  & 0x3F;
 uint8_t b = pixel & 0x1F;
 
 
 // Convert approximately to 8-bit
 r = r << 3;
 g = g << 2;
 b = b << 3;
 
 
 // Approximate grayscale brightness
 return (r * 30 + g * 59 + b * 11) / 100;
}
 
 
// ============================================================
// SEND WARNING TO MAIN ESP32
// ============================================================
 
void sendWarning() {
 
 strcpy(message.text, "WARNING");
 
 
 esp_err_t result = esp_now_send(
 
   receiverMAC,
 
   (uint8_t *)&message,
 
   sizeof(message)
 
 );
 
 
 if (result == ESP_OK) {
 
   Serial.println("WARNING SENT");
 
 }
 
 else {
 
   Serial.println("WARNING SEND ERROR");
 
 }
}
 
 
// ============================================================
// SETUP
// ============================================================
 
void setup() {
 
 Serial.begin(115200);
 
 delay(2000);
 
 
 Serial.println();
 
 Serial.println(
   "========================================"
 );
 
 Serial.println(
   "SMART HELMET ESP32-CAM"
 );
 
 Serial.println(
   "OBJECT / MOVEMENT WARNING SYSTEM"
 );
 
 Serial.println(
   "========================================"
 );
 
 
 // ==========================================================
 // WIFI / ESP-NOW INITIALIZATION
 // ==========================================================
 
 WiFi.mode(WIFI_STA);
 
 delay(1000);
 
 
 Serial.print(
   "ESP32-CAM MAC: "
 );
 
 Serial.println(
   WiFi.macAddress()
 );
 
 
 if (esp_now_init() != ESP_OK) {
 
   Serial.println(
     "ESP-NOW INITIALIZATION FAILED"
   );
 
   return;
 }
 
 
 esp_now_register_send_cb(
   OnDataSent
 );
 
 
 // ==========================================================
 // ADD MAIN ESP32 AS ESP-NOW PEER
 // ==========================================================
 
 esp_now_peer_info_t peerInfo = {};
 
 
 memcpy(
 
   peerInfo.peer_addr,
 
   receiverMAC,
 
   6
 
 );
 
 
 peerInfo.channel = 0;
 
 peerInfo.encrypt = false;
 
 
 if (
 
   esp_now_add_peer(
     &peerInfo
   )
 
   != ESP_OK
 
 ) {
 
   Serial.println(
     "FAILED TO ADD ESP-NOW PEER"
   );
 
   return;
 }
 
 
 Serial.println(
   "ESP-NOW READY"
 );
 
 
 // ==========================================================
 // CAMERA CONFIGURATION
 // ==========================================================
 
 camera_config_t config;
 
 
 config.ledc_channel =
   LEDC_CHANNEL_0;
 
 config.ledc_timer =
   LEDC_TIMER_0;
 
 
 config.pin_d0 =
   Y2_GPIO_NUM;
 
 config.pin_d1 =
   Y3_GPIO_NUM;
 
 config.pin_d2 =
   Y4_GPIO_NUM;
 
 config.pin_d3 =
   Y5_GPIO_NUM;
 
 config.pin_d4 =
   Y6_GPIO_NUM;
 
 config.pin_d5 =
   Y7_GPIO_NUM;
 
 config.pin_d6 =
   Y8_GPIO_NUM;
 
 config.pin_d7 =
   Y9_GPIO_NUM;
 
 
 config.pin_xclk =
   XCLK_GPIO_NUM;
 
 config.pin_pclk =
   PCLK_GPIO_NUM;
 
 config.pin_vsync =
   VSYNC_GPIO_NUM;
 
 config.pin_href =
   HREF_GPIO_NUM;
 
 
 config.pin_sccb_sda =
   SIOD_GPIO_NUM;
 
 config.pin_sccb_scl =
   SIOC_GPIO_NUM;
 
 
 config.pin_pwdn =
   PWDN_GPIO_NUM;
 
 config.pin_reset =
   RESET_GPIO_NUM;
 
 
 config.xclk_freq_hz =
   20000000;
 
 
 /*
   Your camera sensor produced:
 
   "JPEG format is not supported on this sensor"
 
   Therefore RGB565 is used.
 */
 
 config.pixel_format =
   PIXFORMAT_RGB565;
 
 
 /*
   Low resolution is used to reduce
   RAM and processing requirements.
 */
 
 config.frame_size =
   FRAMESIZE_QQVGA;       // 160 x 120
 
 
 config.jpeg_quality = 12;
 
 config.fb_count = 1;
 
 
 // ==========================================================
 // INITIALIZE CAMERA
 // ==========================================================
 
 esp_err_t err =
   esp_camera_init(
     &config
   );
 
 
 if (err != ESP_OK) {
 
   Serial.printf(
 
     "Camera init FAILED. Error: 0x%x\n",
 
     err
 
   );
 
   return;
 }
 
 
 Serial.println(
   "Camera initialized successfully."
 );
 
 
 Serial.print(
   "Movement threshold: "
 );
 
 Serial.print(
   MOVEMENT_THRESHOLD
 );
 
 Serial.println("%");
 
 
 Serial.print(
   "Confirmation frames: "
 );
 
 Serial.println(
   CONFIRM_FRAMES
 );
 
 
 Serial.println(
   "SYSTEM READY"
 );
 
 Serial.println();
}
 
 
// ============================================================
// MAIN LOOP
// ============================================================
 
void loop() {
 
 // ==========================================================
 // CAPTURE FRAME
 // ==========================================================
 
 camera_fb_t *fb =
   esp_camera_fb_get();
 
 
 if (!fb) {
 
   Serial.println(
     "Camera capture FAILED"
   );
 
   delay(500);
 
   return;
 }
 
 
 int width =
   fb->width;
 
 
 uint8_t *data =
   fb->buf;
 
 
 int sampleIndex = 0;
 
 int changedPixels = 0;
 
 
 // ==========================================================
 // CENTRAL ROI FRAME COMPARISON
 // ==========================================================
 
 for (
 
   int y = ROI_Y_MIN;
 
   y < ROI_Y_MAX;
 
   y += SAMPLE_STEP
 
 ) {
 
 
   for (
 
     int x = ROI_X_MIN;
 
     x < ROI_X_MAX;
 
     x += SAMPLE_STEP
 
   ) {
 
 
     // RGB565 uses 2 bytes/pixel
     int pixelIndex =
 
       (y * width + x) * 2;
 
 
     if (
 
       pixelIndex + 1 >=
       fb->len
 
     ) {
 
       continue;
     }
 
 
     uint16_t pixel =
 
       ((uint16_t)data[pixelIndex] << 8)
 
       |
 
       data[pixelIndex + 1];
 
 
     uint8_t brightness =
 
       rgb565ToBrightness(
         pixel
       );
 
 
     // ======================================================
     // COMPARE CURRENT FRAME WITH PREVIOUS FRAME
     // ======================================================
 
     if (
 
       !firstFrame
 
       &&
 
       sampleIndex <
       MAX_SAMPLES
 
     ) {
 
 
       int difference =
 
         abs(
 
           (int)brightness
 
           -
 
           (int)previousFrame[
             sampleIndex
           ]
 
         );
 
 
       if (
 
         difference >
         PIXEL_DIFF_THRESHOLD
 
       ) {
 
         changedPixels++;
 
       }
     }
 
 
     // Save current brightness
     if (
 
       sampleIndex <
       MAX_SAMPLES
 
     ) {
 
       previousFrame[
         sampleIndex
       ] = brightness;
 
     }
 
 
     sampleIndex++;
   }
 }
 
 
 // Return camera framebuffer
 esp_camera_fb_return(
   fb
 );
 
 
 // ==========================================================
 // FIRST FRAME
 // ==========================================================
 
 if (firstFrame) {
 
   firstFrame = false;
 
 
   Serial.println(
     "First ROI frame stored."
   );
 
 
   Serial.println(
     "Detection starting..."
   );
 
 
   Serial.println();
 
 
   delay(300);
 
   return;
 }
 
 
 // ==========================================================
 // CALCULATE CHANGE PERCENTAGE
 // ==========================================================
 
 float changePercent = 0;
 
 
 if (sampleIndex > 0) {
 
   changePercent =
 
     (
 
       (float)changedPixels
 
       /
 
       (float)sampleIndex
 
     )
 
     * 100.0;
 }
 
 
 // ==========================================================
 // SERIAL OUTPUT
 // ==========================================================
 
 Serial.print(
   "CENTER CHANGE = "
 );
 
 
 Serial.print(
 
   changePercent,
 
   1
 
 );
 
 
 Serial.print(
   "%  "
 );
 
 
 // ==========================================================
 // MOVEMENT DETECTION
 // ==========================================================
 
 if (
 
   changePercent >
   MOVEMENT_THRESHOLD
 
 ) {
 
 
   dangerCount++;
 
 
   Serial.print(
     "POSSIBLE MOVEMENT"
   );
 
 
   // ========================================================
   // CONFIRM AFTER MULTIPLE FRAMES
   // ========================================================
 
   if (
 
     dangerCount >=
     CONFIRM_FRAMES
 
   ) {
 
 
     Serial.print(
       "  --> MOVEMENT CONFIRMED"
     );
 
 
     // ======================================================
     // WARNING COOLDOWN
     // ======================================================
 
     if (
 
       millis() -
       lastWarningTime
 
       >
 
       warningCooldown
 
     ) {
 
 
       Serial.println();
 
 
       Serial.println(
         ">>> SENDING WARNING TO MAIN ESP32 <<<"
       );
 
 
       sendWarning();
 
 
       lastWarningTime =
         millis();
 
     }
   }
 }
 
 else {
 
   dangerCount = 0;
 
 
   Serial.print(
     "NORMAL"
   );
 
 }
 
 
 Serial.println();
 
 
 delay(250);
}
