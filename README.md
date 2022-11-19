# SEAL - SINEW Mask Power Electronics Software

This repository contains files used in Arduino-based devices which control the power supply parameters such as frequency, duty cycle, and burst ratio. One version is designed for the [Arduino Nano](https://store-usa.arduino.cc/products/arduino-nano/) (non-BLE, non-WiFi), and other for the [Seeed Studio XIAO SAMD21](https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html).  

Note the steps required to push onto this branch:
```
git init
git add "filename" (or git add . to add all files in folder)
git commit -m "message for commit"
git remote add origin https://github.com/notkevin1/SEAL_SINEW_Mask.git
git push -u origin main
```

## Technical Details for Seeeduino Script:
- Using the 48 MHz clock of GCLK0 (routed to TCC0)
  - Each timer counts up to a maximum or TOP value set by the PER register, this determines the frequency of the PWM operation: `Freq = 48MHz/(2*N*PER)`
  - With N (number of divisions of 48 MHz) set to 1, this becomes `Freq = 48MHz/(2*PER)` or `PER = 48MHz/(2*Freq)`
  - Set duty cycle by adjusting the CC[n] register, which has a relationship of `CC[2].reg = (48MHz*duty_cycle)/(2*freq)`
- Output in pin D3
- Parts of script sourced from [here](https://arduino.stackexchange.com/questions/85741/seeeduino-xiao-write-and-read-pwm-duration-period-using-timers)
