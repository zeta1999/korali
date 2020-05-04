

import load_data
import numpy as np


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

        self.N = self._p.nIndividuals
        self.a = self._p.sigma ** 2 / (self._p.sigma ** 2 + self._p.omega ** 2) # alpha
        self.gamma = np.sqrt(1 / (1 / self._p.sigma ** 2 + 1 / self._p.omega ** 2))
        self.numberLatent = self.N
        self.numberHyperparameters = 1

        assert self.numberLatent == len(self._p.data)
        assert type(self._p.data) == list or (self.numberLatent == len(self._p.data.flatten()))

    def sampleLatent(self, k):
        # /*
        #  * probability to sample from:
        #  * p(x, z | theta) = N(a*theta + (1-a)*x_i, gamma**2)
        #

        hyperparameters = k["Hyperparameters"]
        theta = hyperparameters[0]
        numberSamples = k["Number Samples"]  # how many samples per individuum; we sample the psi_i
        if (k["Number Of Latent Variables"] != self.numberLatent):
            raise ValueError(
                "Implementation error, number of latent variables at initialization does not fit to what was passed as variable")

        samples = np.zeros((self.N, numberSamples))
        for i in range(self.N):
            mean = self.a * theta + (1 - self.a) * self._p.data[i]
            sdev = self.gamma
            samples[i][:] = np.random.normal(mean, sdev, size=(numberSamples,))

        samples = samples.transpose()
        k["Samples"] = samples.tolist()


    def S(self, sample):
        ''' S(psi) = (sum_i[psi_i ** 2], sum_i[psi_i], sum_i[yi * psi_i])  '''
        individual_latent_vars = np.array(sample["Latent Variables"])
        assert len(individual_latent_vars) == self.N
        sample["S"] = [ np.sum(individual_latent_vars**2),
                        np.sum(individual_latent_vars),
                        np.dot(individual_latent_vars , self._p.data)  ]


    def zeta(self, sample):
        '''  zeta(theta) = log(2 * pi * sigma * omega) + sum_i[yi**2 / (2 * sigma**2)] - N*theta**2  '''
        hyperparams = sample["Hyperparameters"]
        hyperparam = hyperparams[0]
        sample["zeta"] = np.log(2 * np.pi * self._p.sigma * self._p.omega) + np.sum(self._p.data**2) / (2 * self._p.sigma**2) - \
                self.N * hyperparam **2


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


class ExampleDistribution2(ExponentialFamilyDistribution):

    def __init__(self):
        self._p = load_data.MultivariateData();
        self.sufficientStatisticsDimension = 1 + self._p.nClusters + self._p.nClusters * self._p.nDimensions;

    def S(self, sample):
        # /* For two clusters:
        #     S(x1, c(1), ... xN, c(N))
        #                     = sum_i [- vec(|x_i|^2, delta(ci=1), delta(ci=2), x_i * delta(ci=1), x_i * delta(ci=2) ) ]*/
        assignments = sample["Latent Variables"]
        if (len(assignments) != self._p.nPoints):
            raise ValueError("Latent variables should be exactly the cluster assignments, so there is one for each point in the sample.")

        S_dim = 1 + self._p.nDimensions * self._p.nClusters + self._p.nClusters
        S_vec = np.zeros((S_dim,), float)
        in_valid_range = True
        for a in assignments:
            if (a <= -0.5 or a >= self._p.nClusters - 0.51):
                in_valid_range = False
                S_vec[0] = np.array([-np.inf])
                sample["S"] = S_vec.tolist()
                return

        for i in range(self._p.nPoints):
            pt = self._p.points[i]
            S_vec[0] -= np.inner(pt, pt)
            cluster = int(np.round(assignments[i]))
            S_vec[cluster + 1] += 1

            mu_ci_location = self._p.nClusters + 1 + cluster * self._p.nDimensions
            S_vec[mu_ci_location : mu_ci_location + self._p.nDimensions] += pt
        sample["S"] = S_vec.tolist()


    def zeta(self, sample):
        hyperparams = sample["Hyperparameters"]
        if (len(hyperparams) != self._p.nDimensions * self._p.nClusters + 1):
            raise ValueError("Hyperparameters should be one mean vector per cluster, plus a 1D variable sigma. The dimension of the hyperparameter vector did not match this.")

        sigma = hyperparams[self._p.nClusters * self._p.nDimensions]

       # dim * N * log(sigma*sqrt(pi*2))
        sample["zeta"] = self._p.nDimensions * self._p.nPoints * np.log(sigma*np.sqrt(2*np.pi))


    def phi(self, sample):
        hyperparams = sample["Hyperparameters"]
        sigma = hyperparams[self._p.nClusters * self._p.nDimensions]

        if (len(hyperparams) != self._p.nDimensions * self._p.nClusters + 1):
            raise ValueError("Hyperparameters should be one mean vector per cluster, plus a 1D variable sigma. The dimension of the hyperparameter vector did not match this.")

        mus = []
        for i in range(self._p.nClusters):
            mu = hyperparams[i * self._p.nDimensions : (i + 1) * self._p.nDimensions]
            mus.append(mu)

          # /* For two variables:
          #       phi(sigma, mu1, mu2) = vec(1, -|mu1|^2, -|mu2|^2, mu1^T, mu2^T) * 1/(2*sigma)  */
        phi_dim = 1 + self._p.nDimensions * self._p.nClusters + self._p.nClusters
        phi = np.zeros((phi_dim,))
        phi[0] = 1.0
        for i in range(self._p.nClusters):
            phi[i + 1] = - np.inner(mus[i], mus[i])
            start_idx = 1 + self._p.nClusters + i * self._p.nDimensions
            phi[start_idx : start_idx + len(mus[0]) ] = np.array(mus[i]) * 2

        phi *= 1. / (2 * sigma ** 2)

        sample["phi"] = phi.tolist()
