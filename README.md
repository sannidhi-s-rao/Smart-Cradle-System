# Smart Cradle System

An IoT-based Smart Cradle system developed to assist caregivers by continuously monitoring an infant's condition and providing timely notifications. The system detects a baby's cry and wetness using sensors, sends alerts to the caregiver, and enables remote control of the cradle's swinging mechanism.

---

## Project Overview

The Smart Cradle integrates embedded systems and IoT technologies to improve infant care. It monitors the baby's environment using sound and moisture sensors connected to an ESP32 microcontroller. When a crying sound or wet condition is detected, the system notifies the caregiver and allows the cradle to be controlled remotely.

---

## Features

- Detects baby's crying using a sound sensor
- Detects wetness using a moisture sensor
- Sends SMS notifications using Twilio
- Supports remote cradle control using MQTT
- Automatically controls the servo motor for cradle swinging
- Real-time monitoring through ESP32

---

## Hardware Used

- ESP32 Development Board
- Sound Sensor
- Moisture Sensor
- Servo Motor
- Power Supply

---

## Software Used

- Arduino IDE
- MQTT
- Twilio API

---

## System Workflow

1. ESP32 continuously monitors the sound and moisture sensors.
2. If the baby cries, an alert is sent to the caregiver.
3. The caregiver can remotely control the cradle using MQTT.
4. The servo motor swings the cradle when activated.
5. If moisture is detected, an SMS notification is sent to the caregiver.

---

## Repository Structure

```
Smart-Cradle-System
│
├── code/
├── diagrams/
├── images/
├── docs/
├── videos/
└── README.md
```

---

## Results

The developed system successfully:

- Detected baby's crying
- Detected moisture conditions
- Sent SMS notifications using Twilio
- Enabled remote cradle control through MQTT
- Operated the servo motor for automatic cradle swinging

---

## Future Improvements

- Temperature and humidity monitoring
- Mobile application interface
- Live video monitoring
- AI-based cry classification
- Health monitoring sensors

---

## Team

This project was developed as part of the Internet of Things Laboratory course at NMAM Institute of Technology.

---

## License

This repository is intended for educational and portfolio purposes.
