

import load_data
import utils

import numpy as np
import pdb



# Model 3:
#  See section 9.2.5 in Lavielle, 'Mixed effect models for the population approach'



class ExponentialFamilyDistribution():
    def __init__(self):
        self._p = None
        self.sufficientStatisticsDimension = None

    def S(self, sample):
        raise NotImplementedError
    def zeta(self, sample):
        raise NotImplementedError
    def phi(self, sample):
        raise NotImplementedError




class ExampleDistribution3(ExponentialFamilyDistribution):
    ''' See section 9.2.5 in Lavielle, 'Mixed effect models for the population approach'

        In the book's notation (theta <-> psi, yi <-> xi ):
        log(p) = -N * log(2*pi*sigma*omega)  + sum_i {- (y_i - psi_i)**2 / (2*sigma**2) - (psi_i - theta) / (2*omega**2) }
        => choose:
            zeta(theta) = log(2 * pi * sigma * omega) + sum_i[yi**2 / (2 * sigma**2)] - N*theta**2
            S(psi)      = ( sum_i[psi_i**2], sum_i[psi_i], sum_i[yi*psi_i] )
            phi(psi)    = ( -1/2*(1/sigma**2 + 1/omega**2), theta/omega**2, 1/sigma**2 )
    '''

    def __init__(self):
        self._p = load_data.SimplePopulationData()
        self.sufficientStatisticsDimension = 3
        self.cur_hyperparameter = 0
        self.numberHyperparameters = 1

        if isinstance(self._p.data, np.ndarray):
            self._p.data = self._p.data.flatten()
        assert self.numberLatent == len(self._p.data)

    # In case we reset the underlying data, we cannot directly define these variables but need to use properties
    # (so that these variables change whenever the data, self._p, is changed)
    @property
    def numberLatent(self):
      return self._p.nIndividuals

    @property
    def N(self):
      return self._p.nIndividuals

    @property
    def a(self):
      ''' alpha'''
      return self._p.sigma ** 2 / (self._p.sigma ** 2 + self._p.omega ** 2)

    @property
    def gamma(self):
      return np.sqrt(1 / (1 / self._p.sigma ** 2 + 1 / self._p.omega ** 2))

    def sampleLatent(self, k):
        ''' probability to sample from:
              p(x, z | theta) = N(a*theta + (1-a)*x_i, gamma**2) '''

        hyperparameters = k["Hyperparameters"]
        theta = hyperparameters[0]
        numberSamples = k["Number Samples"]  # how many samples per individuum; we sample the psi_i
        if (k["Number Of Latent Variables"] != self.numberLatent):
            raise ValueError(
                "Implementation error, number of latent variables at initialization does not fit to what was passed as variable")

        samples = np.zeros((self._p.nIndividuals, numberSamples))
        for i in range(self._p.nIndividuals):
            mean = self.a * theta + (1 - self.a) * self._p.data[i]
            sdev = self.gamma
            samples[i][:] = np.random.normal(mean, sdev, size=(numberSamples,))

        samples = samples.transpose()
        k["Samples"] = samples.tolist()


    def S(self, sample):
        ''' S(psi) = (sum_i[psi_i ** 2], sum_i[psi_i], sum_i[yi * psi_i])  '''
        individual_latent_vars = np.array(sample["Latent Variables"])
        assert len(individual_latent_vars) == self._p.nIndividuals
        sample["S"] = [ np.sum(individual_latent_vars**2),
                        np.sum(individual_latent_vars),
                        np.dot(individual_latent_vars , self._p.data )  ]


    def zeta(self, sample):
        '''  zeta(theta) = log(2 * pi * sigma * omega) + sum_i[yi**2 / (2 * sigma**2)] - N*theta**2  '''
        hyperparams = sample["Hyperparameters"]
        hyperparam = hyperparams[0]
        sample["zeta"] = self.N * np.log(2 * np.pi * self._p.sigma * self._p.omega) + \
                           np.sum(self._p.data **2) / (2 * self._p.sigma**2) + \
                           self.N * hyperparam **2 / (2 * self._p.omega**2)


    def phi(self, sample):
        '''  phi(psi)    = ( -1/2*(1/sigma**2 + 1/omega**2), theta/omega**2, 1/sigma**2 ) '''
        hyperparams = sample["Hyperparameters"]
        hyperparam = hyperparams[0]
        sample["phi"] = [-0.5 * (1 / self._p.sigma ** 2 + 1 / self._p.omega **2),
                         hyperparam / self._p.omega ** 2,
                         1. / self._p.sigma ** 2]

    def joint_probability(self, sample):
        ''' for sampling via mcmc; given sample["Parameters"] -> (psi_i)_i (psi as in the book), and the current
                hyperparameter, cur_hyperparameter, return the joint data probability. (Edit: need to return log-llh?)'''
        pass # TODO

    # Todo remove:
        # /*Model 2:
        #     Assume we have two gaussian distributions with peaks around two means, and identical
        #     covariance = sigma.
        #     Hyperparameters:
        #         mu1, mu2 and sigma (in this order)
        #     Latent variables:
        #         Assignment of each data point to the modes
        #
        #     So,
        #       log(p) = sum_i [log(2*pi*sigma)*dim/2] - sum_i [  |x_i|^2  - <2*mu_c(i), x_i>  +  |mu_c(i)|^2  ]/(2*sigma^2)
        #
        #       ->
        #          zeta(sigma, mu1, mu2) = N * dim * log(2*pi*sigma)*1/2
        #          S(x1, c(1), ... xN, c(N))
        #                     = sum_i [vec(-|x_i|^2, delta(ci=1), delta(ci=2), x_i * delta(ci=1), x_i * delta(ci=2) ) ]
        #          phi(sigma, mu1, mu2)
        #                     = vec(1, -|mu1|^2, -|mu2|^2, 2 * mu1^T, 2 * mu2^T) * 1/(2*sigma)
        #
        # */


class ConditionalDistribution4():
    ''' Same hierarchical model as above, but here we only know the part p(data | latent), which is a product
        of Gaussians:
        #
        #  Model 3:
        #    draw psi_i ~ N(theta, omega**2)
        #    draw x_i ~ N(psi_i, sigma**2)
        '''

    def __init__(self):
        self._p = load_data.SimplePopulationData()

    def conditional_p(self, sample):

        latent_vars = sample["Latent Variables"]
        data = sample["Data Point"]
        assert len(latent_vars) == 1 #self._p.nIndividuals, "we assume each individual has only one measurement and have their own latent variable"
        sigma = self._p.sigma

        # log(p(data | mean, sigma ))
        logp = 0
        #for i in range(self._p.nIndividuals):
            # pt = self._p.data[i]
            # assert pt == data[i]
        mean = latent_vars
        pdb.set_trace()
        logp = np.log(utils.univariate_gaussian_probability(mean, sigma, data))

        sample["Conditional LogLikelihood"] = logp


