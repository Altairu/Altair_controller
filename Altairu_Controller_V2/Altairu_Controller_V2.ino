#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BluetoothSerial.h>

#define OLED_ADDR 0x3C
#define OLED_SDA 21
#define OLED_SCL 22

#define BUTTON_COUNT 8
#define TOGGLE_COUNT 2

String MACadd = "EC:62:60:9C:0C:26";// 黄色 EC:62:60:9C:0C:26 本番　E8:68:E7:31:17:1E
uint8_t address[6] = { 0xe8, 0x68, 0xe7, 0x31, 0x17, 0x1e };
bool connected;

BluetoothSerial SerialBT;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int buttonPins[BUTTON_COUNT] = { 26, 32, 33, 27, 18, 19, 5, 4 };
int togglePins[TOGGLE_COUNT] = { 35, 34 };

float thresholdVoltage = 2.7;  // 閾値電圧
bool buttonStates[BUTTON_COUNT];
bool toggleStates[TOGGLE_COUNT];

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test", true);
  Serial.println("device start");

  connected = SerialBT.connect(address);
  if (connected) {
    Serial.println("Connect OK");
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("No connect");
    }
  }
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Succesfully!");
  }

  SerialBT.connect();

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < TOGGLE_COUNT; i++) {
    pinMode(togglePins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]) == LOW;
  }

  for (int i = 0; i < TOGGLE_COUNT; i++) {
    float voltage = analogRead(togglePins[i]) * (3.3 / 4095.0);  // アナログ読み取り（0-4095）を電圧に変換
    toggleStates[i] = voltage < thresholdVoltage;
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  for (int i = 0; i < BUTTON_COUNT; i++) {
    display.print("B");
    display.print(i);
    display.print(": ");
    display.println(buttonStates[i] ? "ON" : "OFF");
  }

  for (int i = 0; i < TOGGLE_COUNT; i++) {
    display.setCursor(64, i * 10);
    display.print("T");
    display.print(i);
    display.print(": ");
    display.println(toggleStates[i] ? "ON" : "OFF");
  }

  display.display();

  if (SerialBT.connected()) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
      SerialBT.print(buttonStates[i]);
      SerialBT.print(",");
    }
    for (int i = 0; i < TOGGLE_COUNT; i++) {
      SerialBT.print(toggleStates[i]);
      SerialBT.print(",");
    }
    SerialBT.println();
  }

  Serial.print("B:");
  for (int i = 0; i < BUTTON_COUNT; i++) {
    Serial.print(buttonStates[i]);
    Serial.print(",");
  }
  Serial.print("T:");
  for (int i = 0; i < TOGGLE_COUNT; i++) {
    Serial.print(toggleStates[i]);
    Serial.print(",");
  }
  Serial.println();


  delay(100);
}
