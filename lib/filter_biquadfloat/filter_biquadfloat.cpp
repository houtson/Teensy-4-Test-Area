//
// Test of a simple biquad filter using the CMSIS-DSP functions
// https://forum.pjrc.com/threads/67011-Help-with-filter-design
//
//
#include "filter_biquadfloat.h"

#include <Arduino.h>

#include "arm_math.h"

void AudioFilterBiquadFloat::initCoeff() { arm_biquad_cascade_df1_init_f32(&S1, NUMSTAGES, S1_coeffs, S1_pState); }

void AudioFilterBiquadFloat::update(void) {
  audio_block_t *blocka;
  float32_t blockFloat[AUDIO_BLOCK_SAMPLES];

  blocka = receiveWritable(0);
  if (!blocka) {
    return;
  }
  // convert block of samples to floats
  arm_q15_to_float(blocka->data, blockFloat, AUDIO_BLOCK_SAMPLES);
  // process a block of samples
  arm_biquad_cascade_df1_f32(&S1, blockFloat, blockFloat, AUDIO_BLOCK_SAMPLES);
  // convert block back to ints
  arm_float_to_q15(blockFloat, blocka->data, AUDIO_BLOCK_SAMPLES);
  
  transmit(blocka);
  release(blocka);
}
