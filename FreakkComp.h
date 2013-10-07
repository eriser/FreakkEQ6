#ifndef __FREAKKCOMP__
#define __FREAKKCOMP__

#include "IPlug_include_in_plug_hdr.h"
#include "../FrkDSP/simpleSource/SimpleComp.h"

class FreakkComp : public IPlug
{
public:
  FreakkComp(IPlugInstanceInfo instanceInfo);
  ~FreakkComp();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double Peak(double* samples, int length);

private:
  double mGain;
  double mThreshold;
  double mRatio;
  double mAttack;
  double mRelease;
  double mKnee;
 
};

#endif
