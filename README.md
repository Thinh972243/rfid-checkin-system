# RFID Employee Check-In/Check-Out System

A lightweight employee attendance system based on the ATmega328P. This project utilizes an RFID reader to track staff presence and displays the real-time status on an I2C LCD.

# System Workflow
* **Initial State:** Upon system startup, all RFID cards are initialized to the **OUT** state (absent or not checked in).
* **Check-In:** Scanning a card for the first time registers a check-in. The LCD will display: `Check-IN OK`.
* **Check-Out:** Scanning the same card again registers a check-out. The LCD will display: `Check-OUT OK`.

# Data Management
The system tracks employee attendance using a `user_status` array. This simple data structure makes it highly flexible and easy to integrate into larger external systems.
* `user_status = 0`: **OUT** (Absent or not checked in).
* `user_status = 1`: **IN** (Employee is currently at work).

#  Hardware Requirements
* **MCU:** ATmega328P
* **RFID Module:** MFRC522 RFID Reader
* **Display:** LCD 1602 with PCF8574T I2C adapter

# Development Environment
* **EDA / IDE:** Microchip Studio
* **Toolchain:** AVR 8-bit Toolchain 4.0.0

#  Note
> The system can only scan **MIFARE CLASSIC 1K (4 UID bytes)** cards.