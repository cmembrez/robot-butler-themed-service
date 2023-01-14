## Idea behind the project

In this README page, we will go through the process and reflection of a 4-wheel robot project that introduces an initial automatisation of a waiter service in the particular context of a themed coffee.

Our main idea would have been to have a butler robot that will move around thanks to black lines on the ground. There would be some gaps between the lines to indicate intersections in order to roam around and find its table.
For various reasons that could be found under the “detailed problems encountered” section, we ended up with the following goal : a 4-wheeled motorized robot that will take from a dispenser a chosen snack from a customer and go find the table thanks to a face recognition algorithm.

We based our idea after having a robot kit that contained a camera. To explain a bit more our ideal use case : there will be 3 dispensers, 1 phone, 1 robot and 2 tables under the topic of a themed café. Via an MQTT publisher on their phone, a customer orders a snack and gives their table name, in this case, the name of the character that is assigned to the table. The robot handles the request, takes the food from the dispenser and goes to the table based on their camera and face recognition with the following mindset : it moves to a detection zone freely where we have assigned some error margin then moves its camera around to detect a face. Once the correct target is detected, the camera aims the face at its center of their field of view and then faces the target and moves towards it until a predefined distance. It will then indicate that it reached its target with some sound and will understand thanks to some sensor whether the snack was taken or not. It will then announce its leave and rollback to its initial place at an idle state if it has no pending commands. This will mean having to stack in queues the commands.

As we know, this is an idyllic goal for a 4 weeks project. Thus, we decided to go with incremental steps, by defining our wished big milestones we wanted to achieve: 
1. Make face recognition working
2. Handle communication protocols
3. Make phone command
4. Make robot moves (overall possibility to make it move)
5. Calculate target distance from center and target’s depth from robot
6. Make robot moves based on a certain command
7. Create robot’s states (idle:  wait to serve; serving: move to detection area, detect target, move to target, serve and wait ; finish serving: return to idle)
8. Create dispenser (only mechanical)
9. Create robot direction to dispenser and then go to the rest of the behavior to serve
10. Split choice for more than a dispenser
11. Create robot’s behavior based on table
12. Stacking orders
13. Sound effects
14. Sensor to wait until the food is taken

By doing so, we would have at least proven some steps were possible and achievable for the implementation of a motorized butler robot based on their camera direction. This will also help us to better define, whether this approach is a good idea or whether it should be redirected for an in-real life implementation.

Before the real implementation, we decided to make a small, representative scheme of our overall system. However, the actual implementation could result in a totally different one.

//TODO insert pic "layout before implementation"

## Implementation
In this section, we will talk about how our idea was implemented and how far we were able to go from our initial goal.

Before digging into deeper details, it is relevant to mention our current components and kit in order to better grasp why some choices were redirected in a certain way. 
We decided to order a low-cost kit from (ELEGOO)[https://www.elegoo.com/products/elegoo-smart-robot-car-kit-v-3-0-plus] which included an ESP32 WROVER and Arduino UNO R3 board. We also have in our possession a Raspberry Pi 4B. For each of these three boards, you can have more information on each if you click on the link that is located on their name:
- [Arduino UNO R3](./UNO/README.md): SmartRobotCar makes the robot move. It has an Arduino UNO R3 board, motors and sensors, no WIFI
- [ESP32](./ESP/README.md): ESP32 WROVER takes pictures. It has a camera, WIFI.
- [Pi](./Pi/README.md): Raspberry Pi 4B process data, communicates results back to ESP. It has WIFI

For this project, we also used a communication via MQTT on our phone and the details about it could be found under [its section](./Phone/README.md)

First and foremost, let us explain what we were able to implement at the end of our project. Once the global frame is settled, we will dig further into the two most important parts of the implementation process : the face recognition and communication protocols.

To begin with, we managed to sort out the Elegoo libraries and code to snippet the bits of the code that focused on only moving the DC motors and the servo motor. This gave us the overall possibility to make the robot moves however we would like it to without the need to use their implemented algorithm. 

We were then able to make a face recognition algorithm work via the WebCameraServer on the ESP32. However, the frame resolution needed to be really low and the frame quite small in order to have a fluid stream. We then shifted the computation to a PiZero with openCV and sent a continuous frame stream via a websocket. For bigger power, we shifted to a Pi4. The implementation of the face recognition as well as the websocket will be more deeply discussed here, but you can find the whole details for the Pi implementation under [its own section](./Pi).
Based on our initial scheme, we soon realized that more communication protocols would be established, such as an extra UART communication between the Arduino Uno R3 and the ESP32. Contrary to what we initially thought, this Arduino board actually does not have any wifi available. Those iterations resulted in the following scheme where we were able to handle all the communications quite stably.

//TODO insert pic "layout implemented"

We specified on the scheme that a JSON is being sent via MQTT and UART. It shows here where the packet is initially sent to and who is the real receiver. For this JSON, the ESP32 is being used as a middleware to communicate between the two boards. This scheme shows us that a phone was able to successfully connect itself to the broker that is being hosted by the Pi4 and is sending information, here the name of the target that defines the table where the customer is located. The JSON contains actually the depth between a recognized face and the camera, the name of the face and the offset on the x-axis based on the center of the frame and the center of the face. This JSON, that is sent to the Arduino, will actually trigger a method based on the name recognized, the one given by the customer and the offset value that will start to make the robot move its wheels or the camera.

Now that the overall system has been explained, we can focus on the 2 main parts of it.
### Face recognition
//TODO general explanation OpenCV, explain code : decode, landmark, encoding, (frame, depth)
### Communication protocol
#### Websocket
//TODO  Aiohttp, Pi4 and ESP32 → constant communication to stream camera frame
#### MQTT
//TODO Broker Pi4 → mosquitto, Client : Pi4 (acts as 2, needs to redirect as broker but also communicates as it’s computing the face recognition), phone, ESP32. Phone → gives the target's name. Pi4 → gives name recognized, depth and x-offset, receive target’s name and check if it’s the same as recognized. If yes, send as JSON string. ESP32 → receives name and information as JSON string
#### UART
//TODO ESP32 to Arduino Uno R3 → gives JSON with information for further instruction and behavior control
#### JSON construction and deserialization
//TODO Created in pi, during loop, englobe it with MQTT topic in ESP32, deserialized in arduino
## Technical issues
