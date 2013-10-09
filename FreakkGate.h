#ifndef __FREAKKGATE__
#define __FREAKKGATE__

#include "IPlug_include_in_plug_hdr.h"
#include "../FrkDSP/simpleSource/SimpleComp.h"

class FreakkGate : public IPlug
{
public:
  FreakkGate(IPlugInstanceInfo instanceInfo);
  ~FreakkGate();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double Peak(double* samples, int length);

private:
  double mGain;
  double mThreshold;
  double mAttack;
  double mRelease;
  double sampleRate;
  //smooth
  int status;
  int attackSamples;
  int attackCounter;
  int releaseSamples;
  int releaseCounter;
  double attackFactor;
  double releaseFactor;
  void Attack();
  void Release();

  void SetAttack();
  void SetRelease();
  //FILE * fp;
 
};

#endif
