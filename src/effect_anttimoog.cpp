// Houtson 2020
//
// Implementation of the Moog ladder filter based on the work of Antti
// Huovilainen, described in the paper "Non-Linear Digital Implementation of the
// Moog Ladder Filter" (Proceedings of DaFX04, University of Napoli). Adapted
// from a Reaper JS source file at
// http://ajaxsoundstudio.com/cookdspdoc/moog.html
//
//

#include "effect_anttimoog.h"
#include <Arduino.h>

#ifndef TEENSY_ANTTI_FILTERCPP_H
#define TEENSY_ANTTI_FILTERCPP_H
#define DIAG_PER_SAMPLES 0  // set to 0 for no DIAG or >0 for "print some diag every x samples" e.g. 500

AudioEffectAnttiMoog::AudioEffectAnttiMoog(float sampleRate) {
  filter_y1 = filter_y2 = filter_y3 = filter_y4 = filter_y5 = filter_out = 0;
  srate = sampleRate;
  filter_nylimit = srate * 0.49;
  filter_z = 1.0 / 40000.0;  // twice the "thermal voltage of a transistor"
  SetCutoff(1000.0f);
  SetResonance(0.1f);
  compute_coeffs(filter_freq, filter_res);
};

void compute_coeffs(float freq, float res) {
  float f_, fc_, fc2_, fc3_, fcr_, acr_;
  if (freq < 1) {
    freq = 1;
  } else {
    if (freq > filter_nylimit) freq = filter_nylimit;
  }
  if (res < 0) {
    res = 0;
  } else {
    if (res > 1) res = 1;
  }
  filter_freq = freq;
  filter_res = res;
  // srate is half the actual filter sampling rate
  fc_ = filter_freq / srate;
  f_ = 0.5 * fc_;
  fc2_ = fc_ * fc_;
  fc3_ = fc2_ * fc_;
  // frequency &amp; amplitude correction
  fcr_ = 1.8730 * fc3_ + 0.4955 * fc2_ - 0.6490 * fc_ + 0.9988;
  acr_ = -3.9364 * fc2_ + 1.8409 * fc_ + 0.9968;
  filter_scl = float((1.0 - exp(-2 * AM_PI * f_ * fcr_)) / filter_z);  // filter tuning
  filter_r4 = 3.96 * filter_res * acr_;                                // filter feedback
};

void SetCutoff(float freq) {
  if (freq != filter_freq) compute_coeffs(freq, filter_res);
};

void SetResonance(float res) {
  if (res != filter_res) compute_coeffs(filter_freq, res);
};

virtual void Process(float *samples, uint32_t n) {
  for (uint32_t s = 0; s < n; ++s) {
    for (int a = 0; a < 2; a++) {
      filter_y1 = filter_y1 + filter_scl * (tanh((samples[s] - filter_r4 * filter_out) * filter_z) - tanh(filter_y1 * filter_z));
      filter_y2 = filter_y2 + filter_scl * (tanh(filter_y1 * filter_z) - tanh(filter_y2 * filter_z));
      filter_y3 = filter_y3 + filter_scl * (tanh(filter_y2 * filter_z) - tanh(filter_y3 * filter_z));
      filter_y4 = filter_y4 + filter_scl * (tanh(filter_y3 * filter_z) - tanh(filter_y4 * filter_z));
      filter_out = (filter_y4 + filter_y5) * 0.5;
      filter_y5 = filter_y4;
    }
    samples[s] = filter_out;
  }
};

void AudioEffectAnttiMoog::update(void) {
  uint16_t sample_data_unsigned;
  audio_block_t *block;

  block = receiveWritable(0);
  if (!block) return;

  for (uint32_t s = 0; s < AUDIO_BLOCK_SAMPLES; ++s) {
    float current_sample = block->data[s] * INV_WAVE_AMP;
    for (int a = 0; a < 2; a++) {
      filter_y1 = filter_y1 + filter_scl * (tanh((current_sample - filter_r4 * filter_out) * filter_z) - tanh(filter_y1 * filter_z));
      filter_y2 = filter_y2 + filter_scl * (tanh(filter_y1 * filter_z) - tanh(filter_y2 * filter_z));
      filter_y3 = filter_y3 + filter_scl * (tanh(filter_y2 * filter_z) - tanh(filter_y3 * filter_z));
      filter_y4 = filter_y4 + filter_scl * (tanh(filter_y3 * filter_z) - tanh(filter_y4 * filter_z));
      filter_out = (filter_y4 + filter_y5) * 0.5;
      filter_y5 = filter_y4;
    }
    block->data[s] = filter_out * WAVE_AMP;
  }

  // print some diags
  if (DIAG_PER_SAMPLES && diag_count > DIAG_PER_SAMPLES) {
    diag_count = 0;
  }
  if (DIAG_PER_SAMPLES) diag_count++;

  transmit(block);
  release(block);
}
#endif