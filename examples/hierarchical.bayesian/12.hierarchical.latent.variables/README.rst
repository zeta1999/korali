===============================================================================
12 - Optimizing parameters in a hierarchical latent variable model, using SA-EM
===============================================================================

Here, we optimize hierarchical latent variable models. As in tutorial 11, we aim to find the parameters for a
probability distribution, that maximize the likelihood of a set of observed points :math:`X = \{x_i\}_i`,
:math:`x_i \in \mathbb{R}^n`. We first use the standard version of the `SA-EM` solver, and then show the use of
specialized `SA-EM` for hierarchical models (`SAEMh`).

As hierarchical model, we consider a probability distribution of the form (following [1]):


.. math::
    p(x_i, \theta_i | \psi, c_i )  = p( x_i | \theta_i ) \cdot p( \theta_i | \psi, c_i )

Here, each i indexes one 'individuum' from a population, and the optional parameters :math:`c_i` represent individual
covariates. Multiple datapoints might exist for a single individuum. (Again, data points and covariates will be absorbed
into the model, which needs to be taken care of by the user during model definition.)

We assume the latent variables :math:`\theta_i` can be transformed
into (or originate from) normally distributed variables :math:`z_i = h(\theta_i)`, i.e.

.. math::

    y_i \; \sim \;  p(y_i | z_i)  \\
    z_i \; \sim \; \mathcal{N}(\beta, \Omega)

To this end, the user can choose between latent variables :math:`\theta_i` following a normal, log-normal or logit-normal
distribution. Log-normal means :math:`log(\theta_i) \; \sim \; \mathcal{N}(a, b)`, and logit-normal similarly means
:math:`logit(\theta_i) \; \sim \; \mathcal{N}(a, b)`, where :math:`logit(\theta) = log\left({\theta}/{(1 - \theta)}\right)`.
Normal and log-normal distributions describe distributions in many different disciplines `[2] <https://stat.ethz.ch/~stahel/lognormal/bioscience.pdf>`_.

1. **Simple population example:**
   We first follow the simple example of **section 9.2.5 of Lavielle's book [1]**. All distributions are normal which makes
   sampling distribution and optimization analytically solvable [1].
   The model is:

   .. math::

      x_i \; \sim \; \mathcal{N}(\theta_i, \sigma^2) \; \text{for} \; 1 \leq i \leq N  \\
      \theta_i \; \sim \; \mathcal{N}(\psi, \omega)

   We have one sample per "individuum" i. Note that our notation is reverted to that of the book, we use :math:`\theta`
   for latent variables and :math:`\psi` for hyperparameters.


2. **TODO: Some other example:**
   We have a mixture of Gaussians, with :math:`C` clusters with different means :math:`\mu_c \in \mathbb{R}^n` and equal covariance  :math:`\sigma^2\mathbb{I}_n`,  :math:`\sigma \in \mathbb{R}` . That is, given cluster assignments :math:`c_i` ( :math:`x_i` belongs to cluster :math:`c_i \in \{1, ... C\}`), the the joint probability is:
   :math:`p(X | c_1, ... c_N, \mu_1, ... \mu_C, \sigma) = \prod_i  \frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(x_i-\mu_{c_i})^2}{2\sigma^2}}`.
   Here, latent variables are the assignments :math:`c_i \in \{1, ... C\}, i \in {1, ..., N}` where :math:`N` is the number of data points. As hyperparameters we have :math:`C` means :math:`\mu_c`, and a single :math:`\sigma \in \mathbb{R}` defining the covariance.



Code files - TODO everything below this
---------------------------------------

- :code:`run-saem-population-simple.py`: Contain the main code, defining the experiment to run.
- :code:`model/model.py` :  Defines the probability distributions, in exponential form

NOT: - :code:`model/tutorial_samplers.py` :  Implements sampling of latent variables from the distributions defined in :code:`model/model.py`.

- :code:`model/load_data.py` :  Functionality to read the data points from :code:`data_single.in` and :code:`data_multiple.in` . Data points need to be considered in both sampling and when defining the probability distributions.

-----------------------------------

- :code:`model/generate_data.py` :  Run this to re-generate data points. Writes points sampled from a single Gaussian into :code:`data_single.in` and points sampled from a multi-modal Gaussian into :code:`data_multiple.in`. You can change the dimension, number of clusters, number of points, mean vectors and standard deviation.
- :code:`model/test_model.py` :  For completeness: Tests that ensure correctness of the distributions defined in :code:`model/model.py`.




How to run the example
----------------------

In a shell from this directory (inside :code:`12.hierarchical.latent.variables`), type
.. code-block::

    $ python3 run-saem-population-simple.py

to run the example.


Code explanation
----------------

.. code-block:: python

   ...

Computational Model
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   ...


The Sampler
~~~~~~~~~~~~

.. code-block:: python

   ...

Defining the Experiment
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   ...

The Solver
~~~~~~~~~~


Variables and Prior Distributions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Running the Optimization
~~~~~~~~~~~~~~~~~~~~~~~~



References
==========

[1] Lavielle, Marc. Mixed effects models for the population approach: models, tasks, methods and tools. CRC press, 2014.

`[2] <https://stat.ethz.ch/~stahel/lognormal/bioscience.pdf>`_ Limpert, Stahel, Abbt. Log-normal Distributions across the Sciences: Keys and Clues. BioScience May 2001.

