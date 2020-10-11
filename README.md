Sourcecode for project described in Volumio Forum:

https://community.volumio.org/t/wifi-remote-for-volumio-based-on-esp32-and-oled/39146

Releases can be found here:

https://github.com/drvolcano/Volumio-ESP32/releases

Relases currently only support monochrome displays that are compatible with u8g2

Newer code in the repository contains alpha-code for color-displays (SSD1351)

Code is tested on the following hardware components:
- ESP32-WROOM-32
- SSD1327 (Monochromme, 128x128, 4-Wire-SPI, using u8g2 library)
- SSD1351 (65k Color, 128x128, 4-Wire-SPI, currently Alpha)

Basic settings to adapt it to you system can be made in main.h
Pins for color display have to be defined in SSD1351.h

You may be interrested in this fork (work in progress)
https://github.com/Maschine2501/Volumio-ESP32
