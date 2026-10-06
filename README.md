# 🪖 Smart Helmet

### An IoT-Based Accident Detection and Emergency Alert System

The **Smart Helmet** is an IoT-based safety system designed to improve motorcycle rider safety by automatically detecting possible accidents and providing emergency alerts.

The system uses an **ESP32** as the main controller and integrates an **MPU6050 accelerometer/gyroscope**, **SW-420 vibration sensor**, **ESP32-CAM**, **buzzer**, **LED**, and wireless communication technologies.

When an abnormal motion or impact is detected, the system performs accident validation and provides a warning to the rider. After a confirmed accident, the system can communicate with a paired smartphone through **Bluetooth Low Energy (BLE)**. The smartphone can obtain the rider's GPS location and send an emergency notification through **SMS or Telegram**.

---

## 🚀 Features

- 🚨 Automatic accident detection
- 📐 MPU6050-based motion, acceleration and tilt monitoring
- 📳 SW-420 vibration and impact detection
- 🔄 Dual-sensor validation to reduce false alarms
- 🔊 Buzzer-based accident warning
- 💡 LED-based visual warning
- ⏱️ Confirmation period before emergency notification
- ❌ Manual false-alarm cancellation
- 📱 BLE communication with smartphone
- 📍 GPS-based location sharing through smartphone
- 📩 Emergency notification through SMS/Telegram
- 📷 ESP32-CAM visual monitoring
- 📸 Accident-related image capture
- 🔋 Rechargeable battery-powered design
- 💰 Low-cost embedded and IoT implementation

---

## 🧠 System Overview

The Smart Helmet consists of several interconnected modules.

```text
                ┌─────────────────────┐
                │      MPU6050        │
                │ Motion / Acceleration│
                └──────────┬──────────┘
                           │
                           │
                ┌──────────▼──────────┐
                │      SW-420         │
                │ Vibration / Impact  │
                └──────────┬──────────┘
                           │
                           ▼
                  ┌─────────────────┐
                  │      ESP32      │
                  │ Main Controller │
                  └───────┬─────────┘
                          │
              ┌───────────┼────────────┐
              │           │            │
              ▼           ▼            ▼
           Buzzer        LED       Cancel Button
                          │
                          ▼
                       BLE
                          │
                          ▼
                 ┌─────────────────┐
                 │    Smartphone   │
                 │  GPS + Alert    │
                 └───────┬─────────┘
                         │
                    ┌────┴────┐
                    ▼         ▼
                   SMS     Telegram


             ESP32-CAM
                 │
                 ▼
          Visual Monitoring
          / Image Capture
```

The overall system flow follows:

```text
Sensors
   ↓
ESP32
   ↓
Accident Detection
   ↓
Confirmation Period
   ↓
Buzzer + LED Warning
   ↓
Cancel Button?
   ├── YES → Cancel Alert
   │
   └── NO
        ↓
   Accident Confirmed
        ↓
       BLE
        ↓
    Smartphone
        ↓
    GPS Location
        ↓
   SMS / Telegram
        +
   ESP32-CAM Capture
```

---

## 🔧 Hardware Components

| Component | Purpose |
|---|---|
| ESP32 | Main controller and sensor processing |
| MPU6050 | Acceleration, gyroscope, tilt and motion detection |
| SW-420 | Vibration and mechanical shock detection |
| ESP32-CAM | Visual monitoring and image capture |
| ESP32-CAM-MB | USB programming and serial interface |
| Piezo Buzzer | Audible warning |
| LED | Visual warning/status |
| Push Button | Manual false-alarm cancellation |
| 3.7V Li-ion Battery | Portable power source |
| TP4056 | Battery charging and protection |
| MT3608 | Voltage boosting/regulation |
| Smartphone | GPS and emergency communication |

---

## ⚙️ Accident Detection

The system uses two sensors for accident detection:

### MPU6050

The MPU6050 provides:

- 3-axis acceleration
- 3-axis gyroscope data
- Motion detection
- Tilt/orientation monitoring
- Sudden acceleration detection

### SW-420

The SW-420 detects:

- Sudden vibration
- Mechanical shock
- Impact

The proposed accident detection logic combines both sensors to reduce false triggers.

```text
Accident Candidate =
(High Acceleration) AND (High Vibration)
```

The approximate acceleration threshold proposed in the project is:

```text
3G ≈ 29.4 m/s²
```

The exact threshold can be calibrated depending on the helmet, riding condition and sensor placement.

---

## 🚨 Emergency Alert System

When a possible accident is detected:

1. The ESP32 detects abnormal sensor readings.
2. The buzzer and LED are activated.
3. A **10-second confirmation period** starts.
4. The rider can press the cancel button.
5. If the rider cancels the alert, the system returns to normal monitoring.
6. If the rider does not cancel, the accident is confirmed.
7. The ESP32 sends an accident trigger to the paired smartphone using BLE.
8. The smartphone obtains the current GPS location.
9. Emergency information can be sent through SMS or Telegram.

Example BLE trigger:

```text
ACCIDENT_DETECTED
```

---

## 📷 ESP32-CAM

The ESP32-CAM provides an additional visual monitoring feature.

After an accident is confirmed, the camera subsystem can be triggered to capture visual information surrounding the incident.

Possible information includes:

- Road condition
- Nearby objects
- Surrounding environment
- Visual evidence of the incident

The **ESP32-CAM-MB** is used mainly for convenient USB programming and serial communication during development.

---

## 📡 Communication

The project uses multiple communication methods.

### BLE

```text
ESP32 → BLE → Smartphone
```

The smartphone can receive the accident trigger and start the emergency response process.

### Smartphone GPS

```text
Smartphone
     ↓
GPS Coordinates
     ↓
Google Maps Location
```

### Emergency Notification

```text
GPS Location
     ↓
SMS / Telegram
     ↓
Predefined Emergency Contacts
```

---

## 💻 Software Architecture

The software is divided into several modules:

1. Sensor initialization
2. Motion and vibration monitoring
3. Accident candidate detection
4. Confirmation timer
5. Manual cancellation
6. BLE communication
7. Smartphone automation
8. Camera triggering and image capture

---

## 📂 Project Structure

```text
Smart-Helmet/
│
├── README.md
│
├── src/
│   ├── ESP32_CAM_Object_Detection.ino
│   ├── Main_ESP32_Camera_Warning_Receiver.ino
│   └── Smart_Helmet_Sensor_Test.ino
│
└── .gitignore
```

### ESP32-CAM Object Detection

`ESP32_CAM_Object_Detection.ino`

This code:

- Initializes the ESP32-CAM
- Captures RGB565 frames
- Uses QQVGA resolution
- Monitors a central region of interest
- Detects frame-to-frame movement
- Sends a `WARNING` message through ESP-NOW

### Main ESP32 Camera Warning Receiver

`Main_ESP32_Camera_Warning_Receiver.ino`

This code:

- Initializes ESP-NOW
- Receives the camera warning
- Checks for the `WARNING` message
- Activates the buzzer

### Sensor Test

`Smart_Helmet_Sensor_Test.ino`

This code tests:

- SW-420 vibration sensor
- MPU6050 acceleration values

---

## 📌 ESP32-CAM Detection Parameters

The current camera prototype uses:

```cpp
#define SAMPLE_STEP 4
#define PIXEL_DIFF_THRESHOLD 25
#define MOVEMENT_THRESHOLD 12.0
#define CONFIRM_FRAMES 3
```

The camera uses:

```text
Resolution: 160 × 120
Pixel Format: RGB565
```

The central Region of Interest is:

```text
X: 40 → 120
Y: 30 → 90
```

If movement exceeds the configured threshold for three consecutive frames, a warning is transmitted to the Main ESP32.

---

## 🔌 Important Pin Connections

### MPU6050

| MPU6050 | ESP32 |
|---|---|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| VCC | 3.3V |
| GND | GND |

I2C address:

```text
0x68
```

### SW-420

```text
SW-420 Signal → GPIO 27
```

### Buzzer

```text
Buzzer → GPIO 25
```

---

## 🔋 Power Management

The prototype uses a rechargeable **3.7V Li-ion battery**.

Simplified power flow:

```text
3.7V Li-ion Battery
        ↓
      TP4056
        ↓
 Voltage Regulation
        ↓
ESP32 + Sensors + Actuators
```

An MT3608 boost converter can be used where a higher regulated voltage is required.

---

## 🧪 Testing

The system should be tested under controlled conditions before real-world use.

### Sensor Testing

- Sudden acceleration
- Helmet tilt
- Normal movement
- Controlled impact
- Low vibration
- Strong vibration
- False-trigger conditions

### Emergency System Testing

- Normal riding
- Minor movement
- Simulated accident with cancellation
- Simulated accident without cancellation
- BLE communication
- GPS acquisition
- SMS/Telegram notification
- ESP32-CAM image capture

### Camera Testing

- Camera initialization
- Image capture
- Response time
- Daylight image quality
- Low-light performance
- Trigger reliability
- Storage/transmission reliability

---

## ✅ Advantages

- Automatic accident detection
- Reduced dependence on manual emergency calls
- Dual-sensor validation
- Manual false-alarm cancellation
- BLE smartphone connectivity
- GPS-based emergency location sharing
- SMS and Telegram notification capability
- ESP32-CAM visual evidence
- Compact and relatively low-cost design
- Practical implementation of embedded and IoT concepts

---

## ⚠️ Limitations

- Sensor thresholds require calibration.
- Smartphone availability is required for the proposed GPS and long-distance alert system.
- BLE communication depends on the helmet-smartphone connection.
- Camera operation can increase power consumption.
- Low-light conditions may reduce image quality.
- The prototype is not a certified medical or emergency-response device.
- Extensive controlled testing is required before real-world deployment.

---

## 🔮 Future Improvements

Possible future improvements include:

- Dedicated GPS module
- Cellular communication directly on the helmet
- Machine-learning-based accident detection
- Cloud-based incident storage
- Automatic camera image upload
- Larger emergency contact management system
- Improved battery optimization
- Waterproof/weather-resistant enclosure
- Improved camera placement
- Dedicated mobile application

---

## 🎯 Project Goal

The main goal of this project is to demonstrate how **microcontrollers, sensors, wireless communication, mobile automation and camera technology** can be combined to create a practical rider-safety system.

The project was developed as part of the **CSE 4326 – Microprocessors and Microcontrollers Laboratory** course.

---

## 👥 Team

**Team 7 — Section M**  
Department of Computer Science and Engineering  
United International University  
Summer 2026

---

## 📚 References

1. ESP32 Technical Documentation — Espressif Systems
2. ESP32-CAM Module Documentation
3. MPU6050 6-Axis Motion Tracking Device Datasheet
4. SW-420 Vibration Sensor Module Documentation
5. TP4056 Lithium Battery Charger Module Documentation
6. CSE 4326 Microprocessors and Microcontrollers Laboratory Project Proposal

---

## ⚠️ Disclaimer

This project is an **educational prototype** developed for academic and experimental purposes. It should not be considered a certified safety, medical, or emergency-response device without further testing, validation and certification.
