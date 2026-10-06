#include <Wire.h>
 
#define SW420_PIN 27
 
#define MPU_ADDR 0x68
 
void setup() {
 
 Serial.begin(115200);
 
 Wire.begin(21, 22);
 
 pinMode(SW420_PIN, INPUT);
 
 // Wake up MPU6050
 Wire.beginTransmission(MPU_ADDR);
 Wire.write(0x6B);
 Wire.write(0x00);
 Wire.endTransmission();
 
 Serial.println("Smart Helmet Sensor Test Started");
}
 
 
void loop() {
 
 // Read SW420 vibration sensor
 int vibration = digitalRead(SW420_PIN);
 
 
 // Read MPU6050 acceleration
 int16_t ax = 0;
 int16_t ay = 0;
 int16_t az = 0;
 
 
 Wire.beginTransmission(MPU_ADDR);
 Wire.write(0x3B);
 Wire.endTransmission(false);
 
 Wire.requestFrom(MPU_ADDR, 6);
 
 
 if (Wire.available() == 6) {
 
   ax = Wire.read() << 8 | Wire.read();
   ay = Wire.read() << 8 | Wire.read();
   az = Wire.read() << 8 | Wire.read();
 
 }
 
 
 Serial.print("SW420 Vibration: ");
 Serial.print(vibration);
 
 Serial.print(" | MPU6050 AX: ");
 Serial.print(ax);
 
 Serial.print(" AY: ");
 Serial.print(ay);
 
 Serial.print(" AZ: ");
 Serial.println(az);
 
 
 // Basic accident detection test
 if (vibration == 0) {
 
   Serial.println("Vibration detected!");
 
 }
 
 
 delay(200);
}
