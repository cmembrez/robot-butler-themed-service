# Platform IO (PIO) Projects

## Installation

In the Arduino IDE's library manager, install:

 - ArduinoJson by Benoit Blanchon, version 6.19.4
 - WebSockets by Markus Sattler version 2.3.6
 - PubSubClient by Nick O'Leary <nick.oleary@gmail.com> version 2.8
 - Protothreads by Ben Artin <ben@artings.org>, Adam Dunkels, verison 1.4.0-arduino.beta.1


 Menu Sketch > ... > add library zip:
 - Load library from ELEGOO
   - FastLED.h: C:\Users\cedri\Downloads\LL\kit20221026\ELEGOO Smart Robot Car Kit V4.0 2022.10.26\02 Manual & Main Code & APP\02 Main Program   (Arduino UNO)\TB6612 & MPU6050\SmartRobotCarV4.0_V1_20220303\addLibrary

 Important: make sure that you have 
 - selected Board ---> ESP32 Dev Module
 - Partition Scheme ---> Huge APP (3MB No OTA/1MB SPIFFS)
 - PSRAM ---> enabled