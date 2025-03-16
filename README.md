# ESP32_FreeRTOS_Test
Test ESP32 multi-tasking runs on Core 0 and Core 1 simultaneously.
ESP32 test run sketch on 2 Cores separately
- Core 0 Animation
- Core 1 Nes Music Emulator
  
[Youtube](https://youtu.be/w-QqZ7T1ar8)

![Image](rtos.jpg)

this project use add-on library from
[Nes Audio](https://github.com/connornishijima/Cartridge)
[TFT Dsiplay](https://github.com/Bodmer/TFT_eSPI)

## Software:
- Arduino IDE 2.3.2 + ESP32 arduino core 2.0.17
- * IMPORTANT * this project won't work with ESP32 arduiino core 3.x.x
- modified TFT_eSPI library 2.5.43
  
## Hardware
- ESP32 TFT2.8 320x240 with "RESISTIVE" touch board.
  buy here (https://s.click.aliexpress.com/e/_Ew95gMl)
 https://www.youtube.com/watch?v=d2OXlVcRYrU
- A small speaker for mobile phone 1Watt
