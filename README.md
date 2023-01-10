# robot-butler-themed-service
A 4-wheel robot, with a ESP32 WROVER, Arduino UNO R3, and Raspberry Pi 4B, for potential butler use in the context of a themed service (e.g. themed coffee).


It has three main parts:
- UNO: SmartRobotCar makes the robot moves. It has an Arduino UNO R3 board, motors and sensors, no WIFI
- ESP: ESP32 WROVER takes picture. It has a camera, WIFI.
- Pi: Raspberry Pi 4B process data, communicates results back to ESP. It has WIFI
