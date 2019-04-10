#include <iostream>
#include <memory>
#include <stdlib.h>

#include "korali.h"

using namespace std;
using namespace Korali::Parameter;

int main(int argc, char* argv[])
{

 size_t N = 0;
 unique_ptr<Korali::Parameter::Base> p[20];

 p[N++] = make_unique<Cauchy>( 2., 5. );
 p[N++] = make_unique<Cauchy>( 1 );
 p[N++] = make_unique<Exponential>( -2., 3. );
 p[N++] = make_unique<Exponential>( 3 );
 p[N++] = make_unique<Gamma>( 2., 3. );
 p[N++] = make_unique<Gaussian>( 0., 0.1 );
 p[N++] = make_unique<Gaussian>( 2., 3. );
 p[N++] = make_unique<Uniform>( -2., 5. );
 p[N++] = make_unique<Laplace>( -2., 5. );

 double x = 1.;

 double den, log_den;

 // Test that log-density is correctly implemented
 for( size_t k=0; k<N; k++){
   den = p[k]->getDensity(x);
   log_den = p[k]->getDensityLog(x) ;

   if( gsl_isinf(log_den)==-1 ) log_den = 0.;
   else log_den = gsl_sf_exp( log_den );

   p[k]->printDetails(); cout<<":\n";
   cout << "   Density - exp(LogDensity)  =  " <<  den - log_den << "\n\n";
 }

 // Test the statistics of the random number.
 // unique_ptr<Korali::Parameter::Base> a;
 // size_t Ns = 1e3;
 // double *samples = (double*)malloc( Ns * sizeof(double) );
 // double stat;
 //
 // // 1. Gaussian
 // a = make_unique<Gaussian>(0.,1.);
 // for(size_t i=0; i<Ns; i++) samples[i] = a->getRandomNumber() ;
 // cout << gsl_stats_mean(samples, 1, Ns) << endl;


 return 0;
}
