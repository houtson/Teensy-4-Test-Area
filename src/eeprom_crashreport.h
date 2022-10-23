// class to dump CrashReport to EEPROM
// https://forum.pjrc.com/threads/69983-Using-CrashReport-when-not-connected-to-USB

#include <Arduino.h>
#include <EEProm.h>

// a starting address for the CrashReport on EEPROM
#define CR_MARK_ADDR 100                // Starting address for marker
#define CR_MARK "CRASH"                 // Unique marker
#define CR_NEW_FLAG "N"                 // When report is new it is marked as unread
#define CR_READ_FLAG "R"                // Once report read it is marked read
#define CR_START_ADDR CR_MARK_ADDR + 6  // Actual start point for the report

// Helper class to dump report
class StringDumper : public Print {
 public:
  StringDumper(const Printable& p) { this->println(p); }
  operator const char*() const { return buf.c_str(); }

 protected:
  size_t write(uint8_t b) override {
    buf.append((char)b);
    return 1;
  }
  String buf;
};

// Store CrashReport to EEPROM
void storeCrashReport() {
  StringDumper crashReport(CrashReport);
  EEPROM.put(CR_MARK_ADDR, CR_MARK);
  EEPROM.write(CR_MARK_ADDR + 5, CR_NEW_FLAG[0]);
  for (size_t i = 0; i < strlen(crashReport); i++) EEPROM.put((CR_START_ADDR + i), crashReport[i]);
  EEPROM.put((CR_START_ADDR + strlen(crashReport)), '\0');  // add \0 as terminator
}

// Check if a report is avialable on EEPROM
bool checkCrashReportAvailable() {
  if (EEPROM.read(CR_MARK_ADDR + 0) == CR_MARK[0] && EEPROM.read(CR_MARK_ADDR + 1) == CR_MARK[1] && EEPROM.read(CR_MARK_ADDR + 2) == CR_MARK[2] &&
      EEPROM.read(CR_MARK_ADDR + 3) == CR_MARK[3] && EEPROM.read(CR_MARK_ADDR + 4) == CR_MARK[4]) {
    return 1;  // return 1
  }
  return 0;  // return 0 if not
}

// Check if report is new
bool checkCrashReportNew() {
  bool avail;
  checkCrashReportAvailable() && (EEPROM.read(CR_MARK_ADDR + 5) == CR_NEW_FLAG[0]) ? avail = true : avail = false;
  return avail;
}

void printCrashReport() {
  if (Serial && checkCrashReportAvailable()) {
    for (int i = CR_START_ADDR; EEPROM[i] != '\0'; i++) {
      Serial.print((char)EEPROM[i]);
    }
    Serial.println("End");
    if (checkCrashReportNew()) EEPROM.write(CR_START_ADDR + 5, CR_READ_FLAG[0]);
  }
}