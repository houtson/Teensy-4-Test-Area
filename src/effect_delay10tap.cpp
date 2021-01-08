/* Audio Library for Teensy 4.x
 * Modified to extend to 10 taps PMF 16-03-2020
 * Modified for single samples delay line (rather than blocks) and tape delay like behaviour PMF 02-09-2020
 * added delayfade to fade between old and new delay time with expo cross fade PMF 04-09-2020
 * added delaysmooth to smoothly delay from old to new time PMF 14-10-2020
 */
#include "effect_delay10tap.h"

#include <Arduino.h>

void AudioEffectDelay10tap::begin(int16_t *delay_l, uint32_t max_delay_length) {
  delay_line = delay_l;
  max_delay_length_samples = max_delay_length - 1;
  write_index = 0;
}

// activate a tap and/or change time with a fade between old and new time (no clicks), transition time in millis
uint32_t AudioEffectDelay10tap::delayfade(uint8_t channel, float milliseconds, float transition_time) {
  if (channel >= DELAY_NUM_TAPS) return 0;
  if (milliseconds < 0.0) {
    milliseconds = 0.0;
  }
  if (transition_time < 0.0) transition_time = 0.0;

  uint32_t delay_length_samples = millisToSamples(milliseconds);
  if (delay_length_samples > max_delay_length_samples) delay_length_samples = max_delay_length_samples;

  __disable_irq();
  // enable disabled channel
  if (!(activemask & (1 << channel))) {
    // if channel not active then activate and delay as normal, no fade
    tap[channel].fade_to_delay_samples = tap[channel].current_delay = tap[channel].desired_delay = delay_length_samples;
    tap[channel].fade_samples_to_complete_transition = 0;
    activemask |= (1 << channel);
    tap[channel].delay_mode = DELAY_MODE_NORMAL;
  } else {
    // if already active check if currently fading
    if (tap[channel].fade_samples_to_complete_transition == 0) {
      // not currently fading, set up for a expo fade over transition time millis
      tap[channel].fade_to_delay_samples = tap[channel].desired_delay = delay_length_samples;                 // where in the delay are we fading to
      tap[channel].fade_transition_time = transition_time;                                                    // fade over xx milis
      tap[channel].fade_samples_to_complete_transition = millisToSamples(tap[channel].fade_transition_time);  // counter to fade over xx samples
      double tau = -1.0 * transition_time / log(1.0 - 1.0 / 1.01);  // calculate a multiplier for a exponential fade (with 1.01 overshoot)
      tap[channel].fade_expo_multiplier = pow(exp(-1.0 / tau), 1.0 / (AUDIO_SAMPLE_RATE_EXACT / 1000.0));
      tap[channel].fade_multiplier_out = 1.0;  // initialise the multiplier to be applied as gain on outgoing tap
      tap[channel].fade_multiplier_in = 0.01;  // initialise the multiplier to be applied as gain on incoming tap
      tap[channel].delay_mode = DELAY_MODE_FADE;
    } else {
      // currently fading, want to let that play out then fade again to desired so set desired as new target
      tap[channel].desired_delay = delay_length_samples;
    }
  }
  __enable_irq();
  return tap[channel].current_delay;
}

// activate a tap and/or change time without fading but smoothly changing delaytime
uint32_t AudioEffectDelay10tap::delaysmooth(uint8_t channel, float milliseconds) {
  long temp;
  if (channel >= DELAY_NUM_TAPS) return 0;
  if (milliseconds < 0.0) milliseconds = 0.0;

  uint32_t delay_length_samples = millisToSamples(milliseconds);
  if (delay_length_samples > max_delay_length_samples) delay_length_samples = max_delay_length_samples;

  __disable_irq();
  // enable disabled channel
  if (!(activemask & (1 << channel))) {
    // if not previously activie just move straight to it
    tap[channel].delay_mode = DELAY_MODE_NORMAL;
    tap[channel].current_delay = tap[channel].desired_delay = delay_length_samples;
    tap[channel].fade_samples_to_complete_transition = 0;
    activemask |= (1 << channel);
  } else {
    // if already active...set desired and current delay
    tap[channel].desired_delay = delay_length_samples;
    tap[channel].current_delay = tap[channel].current_delay + (static_cast<int32_t>(tap[channel].inc) * tap[channel].inc_direction);
    tap[channel].fade_samples_to_complete_transition = 0;
    // if desire and current different set a rate to increment to desired
    if (tap[channel].current_delay != tap[channel].desired_delay) {
      // set direction to increment in, initialise and set the rate from 1 semiton to 1 octave based on size of change required
      if (tap[channel].current_delay > tap[channel].desired_delay) tap[channel].inc_direction = -1;
      if (tap[channel].current_delay < tap[channel].desired_delay) tap[channel].inc_direction = 1;
      tap[channel].inc = 0.0;
      tap[channel].last_sample = 0;
      temp = map(abs(tap[channel].current_delay - tap[channel].desired_delay), 0, 4000, 1, 12);
      tap[channel].inc_per_sample = delay_inc_per_semitone[constrain(temp, 0, 12)];
      tap[channel].delay_mode = DELAY_MODE_SMOOTH;
    } else {
      // desired and current are equal so normal delay and no change
      tap[channel].delay_mode = DELAY_MODE_NORMAL;
      tap[channel].current_delay = tap[channel].desired_delay;
    }
  }
  __enable_irq();
  return tap[channel].current_delay;
}

void AudioEffectDelay10tap::disable(uint8_t channel) {
  if (channel >= DELAY_NUM_TAPS) return;
  // disable this channel
  activemask &= ~(1 << channel);
};

void AudioEffectDelay10tap::update(void) {
  audio_block_t *input, *output;
  int16_t *input_data_pointer, *output_data_pointer;
  uint32_t read_index, start_index;
  uint32_t fade_to_read_index = 0;
  uint8_t channel;

  int16_t next_sample = 0;
  int16_t sample = 0;

  int32_t inc_samples;
  float inc_frac;

  if (delay_line == NULL) return;
  // reading and wriitng the block separately so grab a copy of the write_index starting poisition
  start_index = write_index;

  // write incoming block of samples to buffer if not freezing buffer
  input = receiveReadOnly();
  if (input) {
    if (!freezeBuffer) {
      input_data_pointer = input->data;
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        delay_line[write_index++] = *input_data_pointer++;
        if (write_index >= max_delay_length_samples) write_index = 0;
      }
    } else {
      // if buffer frozen move write index on a block
      write_index += AUDIO_BLOCK_SAMPLES;
      if (write_index >= max_delay_length_samples) write_index = (write_index + max_delay_length_samples) % max_delay_length_samples;
    }
    release(input);
  }

  // delay
  // process each tap and write out delayed samples
  for (channel = 0; channel < DELAY_NUM_TAPS; channel++) {
    // check if channel is active
    if (!(activemask & (1 << channel))) continue;
    output = allocate();
    if (!output) continue;
    output_data_pointer = output->data;

    // if fading between current delay to desired , position desired read head
    if (tap[channel].fade_samples_to_complete_transition > 0)
      fade_to_read_index = ((start_index - tap[channel].fade_to_delay_samples + max_delay_length_samples) % max_delay_length_samples);

    // position the main read head (current_delay_) for this channel / tap
    read_index = ((start_index - tap[channel].current_delay + max_delay_length_samples) % max_delay_length_samples);

    // process each sample in the audio block
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      //
      // if fading, cross mix in fade_length_samples steps
      //
      if (tap[channel].delay_mode == DELAY_MODE_FADE) {
        // update the fade multiplier for expo fade curves
        tap[channel].fade_multiplier_out *= tap[channel].fade_expo_multiplier;
        tap[channel].fade_multiplier_in /= tap[channel].fade_expo_multiplier;
        // read the two points from the delay line, crossfade and send to the output block
        *output_data_pointer++ =
            (int16_t)(delay_line[read_index] * tap[channel].fade_multiplier_out) + (int16_t)(delay_line[fade_to_read_index] * tap[channel].fade_multiplier_in);
        tap[channel].fade_samples_to_complete_transition--;
        if (tap[channel].fade_samples_to_complete_transition == 0) {  // got to end of fade
          // make the current_delay the fade_to_
          tap[channel].current_delay = tap[channel].fade_to_delay_samples;
          read_index = fade_to_read_index;
          tap[channel].delay_mode = DELAY_MODE_NORMAL;
          // if stil not at desired delay then start another fade to it
          if (tap[channel].desired_delay != tap[channel].current_delay) {
            tap[channel].fade_to_delay_samples = tap[channel].desired_delay;
            tap[channel].fade_samples_to_complete_transition = millisToSamples(tap[channel].fade_transition_time);  // counter to fade over xx samples
            fade_to_read_index = ((start_index - tap[channel].fade_to_delay_samples + max_delay_length_samples) % max_delay_length_samples);
            // re-set fade multipliers
            tap[channel].fade_multiplier_out = 1.0;  // initialise the multiplier to be applied as gain on outgoing tap
            tap[channel].fade_multiplier_in = 0.01;  // initialise the multiplier to be applied as gain on incoming tap
            tap[channel].delay_mode = DELAY_MODE_FADE;
          }
        }
        // increment and wrap around the fade_to_read_index
        fade_to_read_index++;
        if (fade_to_read_index >= max_delay_length_samples) fade_to_read_index = 0;
      }
      //
      // smooth transition from one delay time to another
      //
      if (tap[channel].delay_mode == DELAY_MODE_SMOOTH) {
        // move the delay time by a small increment (inc), split inc into numbers of sample + frac apply direction +-
        tap[channel].inc += tap[channel].inc_per_sample;
        inc_samples = static_cast<int32_t>(tap[channel].inc);
        inc_frac = tap[channel].inc - static_cast<float>(inc_samples);
        inc_samples *= tap[channel].inc_direction;
        // check if reached desired delay
        if (tap[channel].current_delay + inc_samples == tap[channel].desired_delay) {
          // reached desire delay, re-set current and re-position index and change to normal delay
          tap[channel].current_delay = tap[channel].desired_delay;
          tap[channel].inc = 0.0;
          read_index = ((start_index + i - tap[channel].current_delay + max_delay_length_samples) % max_delay_length_samples);
          tap[channel].delay_mode = DELAY_MODE_NORMAL;
        } else {
          // not reached desired yet so get next sample and interpolate
          sample = delay_line[(read_index - inc_samples + max_delay_length_samples) % max_delay_length_samples];
          next_sample = delay_line[(read_index - inc_samples - tap[channel].inc_direction + max_delay_length_samples) % max_delay_length_samples];
          if (tap[channel].last_sample == 0) tap[channel].last_sample = sample; // if just starting help the allpass tune in quickly
          *output_data_pointer++ = tap[channel].last_sample = allPassInterpolSamples(sample, next_sample, tap[channel].last_sample, inc_frac);
        }
      }
      //
      // normal delay
      //
      if (tap[channel].delay_mode == DELAY_MODE_NORMAL) {
        // read delay line and send sample to the output block
        *output_data_pointer++ = delay_line[read_index];
      }

      // increment and wrap around the  read index
      read_index++;
      if (read_index >= max_delay_length_samples) read_index = 0;
    }
    transmit(output, channel);
    release(output);
  }
  // if (dump_samples) dump_samples = false;
}

// set the increment for the smooth delay
void AudioEffectDelay10tap::setDelayIncPerSample(uint8_t channel, float _DELAYINC) {
  tap[channel].inc_per_sample = _DELAYINC;
  if (tap[channel].inc_per_sample < 0.1) tap[channel].inc_per_sample = 0.1;
  if (tap[channel].inc_per_sample > 1.0) tap[channel].inc_per_sample = 1.0;
};
