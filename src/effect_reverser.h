#ifndef effect_reverser_h_
#define effect_reverser_h_
#include "Arduino.h"
#include "AudioStream.h"

class AudioEffectReverser : public AudioStream {
 public:
  AudioEffectReverser(void) : AudioStream(1, inputQueueArray) {}
  
  // initialise the delay line
  void begin(int16_t *delay_line, uint32_t max_delay_length);

  // main update routine
  virtual void update(void);

  void inspect(void) { dump_samples = true; };

 private:
  audio_block_t *inputQueueArray[1];
  uint32_t max_delay_length_samples, half_delay_length_samples;  // lenght of the delay line in samples
  uint32_t write_index;               // write head position
  uint32_t read_index;      // read_index
  int16_t *sample_delay_line;  // pointer to delay line

  boolean buffer_filled = false;

  boolean dump_samples = false;
};
#endif