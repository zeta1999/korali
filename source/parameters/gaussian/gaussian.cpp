#include "korali.h"

using namespace Korali::Parameter;


Gaussian::Gaussian() : _mean(1.), _sigma(1.) {}

Gaussian::Gaussian(double mean, double sigma) : Gaussian(mean, sigma, 0xC0FFEE){}

Gaussian::Gaussian(double mean, double sigma, size_t seed) : _mean(mean), _sigma(sigma)
{
  this->Korali::Parameter::Base::initialize(seed);
  _aux = -0.5*gsl_sf_log(2*M_PI) - gsl_sf_log(_sigma);
}


double Gaussian::getDensity(double x)
{
 return gsl_ran_gaussian_pdf(x - _mean, _sigma);
}

double Gaussian::getDensityLog(double x)
{
 // Optimize by pre-calculating constants and using multiplication instead of power.
 return _aux - 0.5*gsl_sf_pow_int( (x-_mean)/_sigma, 2 );
}

double Gaussian::getRandomNumber()
{
 return _mean + gsl_ran_gaussian(_range, _sigma);
}


double Gaussian::logLikelihood(double sigma, int nData, double* x, double* u)
{
 if (nData == 0)
 {
  fprintf(stderr, "[Korali] Error: _problem's reference dataset not defined for the Likelihood (use: _problem.setReferenceData()).\n");
   return 0.0;
 }

 double sigma2 = sigma*sigma;
 double ssn = 0.0;

 for(int i = 0; i < nData; i++)
  {
  double diff = x[i] - u[i];
  ssn += diff*diff;
  }

 double res = 0.5* (nData*log(2*M_PI) + nData*log(sigma2) + ssn/sigma2);

 // printf("Sigma: %.11f - Res: %f\n", sigma, res);

 return res;
}




void Gaussian::printDetails()
{
  printf("Gaussian(%.3g,%.3g)", _mean, _sigma);
}
