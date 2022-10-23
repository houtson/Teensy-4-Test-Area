
#include <Arduino.h>
//#include <CrashReport.h>
#include <EEPROM.h>
#include "eeprom_crashreport.h"

#define DBG(x) Serial.println(x);

uint32_t startTime, finishTime, t;

void setup() {
  // check for CrashReport and dump to EEPROM
  if (CrashReport) storeCrashReport();
  // set up serial for diagnostics
  Serial.begin(38400);
  while (!Serial && (millis() < 4000))
    ;
  if (Serial && CrashReport) Serial.print(CrashReport);
  pinMode(1, INPUT);
  DBG("Start Print Crash Report \n\n");
  printCrashReport();
  DBG("Setup Complete");
}

void loop() {
 // if (digitalRead(1) == HIGH) doCrash();
  delay(500);
  DBG(".");
}

void doCrash() {
  DBG("Initiating Crash");
  volatile int number = 0xbeef;
  SCB_CCR |= (1 << 4);  // enable "division by zero" hardfaults.
  Serial.println(number / 0);
}