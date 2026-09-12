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
* Buzzer/LED alert when gas is detected or temperature exceeds the set limit.
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

## ⚙️ Working of the Project

### Step 1: System Initialization

When power is supplied to the LPC2148 microcontroller, it initializes all the required peripherals and hardware modules.

* LCD is initialized to display messages and sensor readings.
* UART0 is initialized for communication with the ESP-01 Wi-Fi module.
* I2C interface is initialized for communication with the RTC and AT24C256 EEPROM.
* ADC channels are initialized to read analog values from the LM35 and MQ-2 sensors.
* ESP-01 Wi-Fi module is configured using AT commands and connected to the Wi-Fi network.

After successful initialization, the LCD displays that the system is ready for monitoring.

### Step 2: Temperature Measurement Using LM35

The LM35 temperature sensor continuously senses the surrounding temperature and produces an analog voltage proportional to the temperature.

* The sensor output is connected to the ADC input of LPC2148.
* The ADC converts the analog voltage into a digital value.
* The LPC2148 converts this value into temperature in degrees Celsius (°C).

The measured temperature is updated continuously.

### Step 3: Gas Leakage Detection Using MQ-2

The MQ-2 gas sensor detects the presence of combustible gases such as LPG, methane, propane, and smoke.

* The sensor output is connected to another ADC channel.
* LPC2148 continuously reads the gas sensor value.
* The gas value is compared with a predefined threshold to detect gas leakage.

If the gas concentration exceeds the threshold, the system identifies it as a gas leak.

### Step 4: Real-Time Clock (RTC) Operation

The DS1307 RTC module maintains the current date and time.

* RTC communicates with LPC2148 through the I2C protocol.
* Every sensor reading is associated with the current time and date.
* This provides accurate timestamp information for cloud logging and Excel reports.

### Step 5: LCD Display

The 16×2 LCD displays real-time system information.

The LCD shows:

* Current temperature value.
* Gas sensor status (Safe / Gas Detected).
* Date and time from RTC.
* Wi-Fi connection status.
* Cloud upload status.

This allows the user to monitor the system without a computer.

### Step 6: Temperature Set Point and EEPROM Storage

A temperature limit (set point) is stored permanently in the AT24C256 EEPROM.

* LPC2148 reads the stored set point during startup.
* The user can change the set point using the keypad or switch.
* Whenever the set point is changed, LPC2148 writes the new value into EEPROM through I2C.
* The stored value remains available even after power is turned OFF.

### Step 7: Threshold Comparison and Alarm Generation

LPC2148 continuously compares the measured temperature and gas sensor value with the predefined limits.

**Condition 1:** Temperature greater than set point.

**Condition 2:** Gas sensor detects gas leakage.

If either condition becomes TRUE:

* Buzzer turns ON immediately.
* Warning message is displayed on the LCD.
* Alert data is prepared for cloud transmission.

If both values are normal:

* Buzzer remains OFF.
* Monitoring continues continuously.

### Step 8: ESP-01 Wi-Fi Communication

The ESP-01 module provides internet connectivity.

LPC2148 communicates with ESP-01 through UART using AT commands.

The communication sequence includes:


AT       ->   Checks whether the ESP-01 module is responding.

AT+CWMODE=1  ->  Sets ESP-01 to Station Mode.

AT+CWJAP="SSID","PASSWORD" ->  Connects ESP-01 to the Wi-Fi network.

AT+CIPSTART="TCP","api.thingspeak.com",80 -> Establishes a TCP connection with the ThingSpeak server.

AT+CIPSEND=Length   ->  Prepares ESP-01 to send HTTP data.

GET /update?api_key=YOUR_API_KEY&field1=temperature&field2=gas&field3=tempsetpoint Uploads temp,gas,tempsetpoint data to ThingSpeak.

AT+CIPCLOSE     ->	Closes the TCP connection.

### Step 9: Uploading Data to ThingSpeak Cloud

After establishing the internet connection:

* Temperature value is uploaded to **Field 1**.
* Gas sensor value is uploaded to **Field 2**.
* Data is stored in the ThingSpeak cloud with date and time.

The cloud dashboard displays sensor readings in graphical form for continuous monitoring.

### Step 10: Excel Report Generation

ThingSpeak stores all uploaded sensor readings in the cloud database.

* The logged data can be exported as a CSV or Excel file.
* The Excel report contains temperature, gas sensor values, date, and time.
* This report can be used for analysis, monitoring history, and project documentation.

### Step 11: Continuous Monitoring Loop

The complete system works continuously in a loop.

1. Read temperature from LM35.
2. Read gas value from MQ-2.
3. Read current time from RTC.
4. Display values on LCD.
5. Compare values with the stored set point.
6. Activate buzzer if temperature or gas exceeds the limit.
7. Upload sensor data to ThingSpeak through ESP-01.
8. Store updated set point in EEPROM whenever modified.
9. Repeat the monitoring process continuously.

## ThingSpeak Cloud Output

![ThingSpeak Output]Screenshot%202026-09-12%20092549.png)


### Project Output

The system provides the following outputs:

* Real-time temperature monitoring.
* Gas leakage detection and alert generation.
* Date and time display using RTC.
* Permanent temperature set point storage in EEPROM.
* Wi-Fi-based cloud monitoring using ESP-01.
* ThingSpeak graphical visualization.
* Excel/CSV report generation for recorded environmental data.


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
* Field 3 - Temperature set point 
---
## ThingSpeak Cloud Output

![ThingSpeak Cloud Output](Screenshot%202026-09-12%20092549.png))

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
