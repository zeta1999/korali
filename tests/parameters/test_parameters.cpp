#include <iostream>
#include <memory>
#include "korali.h"

using namespace std;

int main(int argc, char* argv[])
{

 size_t N = 0;
 unique_ptr<Korali::Parameter::Base> p[20];

 N++; p[0] = make_unique<Korali::Parameter::Cauchy>( 2., 5. );
 N++; p[1] = make_unique<Korali::Parameter::Cauchy>( 1 );
 N++; p[2] = make_unique<Korali::Parameter::Exponential>( -2., 3. );
 N++; p[3] = make_unique<Korali::Parameter::Exponential>( 3 );
 N++; p[4] = make_unique<Korali::Parameter::Gamma>( 2., 3. );
 N++; p[5] = make_unique<Korali::Parameter::Gaussian>( 0., 0.1 );
 N++; p[6] = make_unique<Korali::Parameter::Gaussian>( 2., 3. );
 N++; p[7] = make_unique<Korali::Parameter::Uniform>( -2., 5. );


 double x = 1.;


 double den, log_den;

 for( size_t k=0; k<N; k++){

   cout << "\n";
   den = p[k]->getDensity(x);
   log_den = p[k]->getDensityLog(x) ;



   if( gsl_isinf(log_den)==-1 )
    log_den = 0.;
   else
    log_den = gsl_sf_exp( log_den );

    p[k]->printDetails(); cout<<":\n";
    cout << den << "  " << log_den << endl;
   cout << "   Density - exp(LogDensity)  =  " <<  den - log_den << "\n\n";

 }







 return 0;
}
