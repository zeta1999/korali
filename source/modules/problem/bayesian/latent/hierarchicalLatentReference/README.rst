**************************************************************************
Hierarchical Latent-Variable Problem with Likelihood by Reference
**************************************************************************

Hierarchical latent problems impose a specific hierarchical
form on the total likelihood:

.. math::
  p( d, \theta  | \psi ) = \left(\prod_{i=0}^N \prod_{j=0}^{n_i} p(d_{ij} | \theta_i) \right) \cdot p(\theta_i | \psi)


where

- :math:`d` is the data, where we have a varying number :math:`n_i` of data points :math:`d_{i,j}` for each 'individual' :math:`i`
- Vectors :math:`\theta_i` are latent variables, one per 'individual' :math:`i`
- :math:`\psi` are a number of hyperparameters.

A hierarchical *Reference* problem additionally assumes that the data come from a functional relationship:
Every data point :math:`x` comes with a reference evaluation :math:`y`, where we assume
:math:`y = f(x, \theta) + \epsilon`. :math:`\epsilon` is a noise term and defines the *likelihood model*
for the data.

Our data is composed of :math:`n_i` data points and reference evaluations per individual :math:`i`:

.. math::


  d = (x_{ij}, y_{ij})_{i\in I, j\in J_i}, \\
  where \; I = 1...N, J_i = 1...n_i, \;\; and\\
  y_{ij} = f(x_{ij}, \theta_i) + \epsilon_{ij}.

The x values :math:`X = (x_{ij})_{i\in I, j\in J_i}` can also be omitted. This corresponds to a
computational model :math:`f(\theta_i)` that does not depend on additional data points :math:`x_{ij}`.

Usage
~~~~~

.. code-block:: python

    e = korali.Experiment()
    e["Problem"]["Type"] = "Evaluation/Bayesian/Latent/HierarchicalLatentReference"
    # model is a function that calculates the function value f(x, theta), as well as a
    #  standard deviation sigma = g(x, theta) or dispersion coefficient, depending on likelihood model:
    e["Problem"]["Computational Model"] = lambda sample: model(sample)
    e["Problem"]["Likelihood Model"] = "Additive Normal"
    e["Problem"]["Reference Data"] = # insert list of lists of reference evaluations
    e["Problem"]["Data Points"] = # insert list of lists of data points (x values) or omit

Please refer to the corresponding example for further explanation and complete usage examples.
