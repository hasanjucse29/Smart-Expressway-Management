# Smart Expressway Management System

![Project Status](https://img.shields.io/badge/Status-Completed-green)
![Last Updated](https://img.shields.io/badge/Last%20Updated-May%2027,%202025%20-%2004:57%20PM%20%2B06-blue)

Welcome to the **Smart Expressway Management System**, an IoT-based project completed on May 27, 2025, at 04:57 PM +06. This system monitors vehicle speed, identifies vehicles using RFID, and issues overspeed alerts in real-time, with remote control via the Blynk IoT platform. Designed as a prototype, it demonstrates smart traffic management solutions using an ESP32 microcontroller.

## Overview

The Smart Expressway Management System aims to enhance road safety by providing real-time monitoring and control. It uses two IR sensors to calculate vehicle speed, an RFID reader positioned between the sensors to identify vehicles, and a buzzer for overspeed alerts, all managed through a user-friendly Blynk app interface.

## Features

- **Real-Time Speed Detection**: Measures vehicle speed using two IR sensors spaced 10 cm apart.
- **Vehicle Identification**: Reads RFID tags as vehicles pass between the IR sensors, displaying dummy vehicle info (e.g., model, color, plate).
- **Overspeed Alerts**: Automatically activates a buzzer for speeds exceeding 60 cm/s, with manual control via Blynk.
- **Remote Monitoring**: Displays speed and vehicle info on the Blynk app, with buzzer toggle functionality.
- **IoT Integration**: Connects to the Blynk platform via WiFi for remote access.

## Hardware Requirements

- **ESP32 Microcontroller**: Core processing unit with WiFi capability (e.g., ESP-WROOM-32).
- **IR Sensors (2)**: For detecting vehicle movement (connected to pins 33 and 32).
- **MFRC522 RFID Reader**: For vehicle identification (connected via SPI: SS=5, RST=21, SCK=14, MOSI=12, MISO=13).
- **Buzzer**: For overspeed alerts (connected to pin 25).
- **LED (Optional)**: Indicates RFID detection (connected to pin 2).
- **Jumper Wires and Breadboard**: For circuit assembly.
- **Power Supply**: 3.3V or 5V compatible with ESP32.

## Software Dependencies

- **Arduino IDE**: For programming the ESP32.
- **Libraries**:
  - `WiFi.h`: For ESP32 WiFi connectivity.
  - `BlynkSimpleEsp32.h`: For Blynk integration.
  - `SPI.h`: For RFID communication.
  - `MFRC522.h`: For controlling the RFID reader.
- **Blynk IoT Platform**: Requires a Blynk account, template ID, and auth token (see `BLYNK_*` defines in code).
- **WiFi Network**: SSID and password for connectivity (e.g., "A35", "subarna1234").

## Installation

1. **Set Up Hardware**:
   - Connect the ESP32, IR sensors, RFID reader, buzzer, and LED as per the circuit diagram (available in the repository or project documentation).
   - Ensure the RFID reader is placed between the two IR sensors along a 10 cm path.

2. **Install Software**:
   - Install the Arduino IDE (https://www.arduino.cc/en/software).
   - Add the ESP32 board support via the Boards Manager.
   - Install required libraries via the Arduino Library Manager or manually:
     - Blynk library (https://github.com/blynkkk/blynk-library).
     - MFRC522 library (https://github.com/miguelbalboa/rfid).

3. **Configure Blynk**:
   - Sign up at https://blynk.io and create a new template.
   - Note the Template ID, Template Name, and Auth Token.
   - Set up datastreams:
     - V0 (Double): Speed in cm/s.
     - V1 (Integer): Buzzer control (0=OFF, 1=ON).
     - V2 (String): Vehicle info.
   - Configure the Blynk app with Labeled Value (V0), Button (V1), and Text/Labeled Value (V2) widgets.

4. **Upload Code**:
   - Open `Smart_Expressway_Management.ino` (included in this repository).
   - Update the `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`, `ssid`, and `pass` with your values.
   - Select the ESP32 board and port in Arduino IDE.
   - Upload the code to the ESP32.

## Usage

1. **Power On the System**:
   - Connect the ESP32 to a power source and ensure WiFi connectivity.

2. **Test the System**:
   - Pass an object (e.g., a toy car with an RFID tag) from IR Sensor 1 to IR Sensor 2.
   - Verify the sequence in the Serial Monitor (115200 baud):
     - "First IR detected"
     - "UID: ..." (RFID read)
     - "Second IR detected"
     - "Speed: X cm/s"
     - "Vehicle: ..." (with speed)
   - Check the Blynk app:
     - V0 shows the speed (e.g., 66.67 cm/s).
     - V2 shows vehicle info (e.g., "Model: Toyota Prius...").
     - Toggle V1 to control the buzzer manually.

3. **Overspeed Alert**:
   - If speed >60 cm/s, the buzzer activates automatically.
   - Use the Blynk app to toggle the buzzer when speed ≤60 cm/s.

## Code Overview

The main file, `Smart_Expressway_Management.ino`, includes:
- **Setup**: Initializes serial, SPI, pins, and Blynk.
- **Loop**: Manages IR sensor detection, RFID reading (between sensors), speed calculation, and Blynk updates.
- **Blynk Handler**: Processes V1 button inputs for manual buzzer control.

Key snippets:
- Speed calculation:
  ```cpp
  float timeDifference = (secondDetectionTime - firstDetectionTime) / 1000.0;
  speed = distanceBetweenSensors / timeDifference;
  Blynk.virtualWrite(V0, speed);
