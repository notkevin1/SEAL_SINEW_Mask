# SEAL - Plasma Group Power Electronics Software

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
- Output = pin D2 (A3 or B3 on breadboard), GND = pin 2 (J3 on breadboard)
- Parts of script sourced from [here](https://arduino.stackexchange.com/questions/85741/seeeduino-xiao-write-and-read-pwm-duration-period-using-timers)

<!-- ![Seeeduino XIAO](https://files.seeedstudio.com/wiki/Seeeduino-XIAO/img/Seeeduino-XIAO-pinout-1.jpg) -->
### Wiring Diagrams/Schematic
<!-- ![Seeeduino_OLED_schem (1)](https://user-images.githubusercontent.com/61093711/205526785-302dbbd8-a1d2-492f-8910-1b07e42a27aa.png)
![Seeeduino_OLED_bb (1)](https://user-images.githubusercontent.com/61093711/205526779-3df1838e-d886-452e-9c88-fbf7f64f901e.png) 
![seeeduino_schematic](https://user-images.githubusercontent.com/61093711/217952065-acf7c9aa-363b-4d76-bf9a-c653a03f8402.png) -->
![updated_schematic](https://user-images.githubusercontent.com/61093711/221732942-d0582485-4d57-4b1e-9d6b-80b3ec2cc07c.png)


### Example Waveform - Continuous Frequency
<!-- ![scope_6](https://user-images.githubusercontent.com/61093711/211942071-3a559696-8df3-46fb-bc6e-bdae4dcd3852.png) -->
![waveform_and_device](https://user-images.githubusercontent.com/61093711/215390581-c39daa24-87d7-4491-8c49-916f191338b3.png)
### Example Waveform - Base Frequency @ 20 kHz (Duty Cycle = 50%), Burst Frequency = 1 kHz (Burst Ratio = 30%)
![scope_12](https://user-images.githubusercontent.com/61093711/219525169-de02213f-99c3-4996-a456-67aaa537dff7.png)
<img width="842" alt="image" src="https://user-images.githubusercontent.com/61093711/218615347-129ae33b-2ccf-4818-8b12-7658cf702de7.png">

