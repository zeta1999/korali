
import sys
sys.path.append('./model')
from model import *
from tutorial_samplers import *

import numpy as np

import korali


def main():
    # Initialize the distribution
    distrib = ConditionalDistribution4()

    # # rng = np.random.default_rng()
    # # initial_hyperparams = rng.standard_normal(2) # 1d mean and cov
    # initial_hyperparams = np.random.standard_normal(2) # 1d mean and cov

    k = korali.Engine()
    e = korali.Experiment()

    e["Problem"]["Type"] = "Bayesian/Latent/HierarchicalLatent"
    # the distribution already "contains" the data, it implements the function: latent --> p(data | latent)
    e["Problem"]["Conditional Log Likelihood Function"] = lambda sample : distrib.conditional_p(sample)

    e["Solver"]["Type"] = "HSAEM"
    e["Solver"]["Number Samples Per Step"] = 100
    e["Solver"]["Termination Criteria"]["Max Generations"] = 30

    e["Distributions"][0]["Name"] = "Uniform 0"
    e["Distributions"][0]["Type"] = "Univariate/Uniform"
    e["Distributions"][0]["Minimum"] = -100
    e["Distributions"][0]["Maximum"] = 100

    # * Define which hyperparameters we use (only the means - sigma is assumed known)
    for i in range(1): # range(distrib.nIndividuals):
        e["Variables"][i]["Name"] = "latent mean "+str(i)
        e["Variables"][i]["Initial Value"] = -5
        e["Variables"][i]["Bayesian Type"] = "Latent"
        e["Variables"][i]["Individual Index"] = i
        e["Variables"][i]["Distribution Index"] = 0
        e["Variables"][i]["Latent Variable Distribution Type"] = "Normal"
        e["Variables"][i]["Prior Distribution"] = "Uniform 0"  # not used (?) but required

    e["File Output"]["Frequency"] = 50
    e["Console Output"]["Frequency"] = 10
    e["Console Output"]["Verbosity"] = "Detailed"

    k.run(e)




if __name__ == '__main__':
    # ** For debugging, try this: **
    import sys, trace
    sys.stdout = sys.stderr
    tracer = trace.Trace(trace=1, count=0, ignoredirs=["/usr", sys.prefix])
    tracer.runfunc(main)
    # # ** Else: **
    # main()
