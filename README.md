
# PIC32 RTCC (Real Time Clock and Calender)

This repository contains a complete bare-metal driver for the PIC32MZ2048EFG100's internal Real-Time Clock Calendar (RTCC) peripheral, implemented entirely through direct register manipulation without relying on any hardware abstraction layers or middleware.



## Contents
1. Features

2. Who Should Use This Repo?

3. PIC32MZ2048EFG100

4. Prerequisites

5. Updates & Bugs

6. Deployment
## Features

✅ Pure Bare-Metal Implementation – No HAL, no middleware, just direct register manipulation.

✅ Atomic RTCC Initialization – Follows Microchip’s exact recommended hardware sequence.

✅ Peak Hour Detection – Configurable time windows for multi-tariff applications.

✅ UART Debugging Support – Displays real-time clock status.

✅ Hardware-Optimized – Uses cycle-accurate delays (NOP instructions) for stability.

✅ BCD ↔ Decimal Conversion – Efficient handling of RTC register formats.


## Who should use this rep?
This repository is ideal for:

- Embedded developers working with PIC32MZ series microcontrollers.

- Engineers who need precise RTC control without abstraction layers.

- Developers implementing time-based billing, scheduling, or logging.

- Anyone learning bare-metal PIC32 programming.
## PIC32MZ2048EFG100
The PIC32MZ2048EFG100 is a high-performance 32-bit microcontroller from Microchip, featuring:

- 200 MHz MIPS microAptiv core.

- 2MB Flash, 512KB SRAM.

- Integrated RTCC (Real-Time Clock Calendar) with battery backup support.

- Multiple communication peripherals (UART, SPI, I²C)

- This repo focuses on its hardware RTCC module, which maintains time/date even during power loss (with battery backup).
## Prerequisites
Hardware Requirements

✔ PIC32MZ2048EFG100 microcontroller

✔ 32.768 kHz crystal (for RTCC clock source)

✔ Backup battery (if continuous timekeeping is needed)

✔ UART interface (for debugging output)

Software Requirements

✔ MPLAB X IDE with XC32 compiler

✔ Datasheet (PIC32MZ Family Reference Manual, Section 37 - RTCC)

✔ Basic understanding of register-level programming
## Updates and Bugs
Planned Improvements

🔹 Multiple peak periods (e.g., morning + evening peak hours)

🔹 Day-specific rules (weekdays vs. weekends)

🔹 Leap year handling (currently relies on correct user input)

🔹 Low-power mode support (battery backup optimization)

Known Limitations

⚠ Single peak window per day (only one start/end time supported).

⚠ No timezone handling (assumes 24-hour UTC-like operation).

⚠ Year 2000-2099 assumption (2-digit BCD year storage).
## ❓ Frequently Asked Questions

#### Why use bare-metal instead of Microchip's Harmony Framework?

This implementation gives you:

- Full control over RTCC registers

- Smaller code footprint (no HAL overhead)

- Better timing precision for critical applications

- Educational value for learning PIC32 hardware internals

#### How accurate is the internal RTCC?

With a proper 32.768kHz crystal:

- ±2 minutes per month (typical)

- ±5ppm with calibration (using RTCCAL register)

- Requires battery backup for continuous timekeeping

#### Why double-read RTCTIME/RTCDATE registers?

This prevents rollover errors during:

- Clock transitions (e.g., 23:59:59 → 00:00:00)

- Register updates (RTCC syncs time/date in background)

- Ensures atomic access to time data

#### Can I use this without an external crystal?

Not recommended. The RTCC requires:

- 32.768kHz external crystal for reliable timekeeping

- Internal FRC is too unstable for RTC applications

- Crystal load capacitors (typically 12-22pF) must be added
## Authors

- [@fahad-ee](https://www.github.com/octokatherine)


## Feedback

If you have any feedback, please reach out to us at muhammadfahadx2.0@gmail.com

