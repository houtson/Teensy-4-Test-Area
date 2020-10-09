#include <Arduino.h>

#include "effect_reverser.h"

void AudioEffectReverser::begin(int16_t *delay_line, uint32_t max_delay_length) {
  sample_delay_line = delay_line;
  max_delay_length_samples = max_delay_length - 1;
  read_index = half_delay_length_samples = max_delay_length_samples / 2;
  write_index = 0;
}

void AudioEffectReverser::update(void) {
  audio_block_t *block;
  int16_t *block_pointer;

// check if have a delay line
  if (sample_delay_line == NULL) return;

  // grab the block of samples as writeable
  block = receiveWritable();
  if (block) {
    block_pointer = block->data;

    // process each smaple in the block
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      // write the sample to a buffer
      sample_delay_line[write_index++] = *block_pointer;

      // if enought in the buffer start reversing
      if (write_index >= half_delay_length_samples) buffer_filled = true;
      if (buffer_filled) *block_pointer = sample_delay_line[read_index--];

      // wrap round the write index
      if (write_index >= max_delay_length_samples) write_index = 0;

      // wrap round the read index
      if (read_index == 0) read_index = max_delay_length_samples;

      // move the block_pointer on
      block_pointer++;
    }
    // transmit the block out on channel 0
    transmit(block, 0);
    release(block);
  }
}
