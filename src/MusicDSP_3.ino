// See: https://forum.pjrc.com/threads/60488

/*
 Try to implement the MusicDSPModel filter
 on Teensy 4.0 using the Play and Record
 queues

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
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define WAVE_MAX 32768.0
//=====================================================
// amplitude of signal. Should be less than WAVE_MAX
#define WAVE_AMP 32000.0f
// From martianredskies
#define INV_WAVE_AMP (1.f/32000.0f)

// define this for noise otherwise use synthwaveform
//#define USE_NOISE

//                vvv
// Select exactly ONE of these filter options
#define MDSPM   // MusicDSPModel
//#define RKSM    // RKSimulationModel
//#define KRAJM   // KrajeskiModel
//#define IMPRM   // ImprovedModel
//#define STSN    // StilsonModel
//#define MOOG    // MoogModel

#ifdef MDSPM
#include "MusicDSPModel.h"
MusicDSPMoog mDSPm(44100);
const char *model_name = "MusicDSP";
#endif

#ifdef RKSM
#include "RKSimulationModel.h"
RKSimulationMoog mDSPm(44100);
const char *model_name = "RKSimulation";
#endif

#ifdef KRAJM
#include "KrajeskiModel.h"
KrajeskiMoog mDSPm(44100);
const char *model_name = "Krajeski";
#endif

#ifdef IMPRM
#include "ImprovedModel.h"
ImprovedMoog mDSPm(44100);
const char *model_name = "Improved";
#endif

#ifdef STSN
#include "StilsonModel.h"
StilsonMoog mDSPm(44100);
const char *model_name = "Stilson";
#endif

#ifdef MOOG
#include "Moog.h"
//using DistoCore
MoogFilter<float> mDSPm;
const char *model_name = "Moog";
#endif
//=====================================================

// GUItool: begin automatically generated code
#ifdef USE_NOISE
AudioSynthNoiseWhite     waveform1;
#else
AudioSynthWaveform       waveform1;      //xy=421.8833312988281,247.88333129882812
#endif
AudioRecordQueue         queue1;         //xy=569.8833312988281,248.88333129882812
AudioPlayQueue           queue2;         //xy=759.8833312988281,249.88333129882812
AudioOutputI2S           i2s1;           //xy=984.8833312988281,251.88333129882812
AudioConnection          patchCord1(waveform1, queue1);
AudioConnection          patchCord2(queue2, 0, i2s1, 0);
AudioConnection          patchCord3(queue2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000;     //xy=429.8833312988281,334.0000305175781
// GUItool: end automatically generated code


uint32_t now = 0;
void setup(void)
{
  Serial.begin(9600);
  while(!Serial && (millis() - now) < 5000);
  Serial.printf("%s model\n",model_name);
#ifdef USE_NOISE
  Serial.printf("Noise input\n");
#else
  Serial.printf("Waveform input\n");
#endif
  // Audio connections require memory. and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(10);

  // Enable the audio shield. select input. and enable output
  sgtl5000.enable();
  sgtl5000.volume(0.4);
#ifdef USE_NOISE
  waveform1.amplitude(WAVE_AMP/WAVE_MAX);
#else
//                amp   Frq  waveform
  waveform1.begin(WAVE_AMP,220/8.f,WAVEFORM_SINE);
#endif

  // Set resonance first to initialize the required
  // value. Cutoff then recalculates resonance using
  // this value instead of the default or a garbage
  // value
  // Default is 1000Hz
  float init_cutoff = 1500.0;
  // Default is 0.1 (0 to 1)
  float init_resonance = .1;
  mDSPm.SetCutoff(init_cutoff);
  mDSPm.SetResonance(init_resonance);


  Serial.printf("cutoff = %8.2f, resonance = %5.3f\n",init_cutoff,init_resonance);
  // Start the record queue
  queue1.begin();
}

float samplebuf[AUDIO_BLOCK_SAMPLES];
void loop(void)
{
  short *bp;
  // When an input buffer becomes available
  // process it
  if (queue1.available() >= 1) {
    bp = queue1.readBuffer();
    // Copy the int16 samples into floats
    for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++) {
      samplebuf[i] = *bp++ * INV_WAVE_AMP;
    }
    // Free the input audio buffers
    queue1.freeBuffer();

#ifndef STRAIGHT_THROUGH
    // This processes and returns floating point samples
    mDSPm.Process(samplebuf,AUDIO_BLOCK_SAMPLES);
#endif
    bp = queue2.getBuffer();

    // copy the processed data back to the output
    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      *bp++ = samplebuf[i]*WAVE_AMP;
    }

    // and play it back into the audio queues
    queue2.playBuffer();
  }

  // volume control
  static int volume = 0;
  // Volume control
  int n = analogRead(15);
  if (n != volume) {
    volume = n;
    sgtl5000.volume(n / 1023.);
  }

}
