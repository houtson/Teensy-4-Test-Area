#include <Audio.h>
#include <ResponsiveAnalogRead.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
#include <effect_smoothsamplerate.h>

/*
Example of sample rate reducer AudioEffectSmoothSampleRate
Expects an audioshield with an input at linein and a pot on A2
Note: uses floating points so mainly only for Teensy 4.x
*/

// GUItool: begin automatically generated code
AudioInputI2S i2s1;                       // xy=198,215
AudioMixer4 mixer1;                       // xy=404,218
AudioEffectSmoothSampleRate rateReducer;  // xy=601,211
AudioOutputI2S i2s2;                      // xy=795,216
AudioConnection patchCord1(i2s1, 0, mixer1, 0);
AudioConnection patchCord2(i2s1, 1, mixer1, 1);
AudioConnection patchCord3(mixer1, rateReducer);
AudioConnection patchCord4(rateReducer, 0, i2s2, 0);
AudioConnection patchCord5(rateReducer, 0, i2s2, 1);
AudioControlSGTL5000 sgtl5000_1;  // xy=212,422
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;
const int ANALOG_PIN = A2;

ResponsiveAnalogRead analog(ANALOG_PIN, true);
uint32_t upTime = millis();
elapsedMillis loop_timer;

    void
    setup() {
  Serial.begin(38400);
  while (!Serial && ((millis() - upTime) <= 5000))
    ;
  AudioMemory(20);
  // setup audioshield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
  // setup mixer
  mixer1.gain(0, 0.5f);
  mixer1.gain(1, 0.5f);
}

void loop() {
  static float frequ = 0.0f;
  analog.update();
  if (analog.hasChanged()) {
    frequ = float(analog.getValue()) / 1023.0f;
    rateReducer.SetFreq(frequ);
    Serial.printf("Frequency Multiplier : %0.3f Frequency : %0.0fHz\n", frequ, frequ * AUDIO_SAMPLE_RATE);
  }
  if (loop_timer >= 1000) {
    Serial.printf("Processor Use (Max) = %.2f (%.2f),  Memory (Max) = %d(%d)\n", AudioProcessorUsage(), AudioProcessorUsageMax(), AudioMemoryUsage(), AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    loop_timer = 0;
  }
}