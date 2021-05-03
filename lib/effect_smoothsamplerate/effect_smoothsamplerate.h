// Copyright 2014 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Sample rate reducer.
// Ported from
// - pichenettes/eurorack/plaits/dsp/fx/sample_rate_reducer.h
// - Original code written by Emilie Gillet
// to
// - DaisySP by Ben Sergentanis Jan 2021
// then
// - Teensy 4.x by Paul Feely Apr 2021
#ifndef effect_smoothsamplerate_h_
#define effect_smoothsamplerate_h_
#include <stdint.h>

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

class AudioEffectSmoothSampleRate : public AudioStream {
 public:
  AudioEffectSmoothSampleRate() : AudioStream(1, inputQueueArray){};
  void Init();
  virtual void update(void);
  // process a single sample
  float Process(float in);
  // Set the new sample rate, works over 0-1. 1 is full quality, .5 is half sample rate, etc.
  void SetFreq(float frequency);

 private:
  // helpers from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
  inline float ThisBlepSample(float t);
  inline float NextBlepSample(float t);
  inline float fclamp(float in, float min, float max);

  float frequency_;
  float phase_;
  float sample_;
  float previous_sample_;
  float next_sample_;
  audio_block_t *inputQueueArray[1];
};
#endif