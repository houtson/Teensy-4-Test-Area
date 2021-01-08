// example of effect_delay10tap
//

#include <Audio.h>
#include <ResponsiveAnalogRead.h>
#include <SD.h>
#include <SPI.h>

#include "effect_delay10tap.h"

// GUItool: begin automatically generated code
AudioInputI2S i2s_in;          // xy=196,180
AudioMixer4 mixer1;            // xy=434,190
AudioEffectDelay10tap delay1;  // xy=638,182
AudioOutputI2S i2s_out;        // xy=1038,218
AudioConnection patchCord1(i2s_in, 0, mixer1, 0);
AudioConnection patchCord2(i2s_in, 1, mixer1, 1);
AudioConnection patchCord3(mixer1, delay1);
AudioConnection patchCord4(delay1, 0, mixer1, 2);
AudioConnection patchCord5(delay1, 0, i2s_out, 0);
AudioConnection patchCord6(delay1, 0, i2s_out, 1);
AudioControlSGTL5000 sgtl5000_1;  // xy=407,367
// GUItool: end automatically generated code

// delay line
#define DELAYLINE_MAX_LEN 45159  // number of samples at 44100 samples a second
int16_t delay_line[DELAYLINE_MAX_LEN] = {};

// main timing loop
#define LOOP0_DURATION 20  // interval time in millis
elapsedMillis loop0_timer;
// pot to control delaytime
const int DELAY_TIME_KNOB_PIN = A0;  // A12 gain
ResponsiveAnalogRead delayTimeKnob(DELAY_TIME_KNOB_PIN, true);

void setup() {
  Serial.begin(9600);

  // Enable the audio shield and set the output volume.
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.5);

  // alocate some audio memory, don't need much as passing an array to the effect
  AudioMemory(15);

  // set up the mixer including some feedback
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0.6);

  // start up the effect and pass it an array to store the samples
  delay1.begin(delay_line, DELAYLINE_MAX_LEN);
}

void loop() {
  // loop timer
  if (loop0_timer >= LOOP0_DURATION) {
    // update and check the pot
    delayTimeKnob.update();
    if (delayTimeKnob.hasChanged()) {
      //Serial.printf("Delay Time Knob:%d\n", delayTimeKnob.getValue());
      delay1.delaysmooth(0, delayTimeKnob.getValue());
    }
    loop0_timer = 0;
  }
}
