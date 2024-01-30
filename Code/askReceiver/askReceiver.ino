#include <EEPROM.h>
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

RH_ASK driver(2000, 3, 4, 0); // ESP8266 or ESP32: do not use pin 11 or 2
//rx tx ptt
// RH_ASK driver(2000, 3, 4, 0); // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85),
// RH_ASK driver(2000, PD14, PD13, 0); STM32F4 Discovery: see tx and rx on Orange and Red LEDS

int intPin = 2;

int rCH1 = 5;
int rCH2 = 6;
int rCH3 = 7;
int rCH4 = 8;

int CH1on = 0;
int CH2on = 0;
int CH3on = 0;
int CH4on = 0;

int rstr = 12;
int rcAddr = 1;
int ledState = 13;

const int maxWords = 10; // Adjust the maximum number of words as needed
String words[maxWords];
String decodedString = "";

int resetRollingCode = 0;

unsigned long rollingCode = 0;
unsigned long prevRollingCode = 0;

void setup()
{
  Serial.begin(115200);
  driver.init();
  pinMode(rCH1, OUTPUT);
  pinMode(rCH2, OUTPUT);
  pinMode(rCH3, OUTPUT);
  pinMode(rCH4, OUTPUT);

  pinMode(intPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(intPin), ReceiveSig, RISING);
  pinMode(ledState, OUTPUT);
  pinMode(rstr, INPUT_PULLUP);
  if (digitalRead(rstr) == HIGH) {
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
  ReceiveSig();
  if (words[0] == "ONE") {
    if (words[1] == String(rollingCode - 1)) {
      if (CH1on == 0) {
        digitalWrite(rCH1, HIGH);
        words[0] = "";
        words[1] = "";
        CH1on = 1;
        delay(200);
      }
      else if (CH1on == 1) {
        digitalWrite(rCH1, LOW);
        words[0] = "";
        words[1] = "";
        CH1on = 0;
        delay(200);
      }
    }
  }

  else if (words[0] == "TWO") {
    if (words[1] == String(rollingCode - 1)) {
      if (CH2on == 0) {
        digitalWrite(rCH2, HIGH);
        words[0] = "";
        words[1] = "";
        CH2on = 1;
        delay(200);
      }
      else if (CH2on = 1) {
        digitalWrite(rCH2, LOW);
        words[0] = "";
        words[1] = "";
        CH2on = 0;
        delay(200);
      }
    }
  }

  else if (words[0] == "THREE") {
    if (words[1] == String(rollingCode - 1)) {
      if (CH3on == 0) {
        digitalWrite(rCH3, HIGH);
        words[0] = "";
        words[1] = "";
        CH3on = 1;
        delay(200);
      }
      else if (CH3on == 1) {
        digitalWrite(rCH3, LOW);
        words[0] = "";
        words[1] = "";
        CH3on = 0;
        delay(200);
      }
    }
  }

  else if (words[0] == "FOUR") {
    if (words[1] == String(rollingCode - 1)) {
      if (CH4on == 0) {
        digitalWrite(rCH4, HIGH);
        words[0] = "";
        words[1] = "";
        CH4on = 1;
        delay(200);
      }
      else if (CH4on == 1) {
        digitalWrite(rCH4, LOW);
        words[0] = "";
        words[1] = "";
        CH4on = 0;
        delay(200);
      }
    }
  }
}
void ReceiveSig() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) // Non-blocking
  {
    int i;
    decodedString = ((String)(char*)buf);
    decodedString.trim();
    String inputString = decodedString;
    int spaceIndex = 0;
    int startIndex = 0;
    int wordCount = 0;

    while ((spaceIndex = inputString.indexOf('.', startIndex)) != -1) {
      String currentWord = inputString.substring(startIndex, spaceIndex);
      words[wordCount] = currentWord;

      // Move the start index to the next character after the space
      startIndex = spaceIndex + 1;
      wordCount++;

      // Break the loop if the maximum number of words is reached
      if (wordCount >= maxWords) {
        break;
      }
    }

    // Save the last word (or the only word if there's no space)
    words[wordCount] = inputString.substring(startIndex);
    wordCount++;

    // Print each separated word
    //Serial.println("Separated Words:");
    for (int i = 0; i < wordCount; i++) {
      words[i];
    }
    if (Serial.println(decodedString) and words[0] != "") {
      rollingCode++;
      EEPROM.put(rcAddr, rollingCode);
      prevRollingCode = rollingCode;
    }
    else {
      rollingCode++;
      EEPROM.put(rcAddr, rollingCode);
      prevRollingCode = rollingCode;
    }

    Serial.print("Received: ");
    Serial.print(words[1]);
    Serial.print(" Rolling Code: ");
    Serial.print(rollingCode);
    Serial.println();
  }
  digitalWrite(13, LOW);
}
