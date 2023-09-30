#include <BluetoothSerial.h>
#include <ESP32Servo.h>
#include "motor.h"

BluetoothSerial SerialBT;
Motor mtr[3];
Servo servo1;
Servo servo2;

const int S1 = 19;  // サーボ１
const int S2 = 18;  // サーボ2
int speed1 = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_SSS");  // 受信側のデバイス名を設定
  Serial.println("Waiting for connection...");
  servo1.attach(S1, 510, 2400);  //出力のピン,最小値(usの値),最大値（usの値）
  servo2.attach(S2, 510, 2400);
  mtr[0].init(23, 21, 22, 0);
  mtr[1].init(4, 2, 0, 1);
  mtr[2].init(13, 14, 12, 2);
}
void loop() {

  mtr[0].ugoki(speed1);
  mtr[1].ugoki(speed1);
  mtr[2].ugoki(speed1);

  for (int speed1 = -255; speed1 <= 255; speed1++) {
    mtr[0].ugoki(speed1);
    mtr[1].ugoki(speed1);
    mtr[2].ugoki(speed1);
    servo1.write(60);  //回転角を指定
    servo2.write(60);  //回転角を指定
    delay(10);         // 少しの間待つ
  }

  for (int speed1 = 255; speed1 >= -255; speed1--) {
    mtr[0].ugoki(speed1);
    mtr[1].ugoki(speed1);
    mtr[2].ugoki(speed1);
    servo1.write(120);  //回転角を指定
    servo2.write(120);  //回転角を指定
    delay(10);
  }
}
