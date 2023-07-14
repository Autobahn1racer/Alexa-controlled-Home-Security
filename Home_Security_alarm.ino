#include "thingProperties.h"
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;
 
int pir;
int pirState = 0;
 
int light = 0;
 
String light_alarm_state = "";
String movement_alarm_state = "";
String doorLock_state = "";
 
float Gx, Gy, Gz;
 
uint32_t redColor = carrier.leds.Color( 0, 255, 0);
uint32_t noColor = carrier.leds.Color( 0, 0, 0);
 
void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
 
  // Defined in thingProperties.h
  initProperties();
 
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  //Get Cloud Info/errors , 0 (only errors) up to 4
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
 
  //Wait to get cloud connection to init the carrier
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    delay(500);
  }
 
  delay(500);
  CARRIER_CASE = false;
  carrier.begin();
  pir = carrier.getBoardRevision() == 1 ? A5 : A0;
  carrier.display.setRotation(0);
  delay(1500);
  pinMode(pir, INPUT);
 
}
 
void loop() {
  ArduinoCloud.update();
  // Your code here
  pirState = digitalRead(pir);
 
  while (!carrier.Light.colorAvailable()) {
    delay(5);
  }
  int none; //not gonna be used
  carrier.Light.readColor(none, none, none, light);
 
  // read the IMU values
  carrier.IMUmodule.readGyroscope(Gx, Gy, Gz);
 
 
  if (light_alarm == true) {
 
    if (light > 200) {
      light_event = true;
    }
 
 
    if (light_event == true) {
      message_update = "Light alarm breached!";
      alarm();
    }
  }
 
  //movement alarm, can be activated from cloud
  if (movement_alarm == true) {
    if (pirState == HIGH) {
      movement_event = true;
    }
 
    if (movement_event == true) {
      message_update = "Movement alarm breached!";
      alarm();
    }
  }
 
  //shake alarm, can be activated from cloud
  if (doorLock == true) {
    if (Gy > 100 || Gy < -100) {
      shake_event = true;
    }
 
    if (shake_event == true) {
      message_update = "Shake alarm breached!";
      alarm();
    }
  }
  delay(100);
 
}
 
// the alarm() function blinks the RGB LEDs and turns the buzzer on and off
 
void alarm() {
  carrier.leds.fill(redColor, 0, 5);
  carrier.leds.show();
  carrier.Buzzer.sound(500);
  delay(500);
 
  carrier.leds.fill(noColor, 0, 5);
  carrier.leds.show();
  carrier.Buzzer.noSound();
  delay(500);
}
 
// the updateScreen() function updates the screen whenever an alarm has been activated from the cloud
 
void updateScreen() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
 
  carrier.display.setCursor(25, 60);
  carrier.display.print(light_alarm_state);
  carrier.display.setCursor(10, 100);
  carrier.display.print(movement_alarm_state);
  carrier.display.setCursor(30, 140);
  carrier.display.print(doorLock_state);
}
 
void onLightAlarmChange() {
  // Do something
  if (light_alarm == true) {
    light_alarm_state = "LIGHT ALARM: ON";
  } else {
    light_alarm_state = "LIGHT ALARM: OFF";
  }
 
  updateScreen();
}
 
void onShakeAlarmChange() {
  // Do something
  
}
 
void onMovementAlarmChange() {
  // Do something
  if (movement_alarm == true) {
    movement_alarm_state = "MOVEMENT ALARM: ON";
  } else {
    movement_alarm_state = "MOVEMENT ALARM: OFF";
  }
 
  updateScreen();
}
 
 
void onMessageUpdateChange() {
  // Do something
}
void onShakeEventChange() {
  // Do something
}
void onMovementEventChange() {
  // Do something
}
void onLightEventChange() {
  // Do something
}
/*
  Since DoorLock is READ_WRITE variable, onDoorLockChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onDoorLockChange()  {
  // Add your code here to act upon DoorLock change
  if (doorLock == true) {
    doorLock_state = "DOOR ALARM: ON";
  } else {
    doorLock_state = "DOOR ALARM: OFF";
  }
 
  updateScreen();
}