//
// Test of a simple biquad filter using the CMSIS-DSP functions
// https://forum.pjrc.com/threads/67011-Help-with-filter-design
//
//

#ifndef filter_biquadfloat_h_
#define filter_biquadfloat_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

class AudioFilterBiquadFloat : public AudioStream {
 public:
  AudioFilterBiquadFloat() : AudioStream(1, inputQueueArray) { initCoeff(); };
  void initCoeff();
  virtual void update(void);
  void setCoeff(uint32_t stage, const double *coeffs) {
    if (stage >= (NUMSTAGES - 1)) stage = NUMSTAGES - 1;
    S1_coeffs[stage * 5] = coeffs[0];
    S1_coeffs[stage * 5 + 1] = coeffs[1];
    S1_coeffs[stage * 5 + 2] = coeffs[2];
    S1_coeffs[stage * 5 + 3] = coeffs[3];
    S1_coeffs[stage * 5 + 4] = coeffs[4];
  }

 private:
  static const int NUMSTAGES = 2;
  arm_biquad_casd_df1_inst_f32 S1;
  float32_t S1_pState[4 * NUMSTAGES] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  float32_t S1_coeffs[5 * NUMSTAGES] = {0.8132425994466649, 1.6264851988933298, 0.8132425994466649, -1.5912978843171357, -0.661672513469524,
                                        0.8132425994466649, 1.6264851988933298, 0.8132425994466649, -1.5912978843171357, -0.661672513469524};
  audio_block_t *inputQueueArray[1];
};

#endif
