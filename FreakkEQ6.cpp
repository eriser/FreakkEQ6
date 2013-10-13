#include "FreakkEQ6.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <math.h>

#define OFFSET 0.01
#define BYPASS 0
#define MUTE 1
#define ATTACK 2
#define RELEASE 3

#define NUM_BANDS 6

const int kNumPrograms = 1;

enum EParams
{

  kBandGain0,
  kBandGain1,
  kBandGain2,
  kBandGain3,
  kBandGain4,
  kBandGain5,
  kQ,
  
  kGain,
  kMix,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kFader_L = 136,
  kFader_Ver_Y = 94,

  kFader_X_0 = 34,
  kFader_X_1 = 69,
  kFader_X_2 = 104,
  kFader_X_3 = 138,
  kFader_X_4 = 173,
  kFader_X_5 = 207,

  kFader_X_Q = 56,
  kFader_Y_Q = 59,
  
  kKnobFrames = 60
};

FreakkEQ6::FreakkEQ6(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  sampleRate = GetSampleRate();

  mQ = new double[NUM_BANDS];
  mQ[0] = 0.8; mQ[1] = 0.8; mQ[2] = 0.8; mQ[3] = 0.8; mQ[4] = 0.8; mQ[5] = 0.8;

  mFreq = new double[NUM_BANDS];
  mFreq[0] = 100; mFreq[1] = 200; mFreq[2] = 400; mFreq[3] = 800; mFreq[4] = 1600; mFreq[5] = 3200;

  mBandGain = new double[NUM_BANDS];

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  
  GetParam(kBandGain0)->InitDouble("G0", 0, -18, 18, 0.1, "");
  GetParam(kBandGain1)->InitDouble("G1", 0, -18, 18, 0.1, "");
  GetParam(kBandGain2)->InitDouble("G2", 0, -18, 18, 0.1, "");
  GetParam(kBandGain3)->InitDouble("G3", 0, -18, 18, 0.1, "");
  GetParam(kBandGain4)->InitDouble("G4", 0, -18, 18, 0.1, "");
  GetParam(kBandGain5)->InitDouble("G5", 0, -18, 18, 0.1, "");

  GetParam(kQ)->InitDouble("Q", 1., 0.1, 10., 0.1, "");

  GetParam(kGain)->InitDouble("Gain", 100., 0., 200, 0.1, "");
  GetParam(kMix)->InitDouble("Mix", 100., 0., 100, 0.1, "");

  Filter = new Biquad*[NUM_BANDS]; // allocate bands
  for(int i=0; i<NUM_BANDS ; i++)  Filter[i] = new Biquad(); // init bands

  Filter[0]->setBiquad(bq_type_peak, mFreq[0] / sampleRate, mQ[0], 0);
  Filter[1]->setBiquad(bq_type_peak, mFreq[1] / sampleRate, mQ[1], 0);
  Filter[2]->setBiquad(bq_type_peak, mFreq[2] / sampleRate, mQ[2], 0);
  Filter[3]->setBiquad(bq_type_peak, mFreq[3] / sampleRate, mQ[3], 0);
  Filter[4]->setBiquad(bq_type_peak, mFreq[4] / sampleRate, mQ[4], 0);
  Filter[5]->setBiquad(bq_type_peak, mFreq[5] / sampleRate, mQ[5], 0);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  //Generate Background
  IGraphics* pGraphics = MakeGraphics(this, GUI_WIDTH, GUI_HEIGHT); // MakeGraphics(this, kW, kH);
  pGraphics->AttachBackground(BG_ID, BG_FN);
  
  
  //Generate Faders

  //Attach the vertical IFaderControl
  IBitmap bitmap = pGraphics->LoadIBitmap(FADER_VER_ID, FADER_VER_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_0, kFader_Ver_Y, kFader_L, kBandGain0, &bitmap)); // kVertical is default

  bitmap = pGraphics->LoadIBitmap(FADER_VER_ID, FADER_VER_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_1, kFader_Ver_Y, kFader_L, kBandGain1, &bitmap)); // kVertical is default

  bitmap = pGraphics->LoadIBitmap(FADER_VER_ID, FADER_VER_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_2, kFader_Ver_Y, kFader_L, kBandGain2, &bitmap)); // kVertical is default

  bitmap = pGraphics->LoadIBitmap(FADER_VER_ID, FADER_VER_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_3, kFader_Ver_Y, kFader_L, kBandGain3, &bitmap)); // kVertical is default

  bitmap = pGraphics->LoadIBitmap(FADER_VER_ID, FADER_VER_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_4, kFader_Ver_Y, kFader_L, kBandGain4, &bitmap)); // kVertical is default
  
  bitmap = pGraphics->LoadIBitmap(FADER_VER_ID, FADER_VER_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_5, kFader_Ver_Y, kFader_L, kBandGain5, &bitmap)); // kVertical is default

  
  //Attach the horizontal IFaderControl
  bitmap = pGraphics->LoadIBitmap(FADER_HOR_ID, FADER_HOR_FN);
  pGraphics->AttachControl(new IFaderControl(this, kFader_X_Q, kFader_Y_Q, kFader_L, kQ, &bitmap, kHorizontal));

  AttachGraphics(pGraphics);
  

}

FreakkEQ6::~FreakkEQ6() {}

void FreakkEQ6::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

	for (int s = 0; s < nFrames; ++s) {
		outputs[0][s] = inputs[0][s];
		for(int j = 0; j < NUM_BANDS; ++j)
			outputs[0][s] = Filter[j]->process( outputs[0][s] );
	  }
}

void FreakkEQ6::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void FreakkEQ6::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  {
	  switch (paramIdx)
	  {

	   case kBandGain0:
		  mBandGain[0] = GetParam(kBandGain0)->Value();
		  SetGain(0);
		  break;
	   case kBandGain1:
		  mBandGain[1] = GetParam(kBandGain1)->Value();
		  SetGain(1);
		  break;
	   case kBandGain2:
		  mBandGain[2] = GetParam(kBandGain2)->Value();
		  SetGain(2);
		  break;
	   case kBandGain3:
		  mBandGain[3] = GetParam(kBandGain3)->Value();
		  SetGain(3);
		  break;
	   case kBandGain4:
		  mBandGain[4] = GetParam(kBandGain4)->Value();
		  SetGain(4);
		  break;
	   case kBandGain5:
		  mBandGain[5] = GetParam(kBandGain5)->Value();
		  SetGain(5);
		  break;

	  case kQ:
		  for(int i=0; i<NUM_BANDS ; i++) {
			  mQ[i] = GetParam(kQ)->Value();
			  SetGain(i);
		  }
		  break;

		case kGain:
		  mGain = GetParam(kGain)->Value()/100.;
		  break;
		case kMix:
		  mMix = GetParam(kMix)->Value()/100.;
		  break;

		default:
		  break;
	  }


  }

}


void FreakkEQ6::SetGain(int n)
{
		Filter[n]->setBiquad(bq_type_peak, mFreq[n] / sampleRate, mQ[n], mBandGain[n]);
}