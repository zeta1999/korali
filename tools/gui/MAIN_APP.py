'''
Welcome to the Korali Graphical-User-Interface created by Mark Martori Lopez (martori.mark@gmail.com) under the CSE-Lab-ETH.              #####
                                                                                                                                          #####
                                                                                                                                          #####
*Starting File: Runs the APP KORALI, sets window parameters [Geometry and Size] and initializes the listener of the Embeded HTML.         #####
*Incorporates the links displayed on the Third Frame ( CSE-Lab Korali HTML embeded ) under the dictionary: links_website.                 #####
*Dictionary chosen as key, link as value.                                                                                                 #####
*Defines the KORALI Version.                                                                                                              #####
'''

# if python2.7 Tkinter. ttk -> CSS.
from cefpython3 import cefpython as cef
try:
    import tkinter as tk
    from tkinter import *
    from tkinter import ttk
except ImportError:
    import Tkinter as tk
    from Tkinter import *
    from Tkinter import ttk

# Files import:
import class_KORALI

# KORALI's VERSION:
version = 'Korali v2.0.0 (15/06/2020)'
version_no_date = 'Korali v2.0.0'



if __name__ == "__main__":
    app = class_KORALI.KORALI()
    app.geometry("1500x1000") # Size of our appliication.
    app.minsize("1500","1000")
    app.title(version_no_date)
    cef.Initialize()
    app.mainloop()
    cef.Shutdown()




links_website = {'sampling':'https://www.cse-lab.ethz.ch/korali/docs/examples/sampling.html',
                 'reinforcementLearning':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/reinforcementLearning/reinforcementLearning.html',
                 'bayesian':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/bayesian/bayesian.html',
                 'custom':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/bayesian/custom/custom.html',
                 'latent':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/bayesian/latent/latent.html',
                 'reference':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/bayesian/reference/reference.html',
                 'approximate':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/bayesian/approximate/approximate.html',
                 'supervisedLearning':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/supervisedLearning/supervisedLearning.html',
                 'propagation':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/propagation/propagation.html',
                 'hierarchical':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/hierarchical/hierarchical.html',
                 'thetaNew':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/hierarchical/thetaNew/thetaNew.html',
                 'psi':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/hierarchical/psi/psi.html',
                 'theta':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/hierarchical/theta/theta.html',
                 'optimization':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/optimization/optimization.html',
                 'integration':'https://www.cse-lab.ethz.ch/korali/docs/modules/problem/integration/integration.html',
                 'gaussianProcess':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/gaussianProcess/gaussianProcess.html',
                 'DQN':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/DQN/DQN.html',
                 'optimizer':'',
                 'LMCMAES':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/LMCMAES/LMCMAES.html#module-solver-lmcmaes',
                 'CMAES':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/CMAES/CMAES.html#module-solver-cmaes',
                 'gridSearch':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/gridSearch/gridSearch.html',
                 'Adam':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/Adam/Adam.html',
                 'DEA':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/DEA/DEA.html#module-solver-dea',
                 'Rprop':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/Rprop/Rprop.html',
                 'deepLearner':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/deepLearner/deepLearner.html',
                 'executor':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/executor/executor.html',
                 'SAEM':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/SAEM/SAEM.html',
                 'sampler':'',
                 'TMCMC':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/TMCMC/TMCMC.html#module-solver-tmcmc',
                 'Nested':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/Nested/Nested.html',
                 'MCMC':'https://www.cse-lab.ethz.ch/korali/docs/modules/solver/MCMC/MCMC.html#module-solver-mcmc'}
