# Cloud Connected Environmental Data Logger with Excel Reporting

## Overview

This project is an IoT-based Environmental Data Logger developed using the **LPC2148 ARM7 Microcontroller**. It continuously monitors environmental parameters such as **temperature** and **gas leakage**, displays the readings on an LCD, stores configuration data in EEPROM, and uploads sensor data to the **ThingSpeak Cloud** through the ESP-01 Wi-Fi module. The collected data can be viewed online and exported to **Microsoft Excel** for analysis and reporting.

---

## Features

* Real-time temperature monitoring using **LM35** sensor.
* Gas leakage detection using **MQ-2** sensor.
* 16x2 LCD display for live sensor readings and system status.
* ESP-01 Wi-Fi module for cloud connectivity.
* Uploads sensor data to **ThingSpeak**.
* Buzzer alert when gas is detected or temperature exceeds the set limit.
* EEPROM (AT24C256) stores the temperature set point.
* Cloud data can be exported to Excel for reports and analysis.

## 📊 Project Block Diagram

![Project Block Diagram](Screenshot%202026-09-12%20070011.png)

## Hardware Components Used

* LPC2148 ARM7 Microcontroller
* ESP-01 Wi-Fi Module
* LM35 Temperature Sensor
* MQ-2 Gas Sensor
* AT24C256 EEPROM (I2C)
* 16x2 LCD Display
* Buzzer/LED
* Power Supply (3.3V / 5V)
* Keypad(4x4)

## Software Used

* Keil µVision
* Flash Magic
* Embedded C Programming
* ThingSpeak Cloud Platform

## Working Principle

1. LPC2148 initializes LCD, UART, I2C, ADC, and ESP-01.
2. LM35 measures the current temperature.
3. MQ-2 checks for gas leakage.
4. Sensor values are displayed on the LCD.
5. If the temperature exceeds the set point or gas is detected:

   * Buzzer turns ON.
   * Alert data is sent to ThingSpeak.
6. The temperature set point is stored in EEPROM.
7. Sensor data is uploaded to ThingSpeak and can be exported to Excel.

---

## Project Flow

* Initialize LCD
* Initialize UART
* Initialize I2C
* Initialize ADC
* Connect ESP-01 to Wi-Fi
* Read LM35 Temperature
* Read MQ-2 Gas Sensor
* Display Values on LCD
* Compare Temperature with Set Point
* If Temperature High or Gas Detected:

  * Turn ON Buzzer
  * Send Data to ThingSpeak
* Store Set Point in EEPROM
* Repeat Monitoring

---

## Project Structure

```text
Final_Majorp/
├── main.c
├── uart.c
├── uart.h
├── lcd.c
├── lcd.h
├── adc.c
├── adc.h
├── esp01.c
├── esp01.h
├── i2c.c
├── i2c.h
├── eeprom.c
├── eeprom.h
├── delay.c
├── delay.h
└── README.md
```

---

## ThingSpeak Integration

The ESP-01 communicates with ThingSpeak using AT commands over UART.

Example data uploaded:

* Field 1 – Temperature
* Field 2 – Gas Sensor Status

---

## Applications

* Environmental Monitoring
* Smart Home Safety
* Gas Leakage Detection
* Industrial Monitoring
* IoT Data Logging
* Cloud-Based Sensor Monitoring

---

## Future Improvements

* Add humidity sensor (DHT11/DHT22).
* Mobile app notifications.
* Email/SMS alerts.
* Multiple sensor support.
* SD card data backup.

---

## Author

**Harikrishna**

Embedded Systems Project using LPC2148 ARM7 Microcontroller.
