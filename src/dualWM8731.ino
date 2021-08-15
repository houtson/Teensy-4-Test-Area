#include <Audio.h>
#include <BALibrary.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
using namespace BALibrary;
BAAudioControlWM8731 audioShield;

// GUItool: begin automatically generated code
AudioInputI2SQuad i2s_quad1;   // xy=150,69
AudioOutputI2SQuad i2s_quad2;  // xy=365,94
AudioConnection patchCord1(i2s_quad1, 0, i2s_quad2, 2);
AudioConnection patchCord2(i2s_quad1, 1, i2s_quad2, 3);
AudioConnection patchCord3(i2s_quad1, 2, i2s_quad2, 0);
AudioConnection patchCord4(i2s_quad1, 3, i2s_quad2, 1);
AudioControlSGTL5000 sgtl5000_1;  // xy=302,184
AudioControlSGTL5000 sgtl5000_2;  // xy=302,254
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;
// const int myInput = AUDIO_INPUT_MIC;

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the first audio shield, select input, and enable output
  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  // Enable the second audio shield, select input, and enable output
  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(myInput);
  sgtl5000_2.volume(0.5);
}

void loop() {}
