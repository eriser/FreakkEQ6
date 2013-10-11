#include "SVF.h"


SVF::SVF(double fs)
{
	Fs = fs;
	l=0;b=0;h=0,n=0; //hipass
    SetFc(0.5);
    SetQ(0.9);
	Scale = sqrt(Q);
}


SVF::~SVF(void)
{
}

void SVF::Process(double in)
  {

	  // 2x OVERSAMPLING
	  for (int i=0; i<2; i++) {
		l = l + Fc * b;
		h = Scale * in - l - Q * b;
		b = Fc * h + b; 
		n = h + l;
	  }

  }

// SETTERS

void SVF::SetFc(double f)
{
	// f could be from 20 to 20k [Hz]
	Fc = 2 * sin(2 * M_PI * f/(2 * Fs) );
}


void SVF::SetQ(double q)
{
	// Q values: 0 -> 100
	Q = sqrt(1 - atan(sqrt(q)) * 2 / M_PI);
	Scale = sqrt(Q);
}

// GETTERS
double SVF::High()
{
	return h;
}

double SVF::Low()
{
	return l;
}

double SVF::Band()
{
	return b;
}

double SVF::Notch()
{
	return n;
}
