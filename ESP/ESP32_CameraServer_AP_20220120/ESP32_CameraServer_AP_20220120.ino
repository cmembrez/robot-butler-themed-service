/*
 * @Descripttion: 
 * @version: 
 * @Author: Elegoo
 * @Date: 2020-06-04 11:42:27
 * @LastEditors: Changhua
 * @LastEditTime: 2020-09-07 09:40:03
 */
//#include <EEPROM.h>
#include "CameraWebServer_AP.h"
#include <WiFi.h>
#include "esp_camera.h"
#include <PubSubClient.h>
#include <WebSocketsClient.h>      // send data (image buffer) to PiZero

WiFiClient wifiClient;
//WiFiServer server(100);
//#include <SoftwareSerial.h>
/*
const byte rxPin = 4;
const byte txPin = 8;
SoftwareSerial mySerial(rxPin, txPin);
*/
PubSubClient client(wifiClient);
//uint8_t* rgbBuffer = (uint8_t*) ps_malloc(sizeof(uint8_t)); // (3*800*600) //  (uint8_t*) malloc(sizeof(uint8_t));


#define RXD2 33
#define TXD2 4
CameraWebServer_AP CameraWebServerAP;
bool WA_en = false;

/* web socket */
WebSocketsClient webSocket;
#define USE_SERIAL Serial1

String messageMQTT = "";

/* WiFi Settings */
const char* ssid = "Sunrise_Wi-Fi_D671E0";      //  "Pixel_5164";        // "OnePlus 8T Kimmy";   //!!!!!!!!!!!!!!!!!!!!!
const char* password = "disK-1quite%noBody";    // "0988027bc02c";  // "9e4abdc003dd0";       //!!!!!!!!!!!!!!!!!!!!!

///
/// Connect to wifi "ssid" using "password"
///
void wifiConnection(){
  WiFi.mode(WIFI_STA);     // connect to wifi
  WiFi.begin(ssid, password);

  // wait for WiFi to connect
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.print(".");
  }

  // If successful
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

/* MQTT settings */
const char* mqtt_server = "192.168.1.41";
const int mqtt_port = 1883;
const char* mqtt_topic = "teddyCtrl";

/* WebSockets settings */
const char* websockets_server = "192.168.1.41";
const int websockets_port = 8080;
const char* websockets_user = "user";
const char* websockets_password = "Password";


///
/// Print the payload on the topic.
///
void callback(char* topic, byte* payload, unsigned int length){
  
  
  String totalMessage = "";

  /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print(", ");
  */

  totalMessage += "{\"";
  totalMessage += topic;
  totalMessage += "\":";
  
  for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    totalMessage += receivedChar;
  }
  totalMessage += "}";
  //Serial.println(totalMessage);
  //Serial.print("] ");
  Set_MessageMQTT(totalMessage);
  

}

void Set_MessageMQTT(String message){
  messageMQTT = message;
}


///
/// Connect and Subscribe to MQTT
///
void reconnect() {
 // Loop until we're reconnected
 Serial.println("MQTT: reconnect ................");
 
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Attempt to connect
   if (client.connect("ESP32Client")) {
     Serial.println("connected");
     // ... and subscribe to topic
     client.subscribe(mqtt_topic);
   } else {
     Serial.print("failed, rc=");
     Serial.print(client.state());
     Serial.println(" try again in 5 seconds");
     // Wait 5 seconds before retrying
     delay(5000);
   }
 }
}

/* Camera capture and transmission */

///
/// Capture a still image
///
void captureStillImage(){
  //unsigned long currentTime = millis();
  camera_fb_t* frameBuffer = esp_camera_fb_get();
  //Serial.print("esp_camera_fb_get duration: ");
  //Serial.println(millis() - currentTime);
  //Serial.print("Length of the buffy in bytes: ");
  //Serial.println(frameBuffer->len);
  //Serial.println(*frameBuffer->buf);
  //Serial.println(frameBuffer->format);
  //Serial.println(frameBuffer->height);
  //Serial.println(frameBuffer->width);  

  if(frameBuffer == NULL){
    Serial.println("frameBuffer is NULL");
  }

  /*
  if(fmt2rgb888((uint8_t *) frameBuffer->buf, frameBuffer->len, frameBuffer->format, rgbBuffer)){
    Serial.println("SUCCESS RGB!");
    webSocket.sendBIN(rgbBuffer, sizeof(rgbBuffer));
    Serial.println("BIN sent");
    if(rgbBuffer == NULL){
      Serial.println("Test pour rgbBuffer");
    }
    Serial.println("----------");
  }
  */
  //unsigned long currentTime = millis();
  webSocket.sendBIN(frameBuffer->buf, frameBuffer->len);
  //Serial.print("webSocket.sendBIN duration: ");
  //Serial.println(millis() - currentTime);

  //currentTime = millis();
  esp_camera_fb_return(frameBuffer);
  //Serial.print("esp_camera_fb_return(frameBuffer) duration: ");
  //Serial.println(millis() - currentTime);
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  
  //Serial.print("type : ");
  //Serial.println(type);

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("Connected");
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    default:
      USE_SERIAL.printf("type not handled");
      break;
	}

}

/* UART Communication to Arduino UNO */
/*
void Test_Print(String word){

  Serial.println("[Client connected]");
  String readBuff;
  String sendBuff;

  bool Heartbeat_status = false;
  bool data_begin = true;

  if (word != ""){
    char c = word.back();
    Serial.print(c);                    //Print from serial port
    if (true == data_begin && c == '{') //Received start character
    {
      data_begin = false;
    }
    if (false == data_begin && c != ' ') //Remove spaces
    {
      readBuff += c;
    }
    if (false == data_begin && c == '}') //Received end character
    {
      data_begin = true;
      Serial2.print(readBuff);
      //Serial2.print(readBuff);
      readBuff = "";
    }
  }

  if (Serial2.available())
  {
    char c = Serial2.read();
    sendBuff += c;
    if (c == '}') //Received end character
    {
      Serial.print(sendBuff); //Print from serial port
      sendBuff = "";
    }
  }       
        
}
*/


/* Original (and modified) ELEGOO Code */
///
/// Code from ELEGOO...
///
/*
void SocketServer_Test(void)
{
  static bool ED_client = true;
  WiFiClient client = server.available(); //Try to create customer objects

  
  if (client)                             //If the current customer is available
  {
    WA_en = true;
    ED_client = true;
    Serial.println("[Client connected]");
    String readBuff;
    String sendBuff;
    uint8_t Heartbeat_count = 0;
    bool Heartbeat_status = false;
    bool data_begin = true;
    while (client.connected()) //If the client is connected
    {
      if (client.available()) //If there is readable data
      {
        char c = client.read();             //Read a byte
        Serial.print(c);                    //Print from serial port
        if (true == data_begin && c == '{') //Received start character
        {
          data_begin = false;
        }
        if (false == data_begin && c != ' ') //Remove spaces
        {
          readBuff += c;
        }
        if (false == data_begin && c == '}') //Received end character
        {
          data_begin = true;
          if (true == readBuff.equals("{Heartbeat}"))
          {
            Heartbeat_status = true;
          }
          else
          {
            Serial2.print(readBuff);
          }
          //Serial2.print(readBuff);
          readBuff = "";
        }      

      }
      if (Serial2.available())
      {
        char c = Serial2.read();
        sendBuff += c;
        if (c == '}') //Received end character
        {
          client.print(sendBuff);
          Serial.print(sendBuff); //Print from serial port
          sendBuff = "";
        }
      }

      static unsigned long Heartbeat_time = 0;
      if (millis() - Heartbeat_time > 1000) //Heart rate
      {
        client.print("{Heartbeat}");
        if (true == Heartbeat_status)
        {
          Heartbeat_status = false;
          Heartbeat_count = 0;
        }
        else if (false == Heartbeat_status)
        {
          Heartbeat_count += 1;
        }
        if (Heartbeat_count > 3)
        {
          Heartbeat_count = 0;
          Heartbeat_status = false;
          break;
        }
        Heartbeat_time = millis();
      }
      static unsigned long Test_time = 0;
      if (millis() - Test_time > 1000) //Timed detection of connected devices
      {
        Test_time = millis();
        //Serial2.println(WiFi.softAPgetStationNum());
        if (0 == (WiFi.softAPgetStationNum())) //If the number of connected devices is "0", a stop command is sent to the model.
        {
          Serial2.print("{\"N\":100}");
          break;
        }
      }
    }
    Serial2.print("{\"N\":100}");
    client.stop(); //End the current connection:
    Serial.println("[Client disconnected]");
  }
  else
  {
    if (ED_client == true)
    {
      ED_client = false;
      Serial2.print("{\"N\":100}");
    }
  }

  
}
*/


///
/// Code from ELEGOO.../*Action on the test stand*/
///
void FactoryTest(void)
{
  static String readBuff;
  String sendBuff;
  if (Serial2.available())
  {
    char c = Serial2.read();
    readBuff += c;
    if (c == '}') //Received end character
    {
      if (true == readBuff.equals("{BT_detection}"))
      {
        Serial2.print("{BT_OK}");
        Serial.println("Factory...");
      }
      else if (true == readBuff.equals("{WA_detection}"))
      {
        Serial2.print("{");
        Serial2.print(CameraWebServerAP.wifi_name);
        Serial2.print("}");
        Serial.println("Factory...");
      }
      readBuff = "";
    }
  }
  {
    if ((WiFi.softAPgetStationNum())) //The number of connected devices is not "0" The led is on
    {
      if (true == WA_en)
      {
        digitalWrite(13, LOW);
        Serial2.print("{WA_OK}");
        WA_en = false;
      }
    }
    else
    {
      //Get Timestamp timestamp
      static unsigned long Test_time;
      static bool en = true;
      if (millis() - Test_time > 100)
      {
        if (false == WA_en)
        {
          Serial2.print("{WA_NO}");
          WA_en = true;
        }
        if (en == true)
        {
          en = false;
          digitalWrite(13, HIGH);
        }
        else
        {
          en = true;
          digitalWrite(13, LOW);
        }
        Test_time = millis();
      }
    }
  }
}


/* Arduino setup and loop */
///
/// Connect to WiFi, Init ESP32 Camera, and Setup MQTT
///
void setup()
{
  Serial.begin(9600);
  //Serial.println("Hello from ESP32");
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //mySerial.begin(115200);

  wifiConnection();
  String name = "hello";

  //http://192.168.4.1/control?var=framesize&val=3
  //http://192.168.4.1/Test?var=
  CameraWebServerAP.CameraWebServer_AP_Init();

  //server.begin();
  // while (Serial.read() >= 0)
  // {
  //   /*Effacer le cache du port série...*/
  // }
  // while (Serial2.read() >= 0)
  // {
  //   /*Effacer le cache du port série...*/
  // }
  //pinMode(13, OUTPUT);
  //digitalWrite(13, HIGH);
  Serial.println("Elegoo-2020...");
  //Serial2.print("{Factory}");
  //WiFiClient wifiClient = server.available();

  /* MQTT */
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  

  /* Web Socket */
  	// server address, port and URL
	webSocket.begin(websockets_server, websockets_port, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	webSocket.setAuthorization(websockets_user, websockets_password);

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);

}

///
/// 
///
void loop()
{
  String sendBuff;

  /* MQTT */
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  
  /* WEBSOCKET */
  webSocket.loop();
  //webSocket.sendTXT("message here");

  //unsigned long currentTime = millis();
  captureStillImage();
  //Serial.print("captureStillImage duration: ");
  //Serial.println(millis() - currentTime);

  //Serial.println("Test");
  
  //SocketServer_Test();
  //FactoryTest();

  Serial2.print(messageMQTT);
  
  
  //Serial.println(Serial2.read());
  //sendBuff 
  if (Serial2.available())
      {
        //Serial.println("available");
        sendBuff = Serial2.read();
        
        //Serial2.write(sendBuff);

        //Serial.println(sendBuff); //Print from serial port
        sendBuff = "";
              
      } else {
        Serial.println("Not available");
      }
      
}

/*
C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32/tools/esptool/esptool.exe --chip esp32 --port COM6 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 
0xe000 C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32/tools/partitions/boot_app0.bin 
0x1000 C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32/tools/sdk/bin/bootloader_qio_80m.bin 
0x10000 C:\Users\Faynman\Documents\Arduino\Hex/CameraWebServer_AP_20200608xxx.ino.bin 
0x8000 C:\Users\Faynman\Documents\Arduino\Hex/CameraWebServer_AP_20200608xxx.ino.partitions.bin 

flash:path
C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32\tools\partitions\boot_app0.bin
C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32\tools\sdk\bin\bootloader_dio_40m.bin
C:\Users\Faynman\Documents\Arduino\Hex\CameraWebServer_AP_20200608xxx.ino.partitions.bin
*/
//esptool.py-- port / dev / ttyUSB0-- baub 261216 write_flash-- flash_size = detect 0 GetChipID.ino.esp32.bin
