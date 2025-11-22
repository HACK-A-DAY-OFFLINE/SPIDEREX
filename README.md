# SPIDEREX
# Vana-Rakshak: LoRa-Based Tree Monitoring System

Vana-Rakshak is a low-power IoT system for real-time tree vibration and tilt monitoring using **ESP32 + LoRa (RFM95)**. The transmitter encodes sensor events using a **DNA-based 2-bit encoding scheme** and transmits them over LoRa. The receiver decodes the data, updates visual indicators, and pushes alerts via a **Telegram Bot**.

## Features
- LoRa-based long-range event transmission  
- MPU6050 vibration + tilt detection  
- DNA-encoded message compression  
- OLED status display (SSD1306)  
- Buzzer + LED alert system  
- WiFi-enabled Telegram notifications  
- Remote reset command support  

## Hardware
- ESP32 (Tx/Rx)  
- RFM95 LoRa Module  
- MPU6050 IMU  
- SSD1306 OLED  
- Buzzer + LEDs  

## Operation
1. **Transmitter** detects events → encodes → sends via LoRa.  
2. **Receiver** decodes → triggers alerts → displays status.  
3. Alerts are pushed to Telegram through WiFi.  
4. `"2"` command resets receiver state.

