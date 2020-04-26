// See: https://forum.pjrc.com/threads/60488

/*
 Try to implement the MusicDSPModel filter
 on Teensy 4.0 using the Play and Record
 queues

- added Cut off Frequency and Reasonance Pot
- added different ranges of res
- added OberheimVariationModel


200419
_3
  Add fix for MusicDSP from
  https://forum.pjrc.com/threads/60488-Porting-moog-ladder-filters-to-audio-objects?p=236189&viewfull=1#post236189

200418
_2
  Added white noise input.

200417
_1
  The samples must be normalized to a fraction
  of full-scale for input to the Process function
  and then multiplied by full-scale afterwards.
*/

/*
  If this is defined, the sine wave will
  be played unmodified to the output.
  The only difference is that the Process
  function isn't called.
  This works which proves that the synthesized
  audio is being passed through the queues
  to the output.
*/
//#define STRAIGHT_THROUGH

#include <Audio.h>
#include <ResponsiveAnalogRead.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

#define WAVE_MAX 32768.0
//=====================================================
// amplitude of signal. Should be less than WAVE_MAX
#define WAVE_AMP 32000.0f
// From martianredskies
#define INV_WAVE_AMP (1.f / 32000.0f)

// define this for noise otherwise use synthwaveform
//#define USE_NOISE

//                vvv
// Select exactly ONE of these filter options
//#define MDSPM   // MusicDSPModel
//#define RKSM    // RKSimulationModel
//#define KRAJM   // KrajeskiModel
//#define IMPRM  // ImprovedModel
//#define STSN  // StilsonModel
//#define MOOG    // MoogModel
#define OBER  // OberheimVariationModel

#ifdef MDSPM
#include "MusicDSPModel.h"
MusicDSPMoog mDSPm(44100);
const char *model_name = "MusicDSP";
float filterRes = 0.1;
float minRes = 0.1;
float maxRes = 1.0;
#endif

#ifdef RKSM
#include "RKSimulationModel.h"
RKSimulationMoog mDSPm(44100);
const char *model_name = "RKSimulation";
float filterRes = 0.1;
float minRes = 0.0;
float maxRes = 5.0;
#endif

#ifdef KRAJM
#include "KrajeskiModel.h"
KrajeskiMoog mDSPm(44100);
const char *model_name = "Krajeski";
float filterRes = 0.1;
float minRes = 0.0;
float maxRes = 1.0;
#endif

#ifdef IMPRM
#include "ImprovedModel.h"
ImprovedMoog mDSPm(44100);
const char *model_name = "Improved";
float filterRes = 0.1;
float minRes = 0.0;
float maxRes = 4.0;
#endif

#ifdef STSN
#include "StilsonModel.h"
StilsonMoog mDSPm(44100);
const char *model_name = "Stilson";
float filterRes = 0.1;
float minRes = 0.0;
float maxRes = 1.0;
#endif

#ifdef MOOG
#include "Moog.h"
// using DistoCore
MoogFilter<float> mDSPm;
const char *model_name = "Moog";
float filterRes = 0.1;
float minRes = 0.0;
float maxRes = 1.0;
#endif

#ifdef OBER
#include "OberheimVariationModel.h"
OberheimVariationMoog mDSPm(44100);
const char *model_name = "Oberheim";
float filterRes = 1.0;
float minRes = 1.0;
float maxRes = 10.0;
#endif
//=====================================================

// GUItool: begin automatically generated code
#ifdef USE_NOISE
AudioSynthNoiseWhite waveform1;
#else
AudioSynthWaveform waveform1;  // xy=154,58
#endif
AudioSynthWaveform waveform3;  // xy=154,250
AudioSynthWaveform waveform2;  // xy=158,154
AudioSynthWaveform waveform4;  // xy=162,354
AudioMixer4 mixer1;            // xy=359.9999999999999,223.9999999999999
AudioRecordQueue queue1;       // xy=501.1666603088379,222.83332586288452
AudioPlayQueue queue2;         // xy=665.333366394043,222.99999809265137
AudioOutputI2S i2s1;           // xy=826.9999809265137,223.33333206176758
AudioConnection patchCord1(waveform1, 0, mixer1, 0);
AudioConnection patchCord2(waveform3, 0, mixer1, 2);
AudioConnection patchCord3(waveform2, 0, mixer1, 1);
AudioConnection patchCord4(waveform4, 0, mixer1, 3);
AudioConnection patchCord5(mixer1, queue1);
AudioConnection patchCord6(queue2, 0, i2s1, 0);
AudioConnection patchCord7(queue2, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000;  // xy=526.3333511352539,334.50001525878906
// GUItool: end automatically generated code

// Pots
const int FREQ_PIN = PIN_A0;
const int VOL_PIN = PIN_A1;
const int RES_PIN = PIN_A2;
ResponsiveAnalogRead freqPot(FREQ_PIN, true);
ResponsiveAnalogRead volPot(VOL_PIN, true);
ResponsiveAnalogRead resPot(RES_PIN, true);

// Cutoff Freq
float filterFreq = 1500;
float minFreq = 20.0;
float maxFreq = 6000.0;

uint32_t now = 0;
void setup(void) {
  Serial.begin(9600);
  while (!Serial && (millis() - now) < 5000)
    ;
  Serial.printf("%s model\n", model_name);
#ifdef USE_NOISE
  Serial.printf("Noise input\n");
#else
  Serial.printf("Waveform input\n");
#endif
  // Audio connections require memory. and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(10 * (128 / AUDIO_BLOCK_SAMPLES));

  // Enable the audio shield. select input. and enable output
  sgtl5000.enable();
  sgtl5000.volume(0.3);
#ifdef USE_NOISE
  waveform1.amplitude(WAVE_AMP / WAVE_MAX);
#else
  //                amp   Frq  waveform
  waveform1.begin(WAVE_AMP, 220, WAVEFORM_SINE);
  waveform2.begin(WAVE_AMP, 138.5, WAVEFORM_SINE);
  waveform3.begin(WAVE_AMP, 164.5, WAVEFORM_SINE);
  //waveform4.begin(WAVE_AMP, 440, WAVEFORM_SINE);
  /*waveform1.begin(WAVE_AMP, 440, WAVEFORM_SAWTOOTH);
  waveform2.begin(WAVE_AMP, 528, WAVEFORM_SAWTOOTH);
  waveform3.begin(WAVE_AMP, 880, WAVEFORM_SAWTOOTH);
  waveform4.begin(WAVE_AMP, 1052, WAVEFORM_SAWTOOTH);*/
#endif

  // Set resonance first to initialize the required
  // value. Cutoff then recalculates resonance using
  // this value instead of the default or a garbage
  // value
  mDSPm.SetCutoff(filterFreq);
  mDSPm.SetResonance(filterRes);
  // Start the record queue
  queue1.begin();
}

float samplebuf[AUDIO_BLOCK_SAMPLES];
void loop(void) {
  short *bp;
  update_pots();
  // When an input buffer becomes available
  // process it
  if (queue1.available() >= 1) {
    bp = queue1.readBuffer();
    // Copy the int16 samples into floats
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      samplebuf[i] = *bp++ * INV_WAVE_AMP;
    }
    // Free the input audio buffers
    queue1.freeBuffer();

#ifndef STRAIGHT_THROUGH
    // This processes and returns floating point samples
    mDSPm.Process(samplebuf, AUDIO_BLOCK_SAMPLES);
#endif
    bp = queue2.getBuffer();

    // copy the processed data back to the output
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      *bp++ = samplebuf[i] * WAVE_AMP;
    }

    // and play it back into the audio queues
    queue2.playBuffer();
  }
}

void update_pots() {
  // cutoff frequency
  freqPot.update();
  if (freqPot.hasChanged()) {
    filterFreq = map(freqPot.getValue(), 0, 1023, 20, 6000);
    mDSPm.SetCutoff(filterFreq);
    Serial.printf("Cutoff = %8.2f\n", filterFreq);
  }
  // reasonance
  resPot.update();
  if (resPot.hasChanged()) {
    filterRes = ((maxRes - minRes) / 1023.0)*resPot.getValue()+minRes;
//    filterRes = float(map(resPot.getValue(), 0, 1023, 10, 100)) / 100.0;
    mDSPm.SetResonance(filterRes);
    Serial.printf("Resonance = %5.2f\n", filterRes);
  }
  // volume
  //volPot.update();
  //if (volPot.hasChanged()) {
  // static int volume = resPot.getValue();
  //  sgtl5000.volume(volume / 1023.);
  //  Serial.printf("Volume = %5.2f\n", volume / 1023.);
  //}
}