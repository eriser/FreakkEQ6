#include "FreakkWah.h"
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
  kBoost = 0,
  kQ,
  kFreq,
  kFreqMin,
  kFreqMax,
  kGain,
  kMix,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 83,
  kGainY = 27,

  kQX = 16,
  kQY = 91,

  kFreqX = 154,
  kFreqY = 91
  , 
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

FreakkWah::FreakkWah(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;
  status = BYPASS;
  attackSamples = 0;
  releaseSamples = 0;
  attackFactor = 1;
  releaseFactor = 1;
  sampleRate = GetSampleRate();
  Filter = new Biquad();

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kBoost)->InitDouble("Boost", 100, 0., 200, 1.,"");
  GetParam(kQ)->InitDouble("Q", 2.0, 0.001, 5., 0.001, "");
  GetParam(kFreq)->InitDouble("Freq", 50, 0., 100., 1., "");
  GetParam(kFreqMin)->InitDouble("F Min", 178, 20., 2000, 1., "");
  GetParam(kFreqMax)->InitDouble("F Max", 1600, 20., 2000, 1., "");
  GetParam(kGain)->InitDouble("Gain", 6, -18, 18, 0.1, "");
  GetParam(kMix)->InitDouble("Mix", 91., 0., 100, 0.1, "");

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  attackCounter = attackSamples;
  releaseCounter = releaseSamples;

   //Generate Background
  IGraphics* pGraphics = MakeGraphics(this, GUI_WIDTH, GUI_HEIGHT); // MakeGraphics(this, kW, kH);
  pGraphics->AttachBackground(BG_ID, BG_FN);
  
  //Generate Knobs
  IBitmap bitmap = pGraphics->LoadIBitmap(KNOB_GAIN_ID, KNOB_GAIN_FN);
  pGraphics->AttachControl(new IKnobRotaterControl(this, kGainX, kGainY, kGain, &bitmap));
  
  bitmap = pGraphics->LoadIBitmap(KNOB_Q_ID, KNOB_Q_FN);
  pGraphics->AttachControl(new IKnobRotaterControl(this, kQX, kQY, kQ, &bitmap));
  
  bitmap = pGraphics->LoadIBitmap(KNOB_FREQ_ID, KNOB_FREQ_FN);
  pGraphics->AttachControl(new IKnobRotaterControl(this, kFreqX, kFreqY, kFreq, &bitmap));
  AttachGraphics(pGraphics);

}

FreakkWah::~FreakkWah() {}

void FreakkWah::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  // double logThreshold = log10 ( abs(mBoost) ) ; // work with decibels

	for (int s = 0; s < nFrames; ++s) {
		outputs[0][s] = ( inputs[0][s] * (1-mMix) + Filter->process(inputs[0][s]) * mMix ) * mBoost ;
	  }
}

void FreakkWah::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void FreakkWah::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
	case kBoost:
      mBoost = GetParam(kBoost)->Value() / 100.;
      break;

	case kQ:
      mQ = GetParam(kQ)->Value();
	  SetFreq();
      break;

	case kFreq:
      mFreq = GetParam(kFreq)->Value()/100.;
	  SetFreq();
      break;

	case kFreqMin:
      mFreqMin = GetParam(kFreqMin)->Value();
	  SetFreq();
      break;

	case kFreqMax:
      mFreqMax = GetParam(kFreqMax)->Value();
	  SetFreq();
      break;

	case kGain:
      mGain = GetParam(kGain)->Value();
	  SetFreq();
      break;

	case kMix:
      mMix = GetParam(kMix)->Value()/100.;
      break;

    default:
      break;
  }

}

double FreakkWah::Peak(double* samples, int length)
{
	double pk = abs(samples[0]);
	for(int i=1; i < length ; i++) 
			if ( abs(samples[i]) > pk) pk = abs(samples[i]);
	return pk;
}

void FreakkWah::SetFreq()
{
	double Fc = mFreqMin + (mFreqMax-mFreqMin)*mFreq;
	Filter->setBiquad(bq_type_bandpass, Fc / sampleRate, mQ, mGain);
}