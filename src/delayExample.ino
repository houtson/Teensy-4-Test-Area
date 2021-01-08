// example of effect_delay10tap
//

#include <Audio.h>
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



