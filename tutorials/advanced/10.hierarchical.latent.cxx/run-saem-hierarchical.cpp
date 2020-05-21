
#include "model/model.hpp"
#include "korali.hpp"

#include <vector>


ConditionalDistribution4 distrib4 = ConditionalDistribution4();

void distrib4_conditional_p(korali::Sample& s);
void distrib4_conditional_p(korali::Sample& s)
{
  distrib4.conditional_p(s);
}


int main(int argc, char* argv[])
{

  auto k = korali::Engine();
  auto e = korali::Experiment();


    e["Problem"]["Type"] = "Bayesian/Latent/HierarchicalLatent";
   // the distribution already "contains" the data, it implements the function: latent --> p(data | latent)
    e["Problem"]["Conditional Log Likelihood Function"] = &distrib4_conditional_p; // defined in model.cpp

    e["Solver"]["Type"] = "HSAEM";
    e["Solver"]["Number Samples Per Step"] = 100;
    e["Solver"]["Termination Criteria"]["Max Generations"] = 30;

    e["Distributions"][0]["Name"] = "Uniform 0";
    e["Distributions"][0]["Type"] = "Univariate/Uniform";
    e["Distributions"][0]["Minimum"] = -100;
    e["Distributions"][0]["Maximum"] = 100;

    // * Define which hyperparameters we use (only theta, the mean of p(data | theta, sigma) - sigma is assumed known)
    e["Variables"][0]["Name"] = "latent mean";
    e["Variables"][0]["Initial Value"] = -5;
    e["Variables"][0]["Bayesian Type"] = "Latent";
    e["Variables"][0]["Latent Variable Distribution Type"] = "Normal";
    e["Variables"][0]["Prior Distribution"] = "Uniform 0" ; // not used (?) but required

    e["File Output"]["Frequency"] = 50;
    e["Console Output"]["Frequency"] = 10;
    e["Console Output"]["Verbosity"] = "Detailed";

    k.run(e);

    return 0;

}
