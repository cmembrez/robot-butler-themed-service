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
In order to compute facial recognition, we had the options between MediaPipe and OpenCV. We actually ended up prioritizing OpenCV as it is a lighter library that has less functionalities and possibilities rather than MediaPipe. However, we knew that having less features didn’t mean being easier to use, but it would have been easier for us to go through the documentation and the information. We also realized that a lot of documentation was available for OpenCV. This is why we leaned towards this option.
OpenCV is an Open Source Computer Vision library that will allow us to use already made methods and also pre-built configuration in order to compute our face recognition algorithm.
We won’t explain here the dependencies and installation process on the Pi, you can find it in [its section](./Pi). We also won’t be covering the exact algorithm explanation used with OpenCV as it goes outside the reach and needs of this project. However, their [documentation](https://docs.opencv.org/3.4/da/d60/tutorial_face_main.html) covers this topic well if wanted.
We decided to go for a pre-built version where we modified the face_recognition.py duplicata with our needs. We also looked at the [package documentation](https://face-recognition.readthedocs.io/en/latest/face_recognition.html) as well as the [api](https://face-recognition.readthedocs.io/en/latest/_modules/face_recognition/api.html) to have a better grasp of the methods’ meanings and use.
As we are continuously receiving a stream of frame from the ESP32, we actually didn’t need to capture any images. Rather, we needed to decode the received buffer into a suitable, working for the rest of the computation.

The method ```cv.imdecode()``` is actually [reading a image from a buffer](https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html).

Based on that, we took the data of our message in the decode method and assign it to a variable frame in order to use it for the rest of the computation : 
```
frame = cv2.imdecode(np.frombuffer(msg.data, np.uint8), 1)
```

We then use two different computation on the frame : [face landmark](https://datagen.tech/guides/face-recognition/facial-landmarks/), to have facial feature locations in order to calculate the depth and the center of the face and face location and encodings : one to surround a face with a bounding box and the other to compute facial features distinctions in order to distinguish people’s faces.

```
boxes = face_recognition.face_locations(frame)
```
```
encodings = face_recognition.face_encodings(frame, boxes)
```
```
face_landmarks_list = face_recognition.face_landmarks(frame)
```

Every one of those variables are actually an array of values, the more faces there will be in the frame, the greater the array.
#### Face landmark
To begin with the face landmark : we took inspiration on a [tutorial](https://www.youtube.com/watch?v=jsoe1M2AjFk) that explained how to compare the proportional difference between a fixed, in real life constant and the one in the frame. It explained that the eye offset is an almost constant feature on people’s face and it’s generally 62[mm] for women and 64[mm] for men. Thus, we decided to put this eye distance as 63[mm] as it’s the average. 
We do not need precise measurements as it won’t significantly impact the result.
In order to have the most accurate point for the center of each eye, we looked at the enumeration of the facial mesh points and decided to take the inner and outer corner of the eye. The center would be defined as the iris.

> *NB* :  we shouldn’t forget that the face_landmark is a dictionary. Thus we need to assign for each of the features their respective array.

``` 
left_eye = np.array(face_landmark['left_eye'])
right_eye = np.array(face_landmark['right_eye'])
nose_bridge = np.array(face_landmark['nose_bridge'])
nose_tip = np.array(face_landmark['nose_tip'])
```

The computation for the center that will lead to two sets of coordinates of the left and right are is the following :
```
midPoint_left_eye = ((left_eye[0][0]+left_eye[3][0])/2, (left_eye[0][1]+left_eye[3][1])/2)
midPoint_right_eye = ((right_eye[0][0]+right_eye[3][0])/2, (right_eye[0][1]+right_eye[3][1])/2)
```

Here, the corners have indexes that are specific to the list where they were contained in the dictionary. This is why their indexes don’t match with the numbers in the picture. However, you have more indication in the method that lets you interpret which point matches to which.

After having the two points that will serve as a depth calculator, we will also find the center of our face. Theoretically, we could only take the tip of the nose. But for a stronger algorithm and precision, we decided to average multiple points on the nose bridge : actually the whole column with the nose tip and corners.

> NB : we only took the value for the x axis as we only need to know the offset in x in order to center the face, so the offset will be a single variable and not a vector. It is also important to note that the layout information is based with the center of the coordinate system at the top left of the frame, we will need to recenter it by subtracting the width of the frame.

```
## calculate averaging for nose that will be considered as center of the face. NB, only x is taken in account, no need to have Y centered
# to have as if the axis was in the middle, take frame width and subtract the position of nose
sumPosXY = nose_bridge.sum(axis = 0) + nose_tip[0] + nose_tip[2] + nose_tip[4]
totalElement = nose_bridge.shape[0] + 3
```
```
xPos = frameWidth - sumPosXY[0]/totalElement
```

As we do not need to have too many decimals after the coma and also for computation length reasons, we rounded the two coordinates for the center of the eyes.

We then, based on the computation of the thin lens formula, we managed to isolate the required formula where $D$, the distance, is the variable that we are searching to compute : $D = \frac{f*(y’+y)}{y’}$

//TODO pic 1 and 2 of CV lecture 2

(source : computer vision course from Prof FAVARO lecture 2a on camera)

Here is the graphical explanation on how to determine the equalities between the multiples triangles that form each length. In our code, the $y’$ represent the  ```w```, the computed eye distance in the frame and  $y$ the  ```W```, the constant of the in real life eye distance. The D’ is the focus distance, the distance between the lens and where the image is reflected. The focal length f is the distance between the sensor and the lens. We assume knowing $y’$, $y$ and $f$. We want to find $D$ and need to substitute $D’$ with the known variables in order to have the final equation. Thus we computed the following formula : 
**Equation n°1**
$${\frac{y’}{y}} = {\frac{D’}{D}} \Leftrightarrow D = \frac{D’y}{y}$$
On the equation n°1, we wish to isolate the only occurrence of our depth $D$
**Equation n°2**
$${\frac{y’}{y}} = {\frac{D’-f}{f}} \Leftrightarrow  {\frac{y’f}{y}} = {D’-f} \Leftrightarrow D’ = {\frac{y’f}{y}+f}$$
On the equation n°2, our only unknown value is $D’$, this is why we try to isolate it in order to inject the equation n°2 in the n°1. It will give us a third equation : 
$$D = {\frac{{\frac{y’f}{y}+f}y}{y}} \Leftrightarrow D = {\frac{y’f+yf}{y’}} \Leftrightarrow D = {\frac{f(y’+y)f}{y’}}$$

We firstly assume to know the constant f in order to find the correct formula for our depth computation, however it is not the case with our ESP32 WROVER. For the first iteration, we locked the distance to a predefined D and measured the result of f that will come out when we isolate it from the equation n°3 as the following : $f = \frac{Dy’}{y’+y}$. We then hardcoded the f into our computation and put back the original formula in n°3., which means we have 
$$D = \frac{f(W+w)}{w}$$
#### Face encodings
For the face encodings, we actually haven’t done so much beside taking the pre-built file. Indeed, there is already a method called ```compare_faces()```, that checks if there are any matches between every face in the frame and the trained models in the datasets. If so, the label under the face’s name is changed to the name they belong to, otherwise, it stays “unknown”. 
We store all of the values to call them in the loop for the JSON string creation.

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
