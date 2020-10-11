Sourcecode for project described in Volumio Forum:
https://community.volumio.org/t/wifi-remote-for-volumio-based-on-esp32-and-oled/39146

Releases can be found here:
https://github.com/drvolcano/Volumio-ESP32/releases

You may be interrested in this fork (work in progress)
https://github.com/Maschine2501/Volumio-ESP32

Relases currently only support monochrome displays that are compatible with u8g2

Newer code in the repository contains alpha-code for color-displays (SSD1351)

## Code is tested on the following hardware components:
- ESP32-WROOM-32
- SSD1327 (monochrome, 128x128, 4-Wire-SPI, using u8g2 library)
- SSD1351 (65k Color, 128x128, 4-Wire-SPI, currently Alpha)

## Configuration

Basic settings to configure your system can be made in main.h
Pins for color display have to be defined in SSD1351.h as its currently under development

## Debugging
/lib/LibDebug/LibDebug.h can be used to send debugging messages to COM port.
Valöue 0 disables debugging, 1 is normal level, 2 extended (if available)

## The project contains the following libraries:

### DigitalPin
  Debouncing and edge detection for digital inputs

### TouchPin
  Debouncing and edge detection for touch inputs

### Encoder
  Processing of AB digital encoder wheel
  
### CharStream
  Prototype for other classes like Inflate, JSON and SocketIO
  
### JSON
  Parser for JSON strings.

### Inflate
  Inflating streams, compressed by deflate algorithm.
  Needed for SocketIO
  
### SocketIO
  Send and receive data from SocketIO Server
  
### Volumio
  Connect to volumio using SocketIO
  Send commands and receive data
  
### SSD1351
  Driver class for color display
  
### GrayscaleFonts
  Font rendering for color display
  Using prerendered TTF fonts
  
### UiElements
  Helper lib for drawing to OLEd display
  Like progress bars and splitting or scrolling text
 
### U8g2Fonts
  Helper lib for rendering U8g2 fonts to color display
  Actually not used, replaced by GrayscaleFonts
 
 
  
  
  
  
