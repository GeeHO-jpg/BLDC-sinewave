# BLDC-sinewave
### Motor Control Firmware for STM32G0

This project implements embedded firmware for controlling brushless or brushed DC motors using PWM signals on an STM32G0B1CBT6 microcontroller.  
The firmware is developed with STM32CubeIDE and relies on HAL drivers for timer, GPIO, and system configuration.

#### ⚙️ Features
- PWM signal generation using TIM peripherals
- GPIO initialization for motor control lines
- Basic startup and system configuration
- Modular code separation (`pwm.c`, `tim.c`, `main.c`)
- Ready for integration with IMU or serial command interfaces

#### 📌 Target Hardware
- **Microcontroller**: STM32G0B1CBT6 (ARM Cortex-M0+)
- **Timers Used**: TIMx (configurable)
- **Power Supply**: 3.3V logic

#### 🧩 Folder Highlights
- `Core/Src/pwm.c`: Handles PWM configuration and duty cycle updates
- `Core/Src/tim.c`: Timer initialization for PWM generation
- `Core/Src/main.c`: System entry point and user logic
- `Startup/`: Assembly startup code and vector table

#### 🔧 Build & Flash
- IDE: STM32CubeIDE
- Connect ST-Link and click **Build → Flash**
- Optional: Connect motor driver to PWM output pins

> This firmware serves as a base layer for real-time motor control and is ready to be extended with communication interfaces or feedback control loops.
