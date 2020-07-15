import numpy as np
import korali


class Example925Sampler:
  ''' See section 9.2.5 in Lavielle, 'Mixed effect models for the population approach' '''

  def __init__(self, sigma, omega, N, data):
    self.sigma = sigma
    self.omega = omega
    self.N = N  # number of individuals
    self.a = sigma**2 / (sigma**2 + omega**2)
    self.gamma = np.sqrt(1 / (1 / sigma**2 + 1 / omega**2))
    self.numberLatent = N
    self.numberHyperparameters = 1

    self.data = data

    assert self.numberLatent == len(self.data)
    assert type(self.data) == list or (self.numberLatent == len(
        self.data.flatten()))

  def sampleLatent(self, k):
    # /*
    #  * probability to sample from:
    #  * p(x, z | theta) = N(a*theta + (1-a)*x_i, gamma**2)
    #

    hyperparameters = k["Hyperparameters"]
    theta = hyperparameters[0]
    numberSamples = k[
        "Number Samples"]  #  how many samples per individuum; we sample the psi_i
    if (k["Number Of Latent Variables"] != self.numberLatent):
      raise ValueError(
          "Implementation error, number of latent variables at initialization does not fit to what was passed as variable"
      )

    samples = np.zeros((self.N, numberSamples))
    for i in range(self.N):
      mean = self.a * theta + (1 - self.a) * self.data[i]
      sdev = self.gamma
      samples[i][:] = np.random.normal(mean, sdev, size=(numberSamples,))

    samples = samples.transpose()
    k["Samples"] = samples.tolist()
