#!/usr/bin/env python
import sys
import os.path
if not os.path.isfile('../../cxx/heat2d/model/libheat2d.so'):
 os.system('cd ../../cxx/heat2d/model && make')
sys.path.append('../../cxx/heat2d/model')
sys.path.append('../../../source/')
import libheat2d
import libkorali
 
korali = libkorali.Engine(libheat2d.heat2DSolver)

korali["Seed"] = 0xC0FFEE
korali["Verbosity"] = "Normal"

korali["Parameters"][0]["Name"] = "Intensity"
korali["Parameters"][0]["Type"] = "Computational"
korali["Parameters"][0]["Distribution"] = "Uniform"
korali["Parameters"][0]["Minimum"] = 10.0
korali["Parameters"][0]["Maximum"] = 60.0

korali["Parameters"][1]["Name"] = "PosX"
korali["Parameters"][1]["Type"] = "Computational"
korali["Parameters"][1]["Distribution"] = "Uniform"
korali["Parameters"][1]["Minimum"] = 0.0
korali["Parameters"][1]["Maximum"] = 0.5

korali["Parameters"][2]["Name"] = "PosY"
korali["Parameters"][2]["Type"] = "Computational"
korali["Parameters"][2]["Distribution"] = "Uniform"
korali["Parameters"][2]["Minimum"] = 0.6
korali["Parameters"][2]["Maximum"] = 1.0

korali["Parameters"][3]["Name"] = "Sigma"
korali["Parameters"][3]["Type"] = "Statistical"
korali["Parameters"][3]["Distribution"] = "Uniform"
korali["Parameters"][3]["Minimum"] = 0.0
korali["Parameters"][3]["Maximum"] = 20.0

korali["Problem"]["Objective"] = "Posterior"

p = libheat2d.heat2DInit()
for i in range(len(p.refTemp)):
 korali["Problem"]["Reference Data"][i] = p.refTemp[i]

korali["Solver"]["Method"] = "TMCMC"
korali["Solver"]["Covariance Scaling"] = 0.02
korali["Solver"]["Population Size"] = 3000

korali.run()