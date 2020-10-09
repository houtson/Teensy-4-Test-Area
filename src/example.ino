#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

#include "effect_reverser.h"

// GUItool: begin automatically generated code
AudioInputI2S i2s1;            // xy=188,102
AudioEffectReverser reverser;  // xy=408,104
AudioOutputI2S i2s2;           // xy=619,107
AudioConnection patchCord1(i2s1, 0, reverser, 0);
AudioConnection patchCord2(reverser, 0, i2s2, 0);
AudioConnection patchCord3(reverser, 0, i2s2, 1);
AudioControlSGTL5000 sgtl5000_1;  // xy=211,214
// GUItool: end automatically generated code

#define DELAYLINE_MAX_LEN 80000  // number of samples at 44100 samples a second
int16_t delay_line[DELAYLINE_MAX_LEN] = {};

void setup() {
  // Enable the audio shield and set the output volume.
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.5);

  // alocate some audio memory, don't need much as passing an array to the effect
  AudioMemory(15);
  
  // start up the effect and pass it an array to store the samples
  reverser.begin(delay_line, DELAYLINE_MAX_LEN);
}

void loop() {
  // do stuff
}
