// Copyright (c) 2003, David Lowenfels
// Released as the moog~ pd extern.

// This code is Unlicensed (i.e. public domain); in an email exchange on
// 6.14.2019 David Lowenfels stated "You're welcome to use the Moog~ code, 
// license it free as in beer or whatever :)"

#pragma once

#ifndef STILSON_LADDER_H
#define STILSON_LADDER_H

#include "LadderFilterBase.h"

/*
A digital model of the classic Moog filter was presented first by Stilson and
Smith. This model uses a cascade of one-pole IIR filters in series with a global
feedback to produce resonance. A digital realization of this filter introduces a
unit delay, effectively making it a fifth-order filter. Unfortunately, this
delay also has the effect of coupling the cutoff and resonance parameters,
uncharacteristic of the uncoupled control of the original Moog ladder. As a
compromise, a zero can be inserted at z = -0.3 inside each one pole section to
minimize the coupling the parameters (humans are not particularly sensitive to
variations in Q factor). Although fast coefficient updates can be achieved since
the nonlinearities of the Moog are not considered, the filter becomes unstable
with very large resonance values and does not enter self-oscillation.

References: Stilson and Smith (1996), DAFX - Zolzer (ed) (2nd ed)
Original implementation: Tim Stilson, David Lowenfels
*/

static float S_STILSON_GAINTABLE[199] =
{
/*
  0.999969, 0.990082, 0.980347, 0.970764, 0.961304, 0.951996, 0.94281, 0.933777, 0.924866, 0.916077,
  0.90741, 0.898865, 0.890442, 0.882141 , 0.873962, 0.865906, 0.857941, 0.850067, 0.842346, 0.834686,
  0.827148, 0.819733, 0.812378, 0.805145, 0.798004, 0.790955, 0.783997, 0.77713, 0.770355, 0.763672,
  0.75708 , 0.75058, 0.744141, 0.737793, 0.731537, 0.725342, 0.719238, 0.713196, 0.707245, 0.701355,
  0.695557, 0.689819, 0.684174, 0.678558, 0.673035, 0.667572, 0.66217, 0.65686, 0.651581, 0.646393,
  0.641235, 0.636169, 0.631134, 0.62619, 0.621277, 0.616425, 0.611633, 0.606903, 0.602234, 0.597626,
  0.593048, 0.588531, 0.584045, 0.579651, 0.575287 , 0.570953, 0.566681, 0.562469, 0.558289, 0.554169,
  0.550079, 0.546051, 0.542053, 0.538116, 0.53421, 0.530334, 0.52652, 0.522736, 0.518982, 0.515289,
  0.511627, 0.507996 , 0.504425, 0.500885, 0.497375, 0.493896, 0.490448, 0.487061, 0.483704, 0.480377,
  0.477081, 0.473816, 0.470581, 0.467377, 0.464203, 0.46109, 0.457977, 0.454926, 0.451874, 0.448883,
  0.445892, 0.442932, 0.440033, 0.437134, 0.434265, 0.431427, 0.428619, 0.425842, 0.423096, 0.42038,
  0.417664, 0.415009, 0.412354, 0.409729, 0.407135, 0.404572, 0.402008, 0.399506, 0.397003, 0.394501,
  0.392059, 0.389618, 0.387207, 0.384827, 0.382477, 0.380127, 0.377808, 0.375488, 0.37323, 0.370972,
  0.368713, 0.366516, 0.364319, 0.362122, 0.359985, 0.357849, 0.355713, 0.353607, 0.351532, 0.349457,
  0.347412, 0.345398, 0.343384, 0.34137, 0.339417, 0.337463, 0.33551, 0.333588, 0.331665, 0.329773,
  0.327911, 0.32605, 0.324188, 0.322357, 0.320557, 0.318756, 0.316986, 0.315216, 0.313446, 0.311707,
  0.309998, 0.308289, 0.30658, 0.304901, 0.303223, 0.301575, 0.299927, 0.298309, 0.296692, 0.295074,
  0.293488, 0.291931, 0.290375, 0.288818, 0.287262, 0.285736, 0.284241, 0.282715, 0.28125, 0.279755,
  0.27829, 0.276825, 0.275391, 0.273956, 0.272552, 0.271118, 0.269745, 0.268341, 0.266968, 0.265594,
  0.264252, 0.262909, 0.261566, 0.260223, 0.258911, 0.257599, 0.256317, 0.255035, 0.25375
*/
  0.999969f, 0.990082f, 0.980347f, 0.970764f, 0.961304f, 0.951996f, 0.94281f, 0.933777f, 0.924866f, 0.916077f,
  0.90741f, 0.898865f, 0.890442f, 0.882141f, 0.873962f, 0.865906f, 0.857941f, 0.850067f, 0.842346f, 0.834686f,
  0.827148f, 0.819733f, 0.812378f, 0.805145f, 0.798004f, 0.790955f, 0.783997f, 0.77713f, 0.770355f, 0.763672f,
  0.75708f, 0.75058f, 0.744141f, 0.737793f, 0.731537f, 0.725342f, 0.719238f, 0.713196f, 0.707245f, 0.701355f,
  0.695557f, 0.689819f, 0.684174f, 0.678558f, 0.673035f, 0.667572f, 0.66217f, 0.65686f, 0.651581f, 0.646393f,
  0.641235f, 0.636169f, 0.631134f, 0.62619f, 0.621277f, 0.616425f, 0.611633f, 0.606903f, 0.602234f, 0.597626f,
  0.593048f, 0.588531f, 0.584045f, 0.579651f, 0.575287f, 0.570953f, 0.566681f, 0.562469f, 0.558289f, 0.554169f,
  0.550079f, 0.546051f, 0.542053f, 0.538116f, 0.53421f, 0.530334f, 0.52652f, 0.522736f, 0.518982f, 0.515289f,
  0.511627f, 0.507996f, 0.504425f, 0.500885f, 0.497375f, 0.493896f, 0.490448f, 0.487061f, 0.483704f, 0.480377f,
  0.477081f, 0.473816f, 0.470581f, 0.467377f, 0.464203f, 0.46109f, 0.457977f, 0.454926f, 0.451874f, 0.448883f,
  0.445892f, 0.442932f, 0.440033f, 0.437134f, 0.434265f, 0.431427f, 0.428619f, 0.425842f, 0.423096f, 0.42038f,
  0.417664f, 0.415009f, 0.412354f, 0.409729f, 0.407135f, 0.404572f, 0.402008f, 0.399506f, 0.397003f, 0.394501f,
  0.392059f, 0.389618f, 0.387207f, 0.384827f, 0.382477f, 0.380127f, 0.377808f, 0.375488f, 0.37323f, 0.370972f,
  0.368713f, 0.366516f, 0.364319f, 0.362122f, 0.359985f, 0.357849f, 0.355713f, 0.353607f, 0.351532f, 0.349457f,
  0.347412f, 0.345398f, 0.343384f, 0.34137f, 0.339417f, 0.337463f, 0.33551f, 0.333588f, 0.331665f, 0.329773f,
  0.327911f, 0.32605f, 0.324188f, 0.322357f, 0.320557f, 0.318756f, 0.316986f, 0.315216f, 0.313446f, 0.311707f,
  0.309998f, 0.308289f, 0.30658f, 0.304901f, 0.303223f, 0.301575f, 0.299927f, 0.298309f, 0.296692f, 0.295074f,
  0.293488f, 0.291931f, 0.290375f, 0.288818f, 0.287262f, 0.285736f, 0.284241f, 0.282715f, 0.28125f, 0.279755f,
  0.27829f, 0.276825f, 0.275391f, 0.273956f, 0.272552f, 0.271118f, 0.269745f, 0.268341f, 0.266968f, 0.265594f,
  0.264252f, 0.262909f, 0.261566f, 0.260223f, 0.258911f, 0.257599f, 0.256317f, 0.255035f, 0.25375f
};

class StilsonMoog : public LadderFilterBase
{
public:
  
  StilsonMoog(float sampleRate) : LadderFilterBase(sampleRate)
  {
    memset(state, 0, sizeof(state));
    SetCutoff(1000.0f);
    SetResonance(0.10f);
  }
  
  virtual ~StilsonMoog()
  {
    
  }
  
  virtual void Process(float * samples, uint32_t n) override
  {
    float localState;

    for (uint32_t s = 0; s < n; ++s) {
      // Scale by arbitrary value on account of our saturation function
      const float input = samples[s] * 0.65f;
      
      // Negative Feedback
      output = 0.25 * (input - output);
      
      for (int pole = 0; pole < 4; ++pole)
      {
        localState = state[pole];
        output = moog_saturate(output + p * (output - localState));
        state[pole] = output;
        output = moog_saturate(output + localState);
      }
      
      SNAP_TO_ZERO(output);
      samples[s] = output;
      output *= Q; // Scale stateful output by Q
    }
  }
  
  virtual void SetResonance(float r) override
  {
    r = moog_min(r, 1);
    resonance = r;
    
    double ix;
    double ixfrac;
    int ixint;
    
    ix = p * 99;
    ixint = floor(ix);
    ixfrac = ix - ixint;
    
    Q = r * moog_lerp(ixfrac, S_STILSON_GAINTABLE[ixint + 99], S_STILSON_GAINTABLE[ixint + 100]);
  }
  
  virtual void SetCutoff(float c) override
  {
    cutoff = c;
    
    // Normalized cutoff between [0, 1]
    double fc = (cutoff) / sampleRate;
    double x2 = fc * fc;
    double x3 = fc * fc * fc;
    
    // Frequency & amplitude correction (Cubic Fit)
    p = -0.69346 * x3 - 0.59515 * x2 + 3.2937 * fc - 1.0072;
    
    SetResonance(resonance);
  }
  
private:
  
  double p;
  double Q; 
  double state[4];
  double output; 
}; 

#endif
