# Test Automation & Watchdog Demo

This project demonstrates software/hardware integration testing on an Arduino Uno (ATmega328P). It showcases how to implement and test a **Watchdog Timer (WDT)** to recover from system freezes, alongside other hardware drivers like GPIO, Timers, ADC, and EEPROM.

## Key Features

*   **Watchdog Timer Protection:** A safety mechanism that automatically resets the system if the main loop stops running (e.g., due to a software bug or infinite loop).
*   **LED Toggle (Blink):** Uses a timer to blink an LED. The state (blinking vs. off) is saved to EEPROM, so it remembers its setting even after a power loss or reset.
*   **Temperature Monitoring:** Reads a temperature sensor (TMP36) and prints the value to the serial console.
*   **Machine Learning (Linear Regression):** Includes a simple fixed-point linear regression model that "trains" at startup to map sensor voltage to temperature.

## How to Test the Watchdog

A deliberate "bug" has been introduced in `library/source/logic/logic.cpp` to demonstrate the Watchdog in action.

1.  **Upload the Code:** Run `pio run -t upload`.
2.  **Open Serial Monitor:** Run `pio device monitor`.
3.  **Trigger the Bug:** Press and **hold** either the Toggle Button (Pin 4) or Temperature Button (Pin 7).
4.  **Observe:**
    *   The serial monitor will print: `ENTERING INFINITE LOOP - WATCHDOG SHOULD RESET SOON...`
    *   The system will freeze for approximately **1 second**.
    *   The system will automatically **reboot** (you will see the startup messages again).

## Hardware Setup

*   **LED:** Pin 5
*   **Toggle Button:** Pin 4
*   **Temperature Button:** Pin 7
*   **Temperature Sensor (TMP36):** Analog Pin A2

## Project Structure

*   `library/source/logic/logic.cpp`: Contains the main application logic and the "bug" for testing.
*   `library/source/driver/watchdog/`: Contains the driver implementation for the ATmega328P watchdog timer.
*   `platformio.ini`: PlatformIO configuration file.
