/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
 */
inline float ThisBlepSample(float t) { return 0.5f * t * t; }

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
 */
inline float NextBlepSample(float t) {
  t = 1.0f - t;
  return -0.5f * t * t;
}

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
 */
inline float NextIntegratedBlepSample(float t) {
  const float t1 = 0.5f * t;
  const float t2 = t1 * t1;
  const float t4 = t2 * t2;
  return 0.1875f - t1 + 1.5f * t2 - t4;
}

/** Ported from pichenettes/eurorack/plaits/dsp/oscillator/oscillator.h
 */
inline float ThisIntegratedBlepSample(float t) { return NextIntegratedBlepSample(1.0f - t); }

/** Soft Limiting function ported extracted from pichenettes/stmlib */
inline float SoftLimit(float x) { return x * (27.f + x * x) / (27.f + 9.f * x * x); }

/** Soft Clipping function extracted from pichenettes/stmlib */
inline float SoftClip(float x) {
  if (x < -3.0f)
    return -1.0f;
  else if (x > 3.0f)
    return 1.0f;
  else
    return SoftLimit(x);
}

/** quick fp clamp
 */
inline float fclamp(float in, float min, float max) { return fmin(fmax(in, min), max); }

/** efficient floating point min/max
c/o stephen mccaul
*/
inline float fmax(float a, float b) {
  float r;
#ifdef __arm__
  asm("vmaxnm.f32 %[d], %[n], %[m]" : [d] "=t"(r) : [n] "t"(a), [m] "t"(b) :);
#else
  r = (a > b) ? a : b;
#endif  // __arm__
  return r;
}

inline float fmin(float a, float b) {
  float r;
#ifdef __arm__
  asm("vminnm.f32 %[d], %[n], %[m]" : [d] "=t"(r) : [n] "t"(a), [m] "t"(b) :);
#else
  r = (a < b) ? a : b;
#endif  // __arm__
  return r;
}