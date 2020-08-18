#include <Arduino.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define DEBUG true
#define SERIAL_SPEED 9600

#define ROWS 4
#define COLS 4

#define CONS 1
#define UPER 2
#define DOWN 3

#define LEFTS 0
#define RIGHTS 1

#define REFRESH 2000
#define MAX_EEPROM 10
#define MAX_COUNTER 126

byte pinRows[ROWS] = {9, 8, 7, 6};
byte pinCols[COLS] = {5, 4, 3, 2};
char keyChar[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};

static byte symbolLefts = CONS;
static byte symbolRight = CONS;
static int16_t counterLefts = 0;
static int16_t counterRight = 0;
String counterLeftsString, counterRightString;

SoftwareSerial teamLefts(10, 11);
SoftwareSerial teamRight(12, 13);
LiquidCrystal lcd(A4, A5, A0, A1, A2, A3);
Keypad keypad = Keypad(makeKeymap(keyChar), pinRows, pinCols, ROWS, COLS);

const byte numCustomChar = 4;
byte customChar[numCustomChar][8] = {
  {0x10, 0x14, 0x16, 0x1f, 0x16, 0x14, 0x10, 0x00},
  {0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x00, 0x00, 0x00},
  {0x04, 0x0E, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x00},
  {0x04, 0x04, 0x04, 0x04, 0x1f, 0x0e, 0x04, 0x00},
};

void setup(void) {
  if (EEPROM.read(10)) {
    for (int i = 0; i <= MAX_EEPROM; i ++) {
      EEPROM.write(i, 0x00);
    }
  }

  lcd.begin(16, 2);
  lcd.clear();
  for (int i = 0; i < numCustomChar; i++) {
    lcd.createChar((byte)i, customChar[i]);
  }
  teamLefts.begin(SERIAL_SPEED);
  teamRight.begin(SERIAL_SPEED);
}

void loop(void) {
  main: {}
  lcd.setCursor(0, 0); lcd.write((byte)0);
  lcd.setCursor(0, 1); lcd.write((byte)0);
  lcd.setCursor(2, 0); lcd.print("TIM A : 000");
  lcd.setCursor(2, 1); lcd.print("TIM B : 000");
  lcd.setCursor(14,0); lcd.write((byte)CONS);
  lcd.setCursor(14,1); lcd.write((byte)CONS);
  counterLefts = EEPROM.read(LEFTS);
  counterRight = EEPROM.read(RIGHTS);
  teamLefts.write(counterLefts);
  teamRight.write(counterRight);
  counterLefts = counterLefts < 0 ? 0 : counterLefts > MAX_COUNTER ? MAX_COUNTER : counterLefts;
  counterLeftsString = (counterLefts < 10) ? ("00" + (String)counterLefts) \
  : (counterLefts < 100) ? ("0" + (String)counterLefts) : (String)counterLefts;
  counterRight = counterRight < 0 ? 0 : counterRight > MAX_COUNTER ? MAX_COUNTER : counterRight;
  counterRightString = (counterRight < 10) ? ("00" + (String)counterRight) \
  : (counterRight < 100) ? ("0" + (String)counterRight) : (String)counterRight;
  while (true) {
    char key = keypad.getKey();

    if (key != '\0') {
      if (key == '1' || key == '4') {
        if (key == '1') { counterLefts ++; symbolLefts = UPER; symbolRight = CONS; }
        if (key == '4') { counterLefts --; symbolLefts = DOWN; symbolRight = CONS; }
        counterLefts = counterLefts < 0 ? 0 : counterLefts > MAX_COUNTER ? MAX_COUNTER : counterLefts;
        counterLeftsString = (counterLefts < 10) ? ("00" + (String)counterLefts) \
        : (counterLefts < 100) ? ("0" + (String)counterLefts) : (String)counterLefts;
        teamLefts.write(counterLefts);
        EEPROM.write(LEFTS, counterLefts);
      }
      if (key == 'A' || key == 'B') {
        if (key == 'A') { counterRight ++; symbolLefts = CONS; symbolRight = UPER; }
        if (key == 'B') { counterRight --; symbolLefts = CONS; symbolRight = DOWN; }
        counterRight = counterRight < 0 ? 0 : counterRight > MAX_COUNTER ? MAX_COUNTER : counterRight;
        counterRightString = (counterRight < 10) ? ("00" + (String)counterRight) \
        : (counterRight < 100) ? ("0" + (String)counterRight) : (String)counterRight;
        teamRight.write(counterRight);
        EEPROM.write(RIGHTS, counterRight);
      }
      if (key == '0') {
        lcd.clear();
        goto reset;
      }
    }
    lcd.setCursor(10,0); lcd.print(counterLeftsString);
    lcd.setCursor(10,1); lcd.print(counterRightString);
    lcd.setCursor(14,0); lcd.write((byte)symbolLefts);
    lcd.setCursor(14,1); lcd.write((byte)symbolRight);
    static unsigned long prevMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > REFRESH) {
      prevMillis = currentMillis;
      teamLefts.write(counterLefts);
      teamRight.write(counterRight);
    }
  }
  reset: {}
  while (true) {
    lcd.setCursor(0, 0); lcd.write((byte)0);
    lcd.setCursor(0, 1); lcd.write((byte)0);
    lcd.setCursor(2, 0); lcd.print("RESET ?...");
    lcd.setCursor(2, 1); lcd.print("*YES #NO");
    char key = keypad.getKey();
    if (key == '*') {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.write((byte)0);
      lcd.setCursor(2, 0); lcd.print("WAIT...");
      for (int i = 0; i <= MAX_EEPROM; i++) {
        EEPROM.write(i, 0x00);
        delay(100);
      }
      symbolLefts = CONS;
      symbolRight = CONS;
      lcd.clear();
      goto main;
    }
    if (key == '#') {
      lcd.clear();
      goto main;
    }
  }
}
