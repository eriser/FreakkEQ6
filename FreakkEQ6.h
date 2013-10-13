#ifndef __FREAKKEQ6__
#define __FREAKKEQ6__

#include "IPlug_include_in_plug_hdr.h"
#include "Biquad.h"

class FreakkEQ6 : public IPlug
{
public:
  FreakkEQ6(IPlugInstanceInfo instanceInfo);
  ~FreakkEQ6();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void SetGain(int n);

private:
  
  Biquad ** FilterL;
  Biquad ** FilterR;
  double *mBandGain;
  double *mQ;
  double *mFreq;
  
  double mGain; 
  double mMix;
  
  double sampleRate;
 
};

#endif
