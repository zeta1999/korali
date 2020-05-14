
import sys
sys.path.append('./model')
from model import *
from tutorial_samplers import *

import numpy as np
from numpy.random import default_rng

import korali


def main():
    # Initialize the distribution
    distrib = ConditionalDistribution4()

    rng = default_rng()
    initial_hyperparams = rng.standard_normal(2) # 1d mean and cov

    k = korali.Engine()
    e = korali.Experiment()

    e["Problem"]["Type"] = "Bayesian/Latent/HierarchicalLatent"
    # the distribution already "contains" the data, it implements the function: latent --> p(data | latent)
    e["Problem"]["Conditional Log Likelihood Function"] = lambda sample : distrib.conditional_p(sample)

    e["Solver"]["Type"] = "hSAEM"
    e["Solver"]["Number Samples Per Step"] = 100
    e["Solver"]["Termination Criteria"]["Max Generations"] = 30

    e["Distributions"][0]["Name"] = "Uniform 0"
    e["Distributions"][0]["Type"] = "Univariate/Uniform"
    e["Distributions"][0]["Minimum"] = -100
    e["Distributions"][0]["Maximum"] = 100

    # * Define which hyperparameters we use (only theta, the mean of p(data | theta, sigma) - sigma is assumed known)
    e["Variables"][0]["Name"] = "latent mean"
    e["Variables"][0]["Bayesian Type"] = "Latent Variable"
    e["Variables"][0]["Latent Variable Distribution Type"] = "Normal"
    e["Variables"][0]["Prior Distribution"] = "Uniform 0"  # not used (?) but required

    e["File Output"]["Frequency"] = 50
    e["Console Output"]["Frequency"] = 10
    e["Console Output"]["Verbosity"] = "Detailed"

    k.run(e)




if __name__ == '__main__':
    # ** For debugging, try this: **
    # import sys, trace
    # sys.stdout = sys.stderr
    # tracer = trace.Trace(trace=1, count=0, ignoredirs=["/usr", sys.prefix])
    # tracer.runfunc(main)
    # ** Else: **
    main()
