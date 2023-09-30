#include <BluetoothSerial.h>
#include <ESP32Servo.h>
#include "motor.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

BluetoothSerial SerialBT;
Motor mtr[3];
Servo servo1;
Servo servo2;

const int S1 = 18;
const int S2 = 19;
int speed1, speed2, speed3;
double spk1 = 0.3;  //スピードゲイン
double spk2 = 0.3;
double spk3 = 0.3;

String data[11];
bool databox[11];
bool limitSwitches[6];
const int limitSwitchPins[] = { 27, 26, 33, 25, 32, 35, 34 };  // io27, io26, io25, io33, io32, io35, io34

const float thresholdVoltage = 2.7;  // 閾値電圧

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test");
  Serial.println("device start");
  servo1.attach(S1, 510, 2400);
  servo2.attach(S2, 510, 2400);
  mtr[0].init(23, 21, 22, 3);
  mtr[1].init(4, 2, 0, 4);
  mtr[2].init(13, 14, 12, 2);

  for (int i = 0; i < 7; i++) {
    pinMode(limitSwitchPins[i], INPUT);
  }
}

void loop() {
  readLimitSwitches();
  updateSpeeds();
  MtrWrite(speed1, speed2, speed3);
  RaMt();
  servomt();
  
  for(int i=0; i<7; i++){
    Serial.print(limitSwitches[i]);
    Serial.print("  ");
  }
  Serial.println();
}

void MtrWrite(int mtr1, int mtr2, int mtr3) {
  (mtr1 != 0) ? mtr[0].ugoki(mtr1) : mtr[0].tomaru();
  (mtr2 != 0) ? mtr[1].ugoki(mtr2) : mtr[1].tomaru();
  (mtr3 != 0) ? mtr[2].ugoki(mtr3) : mtr[2].tomaru();
}
void RaMt() {
  if (SerialBT.available()) {
    String receivedData = SerialBT.readStringUntil('\n');
    receivedData.trim();
    int dataIndex = 0;

    while (receivedData.length() > 0 && dataIndex < 11) {
      int commaIndex = receivedData.indexOf(',');
      if (commaIndex != -1) {
        data[dataIndex] = receivedData.substring(0, commaIndex);
        receivedData = receivedData.substring(commaIndex + 1);
        data[dataIndex].trim();
        dataIndex++;
      } else {
        break;
      }
    }

    /*
    for (int i = 0; i < dataIndex; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(data[i]);
      Serial.print(", ");
    }
    Serial.println();
   */ 
  }
}

void servomt() {
  if (data[8] == "1") {
    servo1.write(10);
  } else if (data[8] == "0") {
    servo1.write(90);
  }
  if (data[9] == "1") {
    servo2.write(0);
  } else if (data[9] == "0") {
    servo2.write(80);
  }
}


void readLimitSwitches() {
  for (int i = 0; i < 7; i++) {
    limitSwitches[i] = analogRead(limitSwitchPins[i]) < (thresholdVoltage / 3.3) * 4095;
  }
}

void updateSpeeds() {
  //ウォーム速度
  if (data[0] == "1" && data[2] == "0" && limitSwitches[0]) {//ウォームリミット下
    speed1 = 255 * spk1;
  } else if (data[0] == "0" && data[2] == "1" && limitSwitches[1] ) { //ウォームリミット上
    speed1 = -255 * spk1;
  } else {
    speed1 = 0;
  }
  //ロジャー速度
  if (data[1] == "1" && data[3] == "0" && limitSwitches[2] && !limitSwitches[1]) {//ロジャーリミット上,ウォームリミット上
    speed2 = -255 * spk2;
  } else if (data[1] == "0" && data[3] == "1" && limitSwitches[3]) {//ロジャーリミット下
    speed2 = 255 * spk2 * 0.2;
  } else {
    speed2 = 0;
  }
  //お助け速度
  if (data[4] == "1" && data[6] == "0" ) {
    speed3 = -255 * spk3;
  } else if (data[4] == "0" && data[6] == "1") {
    speed3 = 255 * spk3;
  } else {
    speed3 = 0;
  }
}