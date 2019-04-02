#include "parameters/gaussian.h"

Korali::Parameter::Gaussian::Gaussian(double mean, double sigma) : Korali::Parameter::Gaussian::Gaussian("NoNameGaussian", mean, sigma){}
Korali::Parameter::Gaussian::Gaussian(std::string name, double mean, double sigma) : Korali::Parameter::Base::Base(name)
{
 _mean = mean;
 _sigma = sigma;
}

double Korali::Parameter::Gaussian::getDensity(double x)
{
 return gsl_ran_gaussian_pdf(x - _mean, _sigma);
}

double Korali::Parameter::Gaussian::getDensityLog(double x)
{
 // Optimize by pre-calculating constants and using multiplication instead of power.
 return -0.5*log(2*M_PI) - log(_sigma) - 0.5*pow((x-_mean)/_sigma, 2);
}

double Korali::Parameter::Gaussian::getRandomNumber()
{
 return _mean + gsl_ran_gaussian(_range, _sigma);
}

double Korali::Parameter::Gaussian::logLikelihood(double sigma, int nData, double* x, double* u)
{
 if (nData == 0)
 {
  fprintf(stderr, "[Korali] Error: Problem's reference dataset not defined for the Likelihood (use: Problem.setReferenceData()).\n");
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

void Korali::Parameter::Gaussian::printDetails()
{
 printf("Gaussian(%.3g, %.3g)", _mean, _sigma);
}
