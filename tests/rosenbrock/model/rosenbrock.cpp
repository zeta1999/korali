#include "rosenbrock.h"
#include <stdio.h>
#include <math.h>

double rosenbrock(double *x)
{
  double s = 0.;
  for (int i = 0; i < NDIMS-1; i++)
      s += 100.*pow(x[i+1]-x[i]*x[i], 2) + pow(x[i]-1., 2);

  return s;
}
