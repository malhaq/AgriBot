Agribotix – Smart Agricultural Robot

Agribotix is a smart agricultural robot designed to automate essential farming tasks such as navigation, spraying, and environmental monitoring. The robot integrates mechanical, electronic, and software systems to create a reliable solution for modern precision agriculture.

🚜 Project Overview

Agribotix is a 4-wheel autonomous/remote-controlled agricultural robot capable of:

Navigating using GPS coordinates.

Automatic or manual steering.

Spraying water or insecticides based on humidity levels or farmer input.

Communicating wirelessly via ESP32.

Running 4 NEMA23 stepper motors for movement.

Providing a Wi-Fi control interface with real-time commands.

The system combines Arduino Mega, ESP32, GPS module, MPU6050, and various actuators to deliver a flexible and expandable platform for smart farming.

🛠 Hardware Components

Arduino Mega 2560 – main controller for motors, sensors, and spraying mechanisms

ESP32 – Wi-Fi communication, ESP-NOW protocol, and web interface

NEO-6M GPS module – coordinate tracking and navigation

MPU6050 – orientation and balance correction

4 × NEMA23 (3.5A) stepper motors – driving the wheels

Stepper motor drivers – one per motor

Servo motor – controls the sprayer

Humidity sensor – automatic spraying logic

Water/Insecticide sprayer module

4-wheel base with chain-driven front wheels

🧠 Core Features
1. Manual & Automatic Navigation

Control via a mobile/PC web interface (hosted on ESP32)

Autonomous movement to saved GPS coordinates

Direction instructions (left, right, forward, stop) stored and recalled using ESP-NOW communication

2. Smart Spraying System

Sprays automatically based on humidity threshold

Supports manual spraying on command

Controlled with an onboard servo mechanism

3. Wireless Communication

ESP-NOW for fast low-power transmitter/receiver messaging

Wi-Fi AP mode to host a control page

Commands sent to Arduino Mega through UART

4. Sensor Fusion

MPU6050 used for balancing and correcting the orientation

Real-time reading of GPS and IMU for reliable navigation


🧭 Navigation Logic

The receiver stores multiple GPS coordinate pairs as "direction checkpoints" and compares each new coordinate received from the sender to decide the next movement:

Left

Right

Forward

Backward

Stop

🚀 How to Run

Flash the ESP32 code to set up Wi-Fi + ESP-NOW communication

Flash the Arduino Mega code for motor control, MPU6050, and sprayer

Connect wiring as shown in /hardware/wiring_diagram

Power the robot (recommended: dual power supply, motors isolated)

Connect to the ESP32 Wi-Fi network and open the control page

Operate manually or enable automatic GPS navigation
