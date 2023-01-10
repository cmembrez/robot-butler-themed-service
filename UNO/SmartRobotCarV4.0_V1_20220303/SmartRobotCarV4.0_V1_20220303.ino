/*
 * @Author: ELEGOO
 * @Date: 2019-10-22 11:59:09
 * @LastEditTime: 2020-12-18 14:14:35
 * @LastEditors: Changhua
 * @Description: Smart Robot Car V4.0
 * @FilePath: 
 */
#include <avr/wdt.h>
#include "ApplicationFunctionSet_xxx0.h"
//#include <SoftwareSerial.h>

/*
const byte rxPin = 7; // 7;
const byte txPin = 8; // 8;
SoftwareSerial mySerial (rxPin, txPin);
*/

/*
#define RXD2 16
#define TXD2 17
#define USE_SERIAL Serial2
*/

void setup()
{
  // ELEGOO CODE
  Application_FunctionSet.ApplicationFunctionSet_Init();
  wdt_enable(WDTO_2S);
}

void loop()
{
  wdt_reset();

  Application_FunctionSet.ApplicationFunctionSet_SensorDataUpdate();
  Application_FunctionSet.ApplicationFunctionSet_KeyCommand();
  Application_FunctionSet.ApplicationFunctionSet_RGB();
  //Application_FunctionSet.ApplicationFunctionSet_Follow();
  //Application_FunctionSet.ApplicationFunctionSet_Obstacle();
  //Application_FunctionSet.ApplicationFunctionSet_Tracking(1);
  // Application_FunctionSet.ApplicationFunctionSet_Rocker();
  Application_FunctionSet.ApplicationFunctionSet_Standby();
  Application_FunctionSet.ApplicationFunctionSet_IRrecv();
  //Application_FunctionSet.ApplicationFunctionSet_SerialPortDataAnalysis();
  Application_FunctionSet.TeddyCtrlUARTMessage("unknown");    // handle UART message and forward to TeddyCTrlManager

/*
  Application_FunctionSet.CMD_ServoControl_xxx0();
  Application_FunctionSet.CMD_MotorControl_xxx0();
  Application_FunctionSet.CMD_CarControlTimeLimit_xxx0();
  Application_FunctionSet.CMD_CarControlNoTimeLimit_xxx0();
  Application_FunctionSet.CMD_MotorControlSpeed_xxx0();
  Application_FunctionSet.CMD_LightingControlTimeLimit_xxx0();
  Application_FunctionSet.CMD_LightingControlNoTimeLimit_xxx0();
  */
  Application_FunctionSet.CMD_ClearAllFunctions_xxx0();
}
