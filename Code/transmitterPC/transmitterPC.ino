#include <EEPROM.h>

#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

#include "ssd1306.h"

int ledState = 13;
int rstr = 12;
int txPin = 4;

volatile int CH1 = 6;
volatile int CH2 = 7;
volatile int CH3 = 8;
volatile int CH4 = 9;

int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

volatile int v1 = 0;
volatile int v2 = 0;
volatile int v3 = 0;
volatile int v4 = 0;

volatile int intPin = 2;

unsigned long rollingCode = 0;
unsigned long prevRollingCode = 0;

int rcAddr = 1;
int resetRollingCode = 0;

String combineMSG = "";

RH_ASK driver(2000, 5, 3, 0);
// RH_ASK driver(2000, 4, 5, 0); // ESP8266 or ESP32: do not use pin 11 or 2
// RH_ASK driver(2000, 3, 4, 0); // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85),
// RH_ASK driver(2000, PD14, PD13, 0); STM32F4 Discovery: see tx and rx on Orange and Red LEDS


void setup()
{
  Serial.begin(115200);
  driver.init();
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, HIGH);

  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);

  pinMode(ledState, OUTPUT);
  pinMode(intPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(intPin), ReadPin, HIGH);

  pinMode(rstr, INPUT_PULLUP);
  if (digitalRead(rstr) == LOW) {
    resetRollingCode = 1;
  }
  if (resetRollingCode == 0) {
    //prevRollingCode = rollingCode;
    //Serial.println("EEPROM is occupied")
    EEPROM.get(rcAddr, rollingCode);
    prevRollingCode = rollingCode;
  }
  else {
    //Serial.println("EEPROM is vacant, First use or resetted");
    EEPROM.put(rcAddr, rollingCode);
    digitalWrite(ledState, HIGH);
    delay(2000);
  }
  digitalWrite(ledState, LOW);
}

void loop() {
  ReadPin();
  if (v1 == HIGH) {
    prepSend("ONE", rollingCode);
  }

  if (v2 != LOW) {
    prepSend("TWO", rollingCode);
  }

  if (v3 != LOW) {
    prepSend("THREE", rollingCode);
  }

  if (v4 != LOW) {
    prepSend("FOUR", rollingCode);
  }
}

void ReadPin() {
  v1 = digitalRead(CH1);
  v2 = digitalRead(CH2);
  v3 = digitalRead(CH3);
  v4 = digitalRead(CH4);
}

void prepSend(String chState, unsigned long rc2send) {
  digitalWrite(txPin, HIGH);
  String rcString = String(rc2send);
  combineMSG = chState + '.' + rcString;
  int msg_length = combineMSG.length() + 1;
  char msg[msg_length];
  combineMSG.toCharArray(msg, msg_length);
  if (driver.send((uint8_t *)msg, msg_length) and combineMSG != '.' + rcString and Serial.println(combineMSG) and rollingCode <= prevRollingCode + 20) {
    //driver.waitPacketSent();
    rollingCode++;
    EEPROM.put(rcAddr, rollingCode);
  }
  prevRollingCode = rollingCode;
  delay(500);
}
