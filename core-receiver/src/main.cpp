#include <Arduino.h>
#include <DMD2.h>
#include <SoftwareSerial.h>

#define DEBUG false
#define BAUDRATE 9600

#define MAX_COUNTER 126

#define DISPLAYS_WIDE 1
#define DISPLAYS_HIGH 1

SoftDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);
DMD_TextBox box(dmd, 0, 0, 32, 16);
SoftwareSerial data(A4, A5);

void setup(void) {
  dmd.selectFont(Arial_Black_16);
  dmd.setBrightness(25);
  dmd.begin();
  box.clear();
  dmd.drawChar(02, 1, '0');
  dmd.drawChar(12, 1, '0');
  dmd.drawChar(22, 1, '0');
  dmd.drawBox(0, 0, 31, 15);
  #if DEBUG
  Serial.begin(BAUDRATE);
  #endif
  data.begin(BAUDRATE);
}

void loop(void) {
  int cout = -1;
  if (data.available() > 0) {
    cout = (byte)data.read();
  }
  if (cout != -1 && cout < MAX_COUNTER) {
    #if DEBUG
    Serial.println(cout);
    #endif
    char prints[7];
    String counter;
    counter = (cout < 10) ? ("00" + (String)cout) : (cout < 100) \
    ? ("0" + (String)cout) : (String)cout;
    counter.toCharArray(prints, 7);
    box.clear();
    dmd.drawChar(02, 1, prints[0]);
    dmd.drawChar(12, 1, prints[1]);
    dmd.drawChar(22, 1, prints[2]);
    dmd.drawBox(0, 0, 31, 15);
  }
}
