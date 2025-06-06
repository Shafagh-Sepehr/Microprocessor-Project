# 🔐 Dual-ATmega32 Door-Lock System (Micro-Project)

![MCU](https://img.shields.io/badge/MCU-ATmega32-ff69b4)

This repo contains a complete **two-microcontroller** electronic door-lock simulator.  
One AVR handles user I/O (*transmitter*), the other drives the lock (*receiver*)—all modelled in **Proteus** and written in neat C.

---

## 📜 Project Brief
Course spec highlights → our implementation:

| Requirement (spec) | How we met it |
|--------------------|---------------|
| Two MCUs, USART link | `transmiter/` ➜ keypad + LCD; `receiver/` ➜ lock + display confirmation. |
| Non-volatile passwords | EEPROM read/write with checksum validation (`eeprom.h`). |
| Default passcode `0000`, user can add/change codes | Entire password table stored in EEPROM; helper menu on LCD. |
| Alarm after **3** wrong tries | PD5 LED strobes & piezo siren fires. |
| Adjustable alarm loudness via potentiometer | ADC0 samples pot; 8-bit “poor-man’s DAC” on PORTB outputs to buzzer. |
| Display team names & IDs on startup | `show_names()` scrolls credits across 16×2 LCD. |
| Bonus: keypad **interrupt** read & back-space | INT2 on PB2 launches keypad ISR; key `'B'` acts as back-space. |

(Original Persian brief excerpts shown in **micro project_2.pdf** for reference) 

---

## 🛠️ Hardware (Simulated)

* 2 × **ATmega32** @ 1 MHz (internal RC)  
* 1 × 4×3 matrix keypad (12 keys)  
* 1 × 16×2 HD44780 LCD  
* 1 × Piezo buzzer + 10 kΩ potentiometer (volume)  
* 1 × LED on PD5 (alarm)  
* 1 × LED on PC3 (door-bolt indicator)  
* PORTB[7:2] → simple R-2R ladder “DAC” for siren tones  

All parts are stock Proteus items—no custom models needed.

