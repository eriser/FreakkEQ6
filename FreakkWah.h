#ifndef __FREAKKWAH__
#define __FREAKKWAH__

#include "IPlug_include_in_plug_hdr.h"
#include "../FrkDSP/simpleSource/SimpleComp.h"
#include "SVF.h"

#define LOW 0
#define HIGH 1
#define BAND 2
#define NOTCH 3

class FreakkWah : public IPlug
{
public:
  FreakkWah(IPlugInstanceInfo instanceInfo);
  ~FreakkWah();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double Peak(double* samples, int length);
  void SetFreq();

private:
  SVF *svf;
  double mGain;
  double mThreshold;
  double mQ;
  double mFreq, mFreqMin, mFreqMax;
  int mType;
  double sampleRate;
  //smooth
  int status;
  int attackSamples;
  int attackCounter;
  int releaseSamples;
  int releaseCounter;
  double attackFactor;
  double releaseFactor;

 
};

#endif
