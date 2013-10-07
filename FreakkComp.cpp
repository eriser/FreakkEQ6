#include "FreakkComp.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <math.h>

#define OFFSET 0.01

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kThreshold,
  kRatio,
  kAttack,
  kRelease,
  kKnee,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 100,
  kGainY = 100,
  kKnobFrames = 60
};

double max( double a, double b) {
	if (a>=b) return a;
	else return b;
}

FreakkComp::FreakkComp(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 0, 0., 400.0, 1, "%");
  GetParam(kThreshold)->InitDouble("Threshold", 50., 0., 100., 1., "");
  GetParam(kRatio)->InitDouble("Ratio", 1., 1., 14., 0.1, ":1");
  GetParam(kAttack)->InitDouble("Attack", 50., 0., 100.0, 0.01, "ms");
  GetParam(kRelease)->InitDouble("Release", 100., 0., 200.0, 0.01, "ms");
  GetParam(kKnee)->InitDouble("Knee", 0, 0.45, 2.5, 0.01, "dB"); 
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

FreakkComp::~FreakkComp() {}

void FreakkComp::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  // Peak sensing log(in)
  // cv = slope * max( log |in| - log |mThreshold|, 0 ) // max function acts as half-wave rectifier

  double slope = 1/mRatio -1;
  double logThreshold = log10 ( abs(mThreshold)  + OFFSET) ;

  double peak1 = Peak(in1, nFrames);
  double peak2 = Peak(in2, nFrames);
  
  double cv1 = slope * max( log10 ( abs(peak1) + OFFSET ) - logThreshold, 0 ); // offset prevents log(0)
  double cv2 = slope * max( log10 ( abs(peak2) + OFFSET ) - logThreshold, 0 ); // offset prevents log(0)
 
  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = *in1 * pow(10,cv1) * mGain;
    *out2 = *in2 * pow(10,cv2) * mGain;
  }

}

void FreakkComp::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void FreakkComp::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = 2 * GetParam(kGain)->Value() / 100.;
      break;
	case kThreshold:
      mThreshold = GetParam(kThreshold)->Value() / 100. + OFFSET;
      break;
	case kRatio:
      mRatio = GetParam(kRatio)->Value();
      break;
	case kAttack:
      mAttack = GetParam(kAttack)->Value();
      break;
	case kRelease:
      mRelease = GetParam(kRelease)->Value();
      break;
	case kKnee:
      mKnee = GetParam(kKnee)->Value();
      break;

    default:
      break;
  }


}

double FreakkComp::Peak(double* samples, int length)
{
	double pk = abs(samples[0]);
	for(int i=1; i < length ; i++) 
			if ( abs(samples[i]) > pk) pk = abs(samples[i]);
	return pk;
}
