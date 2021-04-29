//
// Test of a simple biquad filter using the CMSIS-DSP functions
// https://forum.pjrc.com/threads/67011-Help-with-filter-design
//
//
#include "filter_biquadfloat.h"
#include <Arduino.h>
#include "arm_math.h"

void AudioFilterBiquadFloat::initCoeff() {
  arm_biquad_cascade_df1_init_f32(&S1, NUMSTAGES, S1_coeffs, S1_pState);
}

void AudioFilterBiquadFloat::update(void) {
  audio_block_t *blocka;
  float32_t blockFloat[AUDIO_BLOCK_SAMPLES];

  blocka = receiveWritable(0);
  if (!blocka) {
    return;
  }
  // convert block of samples to floats
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    blockFloat[i] = blocka->data[i] / 32768.0f;
  }
  // process a block of samples
  arm_biquad_cascade_df1_f32(&S1, blockFloat, blockFloat, AUDIO_BLOCK_SAMPLES);

  // convert block back to ints
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    blocka->data[i] = (float)(blockFloat[i]) * 32768.0f;
  }
  transmit(blocka);
  release(blocka);
}
