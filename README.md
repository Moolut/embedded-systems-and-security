# Embedded Systems and Security – TUM

This repository contains the three graded programming assignments from the *Embedded Systems and Security* course at the Technical University of Munich. Each assignment focuses on a different aspect of embedded system design and security using the Infineon XMC4500 platform.

---

## 📦 Assignment 1 – Morse Time

In this task, a microcontroller is programmed to transmit a predefined string ("I CAN MORSE") using LED blinking in Morse code according to ITU standard M.1677-1.

- **Part A:** Continuously transmit the message via LED1 using correct timing for dots, dashes, and spaces.
- **Part B:** Enhance interactivity using two buttons:
  - Pressing Button1 sends the message once.
  - Pressing Button2 sends the time between the last two Button1 presses (in milliseconds) via Morse code.
  - Conflict resolution and precise timing management were implemented using SysTick or CCU4 timers.

---

## 💥 Assignment 2 – Smashing the Stack for Fun and Profit

This multi-part assignment dives into classic memory exploitation and secure software design.

- **Part A:** Perform a stack-based buffer overflow attack to inject and execute custom code that turns on both LEDs on the board.
- **Part B:** Demonstrate that compiler-level protections (stack canaries, MPU) are insufficient by bypassing them and triggering a blinking LED.
- **Part C:** Replace the insecure crypto with the `libsodium` library and use authenticated encryption (`crypto_secretbox_easy`) to securely send commands to a real-world system — the TUM Marble Run.

---

## ⌨️ Assignment 3 – HIDe Your Password?

This assignment involves exploiting a timing side-channel vulnerability in a custom Linux CLI password authenticator.

- **Objective:** Model an HID keyboard (USB) that cracks the password by measuring subtle timing differences in keystroke validation.
- **Execution:** Once the password is cracked, a file containing your name is dropped into the victim's `$HOME` directory using automated keyboard input.
- **Challenges tackled:** German keyboard layout mapping, USB HID protocol implementation, and side-channel timing analysis using SysTick.

---

