#include "FreakkGate.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <math.h>

#define OFFSET 0.01
#define BYPASS 0
#define MUTE 1
#define ATTACK 2
#define RELEASE 3


const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kThreshold,
  kAttack,
  kRelease,
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

double rms(double *v, int size)
{
  int i;
  double sum = 0.0;
  for(i = 0; i < size; i++)
    sum += v[i] * v[i];
  return sqrt(sum / size);
}

FreakkGate::FreakkGate(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;
  status = BYPASS;
  attackSamples = 0;
  releaseSamples = 0;
  attackFactor = 1;
  releaseFactor = 1;
  sampleRate = GetSampleRate();
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 100., 0., 400.0, 1, "%");
  GetParam(kThreshold)->InitDouble("Threshold", 50., 0., 100., 1., "");
  GetParam(kAttack)->InitDouble("Attack", 50., 0., 1000.0, 1, "ms");
  GetParam(kRelease)->InitDouble("Release", 100., 0., 100.0, 1, "ms");
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  SetAttack();
  SetRelease();
  attackCounter = attackSamples;
  releaseCounter = releaseSamples;
}

FreakkGate::~FreakkGate() {}

void FreakkGate::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double logThreshold = log10 ( abs(mThreshold) ) ;

  /*
  double peak1 = Peak(inputs[0], nFrames);
  double peak2 = Peak(inputs[1], nFrames);
  double peak = (peak1 >= peak2) ? peak1 : peak2 ;
  */
  double rms1 = rms(inputs[0], nFrames);
  double rms2 = rms(inputs[1], nFrames);
  double rmsFrame;

  if ( rms1 > rms2) rmsFrame = rms1;
  else rmsFrame = rms2;

  if( rmsFrame < mThreshold ){
	if(status == BYPASS){
		status = ATTACK;
		SetAttack();
	}
	 for (int s = 0; s < nFrames; ++s) {
			 if(status == ATTACK) Attack();
			 outputs[0][s] = inputs[0][s] * attackFactor;
			 outputs[1][s] = inputs[1][s] * attackFactor;
	 }
  }
  else {
	  status = BYPASS;
    for (int s = 0; s < nFrames; ++s) {
		outputs[0][s] = inputs[0][s] * mGain;
		outputs[1][s] = inputs[1][s] * mGain;
	}
  }

}

void FreakkGate::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void FreakkGate::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;
	case kThreshold:
      mThreshold = GetParam(kThreshold)->Value() / 100.;
      break;
	case kAttack:
      mAttack = GetParam(kAttack)->Value();
	  SetAttack();
      break;
	case kRelease:
      mRelease = GetParam(kRelease)->Value();
	  SetRelease();
      break;

    default:
      break;
  }

}

double FreakkGate::Peak(double* samples, int length)
{
	double pk = abs(samples[0]);
	for(int i=1; i < length ; i++) 
			if ( abs(samples[i]) > pk) pk = abs(samples[i]);
	return pk;
}

void FreakkGate::Attack()
{
	if(attackCounter < 0){
		attackFactor = 0;
		status = MUTE;
		return ;
	}
	attackFactor = ((double)attackCounter) / attackSamples ;
	--attackCounter;
}

void FreakkGate::Release()
{
	return ;
}

void FreakkGate::SetAttack()
{
	attackSamples = (int) (mAttack/1000. * sampleRate);
	attackCounter = attackSamples;
	return;
}

void FreakkGate::SetRelease()
{
	releaseSamples = (int) (mRelease * sampleRate / 1000.);
	releaseCounter = releaseSamples;
	return;
}