#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

// GUItool: begin automatically generated code
AudioInputI2S i2s1;   // xy=189,186
AudioOutputUSB usb1;  // xy=382,191
AudioConnection patchCord1(i2s1, 0, usb1, 0);
AudioConnection patchCord2(i2s1, 1, usb1, 1);
AudioControlSGTL5000 codec;  // xy=310,397
// GUItool: end automatically generated code

void setup(void) {
  Serial.begin(9600);
  AudioMemory(10 * (128 / AUDIO_BLOCK_SAMPLES));
  codec.enable();
  codec.volume(0.3);
}

void loop(void) {
}

