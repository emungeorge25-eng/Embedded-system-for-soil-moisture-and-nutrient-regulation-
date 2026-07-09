# SmartSoil: IoT-Based Soil Moisture and Nutrient Monitoring System

## Project Title
Embedded system for Soil Moisture and Nutrient Regulation System for Peri-Urban Smallholder Tomato Farmers

## Problem & Solution

**Problem:**
Peri-urban smallholder tomato farmers often struggle with inconsistent and inefficient irrigation and nutrient application. Manual monitoring of soil moisture and nutrient levels is time-consuming, imprecise, and often leads to over-watering, under-watering, or poor nutrient dosing — all of which reduce crop yield and waste resources.

**Solution:**
SmartSoil is an ESP32-based IoT system that automatically monitors soil moisture in real time and controls a water pump and nutrient pump accordingly. Readings are displayed locally on a TFT screen and pushed to Firebase, allowing farmers to view soil status remotely and manually override irrigation or nutrient dosing through a companion mobile app (built with MIT App Inventor). This reduces guesswork, saves water, and improves nutrient delivery efficiency for smallholder farms.

## System Overview

- **Microcontroller:** ESP32
- **Soil Sensor:** Analog soil moisture sensor (powered on-demand via a dedicated GPIO to reduce probe corrosion)
- **Display:** ST7735 TFT display (SPI) showing real-time moisture % and soil N/P/K readings
- **Actuators:** Water pump and nutrient pump, each switched via digital output pins
- **Connectivity:** WiFi + Firebase Realtime Database for remote monitoring and control
- **Mobile App:** MIT App Inventor app for remote pump control (irrigation and nutrient dosing)

## Setup Instructions

### 1. Hardware Requirements
- ESP32 development board
- Analog soil moisture sensor
- ST7735 TFT display (SPI)
- 2-channel relay module (or transistor driver circuit) for water pump and nutrient pump
- Water pump and nutrient/peristaltic pump
- Jumper wires, breadboard/PCB, 5V power supply

### 2. Wiring (as per firmware pin definitions)
| Component        | ESP32 Pin |
|-------------------|-----------|
| TFT CS            | GPIO 27   |
| TFT DC            | GPIO 2    |
| TFT RST           | GPIO 4    |
| Soil sensor power | GPIO 22   |
| Soil sensor signal | GPIO 35 (ADC1_CH7) |
| Water pump        | GPIO 21   |
| Nutrient pump      | GPIO 14   |

### 3. Software Requirements
- Arduino IDE (latest version)
- ESP32 board package installed via Arduino Boards Manager
- Required libraries (install via Library Manager):
  - `Adafruit_GFX`
  - `Adafruit_ST7735`
  - `SPI`
  - Firebase library (as referenced by `firebase.h` in the firmware)

### 4. Firebase Setup
1. Create a Firebase Realtime Database project.
2. Set up a database path (e.g. `iot_soil_monitoring/`) with child keys for `irrigation`, `nutrient`, `moistureValue`, `nitrogen`, `phosphorus`, `potassium`.
3. Add your Firebase project credentials (API key, database URL) into `firebase.h`.
4. Ensure your ESP32 WiFi credentials are set in the `wifi_setUp()` function.

### 5. Uploading the Firmware
1. Open `smartsoil.ino` in Arduino IDE.
2. Select **Board:** ESP32 Dev Module.
3. Select the correct COM port.
4. Click **Upload**.
5. Open Serial Monitor at 115200 baud to view live readings.

### 6. Mobile App
1. Open the SmartSoil MIT App Inventor app on an Android device.
2. Ensure it is linked to the same Firebase project/database.
3. Use the app to send `"1"` to trigger irrigation or `"3"` to trigger nutrient dosing (these numeric flags are used instead of text strings for compatibility with App Inventor).

## Repository Structure
```
SmartSoil-IoT/
├── README.md
├── firmware/
│   └── smartsoil.ino
├── schematics/
│   └── circuit_diagram.png
└── docs/
    └── report_excerpt.pdf (optional)
```

## Notes
- Soil moisture thresholds (`soilWet`, `soilDry`) are calibrated analog values and may need adjustment depending on the specific sensor and soil type used.
- N, P, K values displayed are currently simulated for demonstration purposes; replace with actual sensor readings if/when an NPK sensor is integrated.

## Author
Adino George Emun — Final Year Diploma in Electrical and Electronics Engineering, Uganda Institute of Information and Communications Technology (UICT)
