#include "ackley.h"
#include <math.h>

double ackley(double *x)
{
   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;

   for (int i = 0; i < NDIMS; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }

   return -a*exp(-b*sqrt(s1/NDIMS)) - exp(s2/NDIMS) + a + exp(1.);
}
