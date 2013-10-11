#pragma once
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class SVF
{
private:
  double l,b,h,n;
  double Fs,Fc,Q,Scale;

public:
  SVF(double samplinfreq);
  ~SVF(void);
  void Process(double in);
  
  // SETTERS
  void SetFc(double f);
  void SetQ(double q);
  
  //GETTERS
  double High();
  double Low();
  double Band();
  double Notch();
};

