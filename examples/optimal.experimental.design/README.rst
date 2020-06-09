Integration
==================

In this tutorial we show how to perform **optimal experimental design** with Korali. 

Problem Description
------------------- 

We are given the function :math:`f(\theta,d)=\theta^3d^2+\theta\exp(-|0.2-d|)`. We assume that :math:`d\in [0,1]` is the design parameter and :math:`\theta\in [0,1]` is the model parameter we want to infer.

Sample the model for different parameters
-----------------------------------------

Run the script called `nonlinear.py --numSimulations nTheta --numLocations nParams`. Here, `nTheta` denotes the number of parameters for which we want to evaluate the model. We assume that the prior distribution over the parameters is uniform :math:`\theta\sim{\cal U}([0,1])`. `nParams` denotes the number of possible values for the design parameter. Here we assume an uniformly spaced grid :math:`d=(d_1,\dots,d_{nParams})` with :math:`\Delta d=d_{i+1}-d_i=\frac{1}{nParams}`.

Optimal Experimental Design with Korali
----------------------------------------

First, open a file (you could name it 'run-oed.py') and import the korali module

.. code-block:: python

    #!/usr/bin/env python3
    import numpy as np
    import korali

The Korali Engine and Experiment Objects
----------------------------------------

Next we construct a `korali.Engine` and a `korali.Experiment` object and set the computational model,

.. code-block:: python

    k = korali.Engine()
    e = korali.Experiment()

The Problem Type
----------------

Then, we set the type of the problem to `Optimal Experimental Design`, set type of utility function, the dimensionality of the parameter space and the data dimensionality

.. code-block:: python

        e["Problem"]["Type"] = "Optimal Experimental Design"
        e["Problem"]["Utility Function"] = "Kullback-Leibler"
        e["Problem"]["Design Parameter Dimensionality"] = 1
        e["Problem"]["Data Dimensionality"] = 1
        e["Problem"]["Number of Measurements"] = 1

The Variables
-------------

In this problem there is two Korali variables which we have to provide, `d` the possible design variables and `F(d,theta)` which is the model evaluations for the given samples for :math:`\theta`

.. code-block:: python

        e["Variables"][0]["Name"] = "d"
        e["Variables"][0]["Type"] = "Design Parameters"
        e["Variables"][0]["Values"] = np.loadtxt("designParameters.dat").tolist()

        e["Variables"][1]["Name"] = "F(d,theta)"
        e["Variables"][1]["Type"] = "Design Parameters"
        e["Variables"][1]["Values"] = np.loadtxt("designParameters.dat").tolist()

The Solver
----------
We choose the solver `Experiment Designer`, were we select the `Integration Method Parameters` used for the parameter integral, the number of samples `Ny` used for the integration over the measurement space and the `Optimisation Method`.

.. code-block:: python

        e["Solver"]["Type"] = "Experiment Designer"
        e["Solver"]["Integration Method Parameters"] = "Monte Carlo"
        e["Solver"]["Ny"] = 100
        e["Solver"]["Optimisation Method"] = "Sequential Optimisation"

For a detailed description of Integrator settings see :ref:`Integrator <module-solver-integrator>`.

Finally, we need to add a call to the run() routine to start the Korali engine.

.. code-block:: python

    k.run(e)

Running
-------

We are now ready to run our example:
`./run-integration`

Or, alternatively:
`python3 ./run-integration.py`

The results are saved in the folder `_korali_result/`.
