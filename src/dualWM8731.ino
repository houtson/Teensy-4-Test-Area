#include <Audio.h>
#include <BALibrary.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
using namespace BALibrary;
BAAudioControlWM8731 audioShield;

// GUItool: begin automatically generated code
AudioSynthWaveform waveform4;  // xy=410,351
AudioSynthWaveform waveform1;  // xy=411,196
AudioSynthWaveform waveform2;  // xy=411,247
AudioSynthWaveform waveform3;  // xy=411,299
AudioOutputI2SQuad i2s_quad1;  // xy=632,273
AudioConnection patchCord1(waveform4, 0, i2s_quad1, 3);
AudioConnection patchCord2(waveform1, 0, i2s_quad1, 0);
AudioConnection patchCord3(waveform2, 0, i2s_quad1, 1);
AudioConnection patchCord4(waveform3, 0, i2s_quad1, 2);
using namespace BALibrary;
BAAudioControlWM8731 wm8731_1;  // xy=632,378
BAAudioControlWM8731 wm8731_2;  // xy=632,378
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;
// const int myInput = AUDIO_INPUT_MIC;

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the first audio shield, select input, and enable output
  wm8731_1.setAddress(LOW);
  wm8731_1.enable();
  wm8731_1.setDacMute(true);
  wm8731_1.setAdcBypass(false);
  wm8731_1.setHPFDisable(true);
  wm8731_1.setDacSelect(true);
  wm8731_1.setLinkLeftRightIn(true);
  wm8731_1.setLeftInputGain(0);
  wm8731_1.setDacMute(false);

  // Enable the second audio shield, select input, and enable output
  wm8731_2.setAddress(HIGH);
  wm8731_2.enable();
  wm8731_2.setDacMute(true);
  wm8731_2.setAdcBypass(false);
  wm8731_2.setHPFDisable(true);
  wm8731_2.setDacSelect(true);
  wm8731_2.setLinkLeftRightIn(true);
  wm8731_2.setLeftInputGain(0);
  wm8731_2.setDacMute(false);

  // start the oscilators
  //                amp   Frq  waveform
  waveform1.begin(WAVE_AMP, 131, WAVEFORM_SAWTOOTH);
  waveform2.begin(WAVE_AMP, 196, WAVEFORM_SAWTOOTH);
  waveform3.begin(WAVE_AMP, 262, WAVEFORM_SAWTOOTH);
  waveform4.begin(WAVE_AMP, 330, WAVEFORM_SAWTOOTH);
}

void loop() {}
