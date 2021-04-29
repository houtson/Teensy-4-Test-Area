#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
#include <filter_biquadfloat.h>

// GUItool: begin automatically generated code
AudioInputI2S i2s1;              // xy=198,215
AudioMixer4 mixer1;              // xy=404,218
AudioFilterBiquadFloat biquad1;  // xy=601,211
AudioOutputI2S i2s2;             // xy=795,216
AudioConnection patchCord1(i2s1, 0, mixer1, 0);
AudioConnection patchCord2(i2s1, 1, mixer1, 1);
AudioConnection patchCord3(mixer1, biquad1);
AudioConnection patchCord4(biquad1, 0, i2s2, 0);
AudioConnection patchCord5(biquad1, 0, i2s2, 1);
AudioControlSGTL5000 sgtl5000_1;  // xy=212,422
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;
double Coeff[4][5] = {{0.9721850412336036, -1.9443700824672072, 0.9721850412336036, 1.9435962752707163, -0.9451438896636983},
                      {0.05720022103530203, 0.11440044207060406, 0.05720022103530203, 1.2188761083637003, -0.4476769925049082},
                      {0.8132425994466649, 1.6264851988933298, 0.8132425994466649, -1.5912978843171357, -0.661672513469524},
                      {1.0, -2.0, 1.0, -1.81956193, 0.89601845}};
uint32_t upTime = millis();

void setup() {
  // set up serial for diagnostics
  Serial.begin(38400);
  // wait for serial
  while (!Serial && ((millis() - upTime) <= 5000))
    ;
  AudioMemory(200);
  sgtl5000_1.enable();  // Enable the audio shield
  //sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
  mixer1.gain(0, 0.5f);
  mixer1.gain(1, 0.5f);
  Serial.println("got here");
}

void loop() {
  biquad1.setCoeff(0, Coeff[0]);
  biquad1.setCoeff(1, Coeff[1]);
  biquad1.initCoeff();
  Serial.println("High Pass plus Low Pass >>");
  delay(5000);
  biquad1.setCoeff(0, Coeff[2]);
  biquad1.setCoeff(1, Coeff[2]);
  biquad1.initCoeff();
  Serial.println("Low Pass at 20000Hz");
  delay(5000);
}
/*
Coefficients
CMSIS formula
y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]

Calculator from https://www.earlevel.com/main/2003/02/28/biquads/
https://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/

y[n] = a0 * x[n] + a1 * x[n-1] + a2 * x[n-2] – b1 * y[n-1] – b2 * y[n-2]

  High Pass at 280hz
a0 = 0.9721850412336036
a1 = -1.9443700824672072
a2 = 0.9721850412336036
b1 = -1.9435962752707163
b2 = 0.9451438896636983
>>
b0 = 0.9721850412336036
b1 = -1.9443700824672072
b2 = 0.9721850412336036
a1 = 1.9435962752707163
a2 = -0.9451438896636983

0.9721850412336036f, -1.9443700824672072f, 0.9721850412336036f, 1.9435962752707163f, -0.9451438896636983f

Low Pass at 4000hZ

a0 = 0.05720022103530203
a1 = 0.11440044207060406
a2 = 0.05720022103530203
b1 = -1.2188761083637003
b2 = 0.4476769925049082
>>
b0 = 0.05720022103530203
b1 = 0.11440044207060406
b2 = 0.05720022103530203
a1 = 1.2188761083637003
a2 = -0.4476769925049082

0.05720022103530203f, 0.11440044207060406f, 0.05720022103530203f, 1.2188761083637003f, -0.4476769925049082

LowPass at 20kHz
a0 = 0.8132425994466649
a1 = 1.6264851988933298
a2 = 0.8132425994466649
b1 = 1.5912978843171357
b2 = 0.661672513469524

>>
0.8132425994466649, 1.6264851988933298, 0.8132425994466649, -1.5912978843171357, -0.661672513469524
*/