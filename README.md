# 🎥 Project Demo
https://github.com/user-attachments/assets/ef4c2e71-47cd-492e-9889-8ebece63fd46

# 💳 RFID Employee Check-In/Check-Out System

A lightweight employee attendance system based on the ATmega328P. This project utilizes an RFID reader to track staff presence and displays the real-time status on an I2C LCD.

## 🔄 System Workflow
* 🏁 **Initial State:** Upon system startup, all RFID cards are initialized to the **OUT** state (absent or not checked in).
* 📥 **Check-In:** Scanning a card for the first time registers a check-in. The LCD will display: `Check-IN NV(user_index)`.
* 📤 **Check-Out:** Scanning the same card again registers a check-out. The LCD will display: `Check-OUT NV(user_index)`.

## 🔑 Understanding `user_index`

The `user_index` (0 to 19) acts as a lightweight employee ID, directly mapped to the physical RFID cards via EEPROM memory blocks. 

* 🔍 **Implicit Mapping:** When a card is scanned, the system searches the EEPROM for the matching 4-byte UID. The loop counter (`i`), which represents the EEPROM block position of that UID, is returned directly as the `user_index`.
* ⚡ **Resource Efficiency:** This decoupled approach avoids storing complex UID arrays in the strictly limited SRAM. It also allows replacing a lost card simply by overwriting its 4-byte block in EEPROM, leaving the core `user_status` tracking logic completely untouched.

## 📊 Data Management
The system tracks employee attendance using a `user_status` array. This simple data structure makes it highly flexible and easy to integrate into larger external systems.
* 🔴 `user_status = 0`: **OUT** (Absent or not checked in).
* 🟢 `user_status = 1`: **IN** (Employee is currently at work).

## 🛠️ Hardware Requirements
* 🎛️ **MCU:** ATmega328P
* 📡 **RFID Module:** MFRC522 RFID Reader
* 📺 **Display:** LCD 1602 with PCF8574T I2C adapter

## 💻 Development Environment
* ⚙️ **EDA / IDE:** Microchip Studio
* 🧰 **Toolchain:** AVR 8-bit Toolchain 4.0.0

## 📌 Note
> ⚠️ The system can only scan **MIFARE CLASSIC 1K (4 UID bytes)** cards.
