import load_data
from _model import utils

import numpy as np


class ConditionalDistribution5():
  ''' Model 5:
     - multiple dimensions
     - multiple distribution types
     - latent variable coordinates are correlated
     - p(datapoint | latent) is still a normal distribution N(latent, sigma**2)
    '''

  def __init__(self):
    self._p = load_data.PopulationData()

  def conditional_p(self, sample):

    latent_vars = sample["Latent Variables"]
    dataPoint = sample["Data Point"]
    assert len(latent_vars) == self._p.nDimensions
    sigma = self._p.sigma

    logp = 0
    for dim in range(self._p.nDimensions):
      pt = dataPoint[dim]
      mean = latent_vars[dim]
      p = utils.univariate_gaussian_probability([mean], sigma, [pt])
      logp += np.log(p)

    sample["Conditional LogLikelihood"] = logp
