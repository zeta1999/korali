import numpy as np
import matplotlib.pyplot as plt
import argparse


# parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('--numSimulations', help='number of simulations to perform', required=True, type=int)
parser.add_argument('--numLocations', help='number of possible sensor location', required=True, type=int)
args = vars(parser.parse_args())

numSimulations = args['numSimulations']
numLocations = args['numLocations']

# create parameters for simulations (1,..,Nr) and locations (s)
theta = np.random.uniform(0,1,numSimulations)
d = np.linspace(0,1,numLocations+1)

# perform simulations
measurements = []
for th in theta:
  measurement = th**3*d**2 + th*np.exp(-np.abs(0.2-d))
  measurements.append(measurement)

with open("designParameters.dat",'w') as parametersFile, open("modelSamples.dat",'w') as measurementFile:
  np.savetxt(measurementFile, measurements)
  np.savetxt(parametersFile, d)
