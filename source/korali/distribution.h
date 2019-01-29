#ifndef _DISTRIBUTION_H_
#define _DISTRIBUTION_H_

#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

namespace Korali
{

class Distribution
{
 protected:

 gsl_rng* _range;
 size_t _seed;

 public:
  Distribution(size_t seed);
  virtual double getDensity(double x){};
  virtual double getDensityLog(double x){};
  virtual double getRandomNumber(){};
};

class GaussianDistribution : public Distribution
{
 using Distribution::Distribution;

 private:
  double _mean;
  double _sigma;

 public:
  GaussianDistribution(double mean, double sigma, size_t seed = 0) : _mean(mean), _sigma(sigma), Distribution(seed) {};
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

class UniformDistribution : public Distribution
{
 private:
  double _min;
  double _max;

 public:
  UniformDistribution(double min, double max, size_t seed = 0) : _min(min), _max(max), Distribution(seed) {};
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

class GammaDistribution : public Distribution
{
 private:
  double _shape;
  double _rate;

 public:
  GammaDistribution(double shape, double rate, size_t seed = 0) : _shape(shape), _rate(rate), Distribution(seed) {};
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

class ExponentialDistribution : public Distribution
{
 private:
  double _mean;

 public:
  ExponentialDistribution(double mean, size_t seed = 0) : _mean(mean), Distribution(seed) {};
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

} // namespace Korali

#endif // _DISTRIBUTION_H_
