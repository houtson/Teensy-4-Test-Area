// Houtson 2020
//
// Implementation of the Moog ladder filter based on the work of Antti
// Huovilainen, described in the paper "Non-Linear Digital Implementation of the
// Moog Ladder Filter" (Proceedings of DaFX04, University of Napoli). Adapted
// from a Reaper JS source file at
// http://ajaxsoundstudio.com/cookdspdoc/moog.html
//
//

#include <math.h>
#include <stdint.h>
#include "AudioStream.h"

#ifndef TEENSY_ANTTI_FILTER_H
#define TEENSY_ANTTI_FILTER_H

#define AM_PI 3.14159265358979323846 /* pi */
#define WAVE_MAX 32768.0
//=====================================================
// amplitude of signal. Should be less than WAVE_MAX
#define WAVE_AMP 32000.0f
// From martianredskies
#define INV_WAVE_AMP (1.f / 32000.0f)


class AudioEffectAnttiMoog : public AudioStream {
 public:
  AudioEffectAnttiMoog(float sampleRate) : AudioStream(1, inputQueueArray) {}
  void compute_coeffs(float freq, float res);
  void SetCutoff(float freq);
  void SetResonance(float res);
  virtual void Process(float *samples, uint32_t n);
  virtual void update(void);

 private:

  audio_block_t *inputQueueArray[1];
  int16_t *sample_bank;
  int16_t nibble_effect_mode;
  int16_t prev_input;
  uint16_t bitmask_mute;
  uint16_t bitmask_invert;
  uint16_t bitmask_squeeze;
  bool waveform_negative;
  int diag_count = 0;

  float srate;
  float filter_scl, filter_z, filter_r4, filter_res, filter_y1, filter_y2, filter_y3, filter_y4, filter_y5, filter_out, filter_freq, filter_nylimit;
  uint32_t count = 0;
  bool print_out = false;
};
#endif


