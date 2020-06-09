#!/usr/bin/env python3
import numpy as np

# Creating new experiment
import korali
e = korali.Experiment()

e["Problem"]["Type"] = "Optimal Experimental Design"
e["Problem"]["Utility Function"] = "Kullback-Leibler"
e["Problem"]["Likelihood Model"] = "Normal"
e["Problem"]["Design Parameter Dimensionality"] = 1
e["Problem"]["Data Dimensionality"] = 1
e["Problem"]["Number of Measurements"] = 1

e["Variables"][0]["Name"] = "d"
e["Variables"][0]["Type"] = "Design Parameters"
e["Variables"][0]["Values"] = np.loadtxt("designParameters.dat").tolist()

e["Variables"][1]["Name"] = "F(d,theta)"
e["Variables"][1]["Type"] = "Model Samples"
e["Variables"][1]["Values"] = np.loadtxt("modelSamples.dat").flatten().tolist()

e["Solver"]["Type"] = "Experiment Designer"
e["Solver"]["Integration Method Parameters"] = "Monte Carlo"
e["Solver"]["Ny"] = 100
e["Solver"]["Optimisation Method"] = "Sequential Optimisation"

e["Console Output"]["Verbosity"] = "Detailed"

# Starting Korali's Engine and running experiment
k = korali.Engine()
k.run(e)
