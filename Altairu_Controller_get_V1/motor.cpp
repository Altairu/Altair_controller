#include "motor.h"

Motor::Motor() {
}
void Motor::init(int apin,int bpin, int pwm,int chanel) {
  pin[0] = apin;
  pin[1] = bpin;
  pin[2] = pwm;
  chan[0] = chanel;

  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);
  ledcSetup(chan[0],960,8);
  ledcAttachPin(pin[2],chan[0]);
}

void Motor::ugoki(int syutu) {
  if (syutu > 0) {
    digitalWrite(pin[0], HIGH);
    digitalWrite(pin[1], LOW);
    ledcWrite(chan[0], syutu);
  } else if (syutu < 0) {
    digitalWrite(pin[0], LOW);
    digitalWrite(pin[1], HIGH);
    ledcWrite(chan[0], -1*  syutu);
  } else {
    digitalWrite(pin[0], LOW);
    digitalWrite(pin[1], LOW);
    ledcWrite(chan[0], 0);
  }

}

void Motor::tomaru() {
  digitalWrite(pin[0], HIGH);
  digitalWrite(pin[1], HIGH);
  ledcWrite(chan[0], 0);
}
