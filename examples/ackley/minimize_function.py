#!/usr/bin/env python
import sys
import math
sys.path.append('/home/martiser/skorali/korali/lib')
import libkorali

def ackley(x):
 a = 20.0
 b = 0.2
 c = 2.*math.pi
 s1 = 0.0
 s2 = 0.0

 for i in range(4):
  s1 += x[i]*x[i]
  s2 += math.cos(c*x[i])

 return -(-a*math.exp(-b*math.sqrt(s1/4)) - math.exp(s2/4) + a + math.exp(1.))
  
korali = libkorali.Engine(ackley)

korali["Verbosity"] = "Normal";

for i in range(4):
 korali["Parameters"][i]["Name"] = "X" + str(i);
 korali["Parameters"][i]["Distribution"] = "Uniform";
 korali["Parameters"][i]["Type"] = "Computational";
 korali["Parameters"][i]["Minimum"] = -32.0;
 korali["Parameters"][i]["Maximum"] = +32.0;

korali["Problem"]["Objective"] = "Direct Evaluation";
korali["Solver"]["Method"] = "CMA-ES";
korali["Solver"]["Termination Criteria"]["Min DeltaX"] = 1e-11;
korali["Solver"]["Termination Criteria"]["Max Generations"] = 600;
korali["Solver"]["Lambda"] = 128;

korali.run()