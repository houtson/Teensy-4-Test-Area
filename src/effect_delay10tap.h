/* Audio Library for Teensy 3.X
 *
 * Modified to extend to 10 taps PMF 16-03-2020
 * Modified for single samples delay line (rather than blocks) and tape delay like behaviour PMF 02-09-2020
 * added delayfade to fade between old and new delay time PMF 04-09-2020
 */

#ifndef effect_delay10tap_h_
#define effect_delay10tap_h_
#include "Arduino.h"
#include "AudioStream.h"

#define DELAY_NUM_TAPS 10                       // max numer of fixed delay taps / channels
#define DELAY_INC 0.19                          // delaysmooth, default increment per samples for delaysmooth

class AudioEffectDelay10tap : public AudioStream {
 public:
  AudioEffectDelay10tap(void) : AudioStream(1, inputQueueArray) {}
  // initialise the delay line
  void begin(int16_t *delay_line, uint32_t max_delay_length);
  // activate a tap and/or change time with a fade between old and new time (no clicks), transition time in millis
  uint32_t delayfade(uint8_t channel, float milliseconds, float transition_time);
  uint32_t delaysmooth(uint8_t channel, float milliseconds);
  void setDelayIncPerSample(uint8_t channel, float _DELAYINC);
  // disable a tap
  void disable(uint8_t channel);
  // main update routine
  virtual void update(void);

  void setBufferFreeze(bool _FREEZE) { freezeBuffer = _FREEZE; };
  bool returnBufferFreeze() { return freezeBuffer; };

  void inspect(void) { dump_samples = true; };

 private:
  // linear interpolation between two samples (frac = 0 - 1)
  int16_t lerpSamples(int16_t sample1, int16_t sample2, float frac) {
    return static_cast<int16_t>(static_cast<float>(sample1) + static_cast<float>(sample2 - sample1) * frac);
  };
  // all pass interpol sample 1= current sample, 2= next sample, 3= last sample
  int16_t allPassInterpolSamples(int16_t sample1, int16_t sample2, int16_t sample3, float frac) {
    return int16_t(sample2 + (1 - frac) * sample1 - (1 - frac) * sample3);
  };
  // convert milliseconds to number of samples at sample rate
  int32_t millisToSamples(float milliseconds) { return milliseconds * (AUDIO_SAMPLE_RATE_EXACT / 1000.0) + 0.5; };

  audio_block_t *inputQueueArray[1];
  uint32_t max_delay_length_samples;  // lenght of the delay line in samples
  uint32_t write_index;               // write head position
  uint16_t activemask;                // which taps/channels are active
  int16_t *delay_line;                // pointer to delay line

  // delay modes
  enum delay_modes { DELAY_MODE_NORMAL, DELAY_MODE_SMOOTH, DELAY_MODE_FADE };

  // struct for tap
  typedef struct {
    int32_t current_delay;  // actual # of sample delay for each channel
    int32_t desired_delay;  // desired # of sample delay for each channel
    uint32_t fade_to_delay_samples;
    uint32_t fade_transition_time;
    uint32_t fade_samples_to_complete_transition;
    int16_t last_sample;
    double fade_multiplier_out;
    double fade_multiplier_in;
    double fade_expo_multiplier;
    int16_t inc_direction = 1;   // direction (+/-) to increment if delaysmooth
    float inc = 0.0;             // cummulative increment if delaysmooth
    float inc_per_sample = 0.0;  // increment per sample if delaysmooth
    int16_t delay_mode = DELAY_MODE_NORMAL;
  } tap_struct;
  tap_struct tap[DELAY_NUM_TAPS];
  
  // smooth delay increments for changing delay times in semitones
  float delay_inc_per_semitone[13] = {0.0000000, 0.0594631, 0.1224620, 0.1892071, 0.2599210, 0.3348399, 0.4142136,
                                      0.4983071, 0.5874011, 0.6817928, 0.7817974, 0.8877486, 1.0000000};
  
  
  uint32_t temp_timer = 0;
  boolean freezeBuffer = false;
  boolean dump_samples = false;
};
#endif