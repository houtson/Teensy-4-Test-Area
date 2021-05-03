#include "effect_smoothsamplerate.h"
#include <Arduino.h>

void AudioEffectSmoothSampleRate::Init() {
  frequency_ = .2f;
  phase_ = 0.0f;
  sample_ = 0.0f;
  next_sample_ = 0.0f;
  previous_sample_ = 0.0f;
}

void AudioEffectSmoothSampleRate::update(void) {
  audio_block_t *blocka;
  float32_t blockFloat[AUDIO_BLOCK_SAMPLES];

  blocka = receiveWritable(0);
  if (!blocka) {
    return;
  }
  // convert block of samples to floats
  arm_q15_to_float(blocka->data, blockFloat, AUDIO_BLOCK_SAMPLES);
  // process a block of samples
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    blockFloat[i] = Process(blockFloat[i]);
  }
  // convert block back to ints
  arm_float_to_q15(blockFloat, blocka->data, AUDIO_BLOCK_SAMPLES);
  // transmitt block
  transmit(blocka);
  release(blocka);
}

float AudioEffectSmoothSampleRate::Process(float in) {
  float this_sample = next_sample_;
  next_sample_ = 0.f;
  phase_ += frequency_;
  if (phase_ >= 1.0f) {
    phase_ -= 1.0f;
    float t = phase_ / frequency_;
    // t = 0: the transition occurred right at this sample.
    // t = 1: the transition occurred at the previous sample.
    // Use linear interpolation to recover the fractional sample.
    float new_sample = previous_sample_ + (in - previous_sample_) * (1.0f - t);
    float discontinuity = new_sample - sample_;
    this_sample += discontinuity * ThisBlepSample(t);
    next_sample_ = discontinuity * NextBlepSample(t);
    sample_ = new_sample;
  }
  next_sample_ += sample_;
  previous_sample_ = in;
  return this_sample;
}

void AudioEffectSmoothSampleRate::SetFreq(float frequency) { frequency_ = fclamp(frequency, 0.001f, 1.f); }

// helpers
float AudioEffectSmoothSampleRate::ThisBlepSample(float t) { return 0.5f * t * t; }
float AudioEffectSmoothSampleRate::NextBlepSample(float t) {
  t = 1.0f - t;
  return -0.5f * t * t;
}
float AudioEffectSmoothSampleRate::fclamp(float in, float min, float max) { return fmin(fmax(in, min), max); }