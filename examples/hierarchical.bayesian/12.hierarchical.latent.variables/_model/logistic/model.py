

import load_data
import ..utils

import numpy as np

def logisticModel(x, theta):
    assert len(theta) >= 3
    # logistic function
    f = np.exp(theta[2] * x)
    y = (theta[0] * theta[1] * f) / (theta[0] + theta[1] * (f - 1.))

    if np.isinf(y) or np.isnan(y):
        # set inf or nan to something large. otherwise nlmefitsa complains -- not sure if necessary in python, too. we'll see.
        y = 1e300
    return y


class LogisticConditionalDistribution():
    ''' Model 7:
        Data generation process: yi = f(xi, theta) + eps,
                where eps ~ N(0, alpha) (alpha is the standard deviation and is known from the model.)
            Everything is one-dimensional.

    '''

    def __init__(self):
        self._p = load_data.LogisticData()



    def conditional_p(self, sample):

        latent_vars = sample["Latent Variables"]
        dataPoint = sample["Data Point"]

        assert len(dataPoint) == 2
        x = dataPoint[0]
        y = dataPoint[1]
        assert len(latent_vars) == 1
        fx = logisticModel(x, latent_vars[0])
        eps = 1e-10
        if self._p.error_model == "constant":
            err = (y - fx) ** 2
            det = self.alpha ** 2
        elif self._p.error_model == "proportional":
            y2 = max(y ** 2, eps)
            err = (y - fx) ** 2 / y2
            det = self.alpha ** 2 * y2
        else:
            raise ValueError(f"Unknown error model: {self._p.error_model}")

        if np.isinf(err) or np.isnan(err):
            sample["Conditional LogLikelihood"] = -1.e200
        else:
            log2pi = 0.5 * np.log(2 * np.pi)
            logp = -log2pi - 0.5 * np.log(det) - 0.5 * err / self.alpha ** 2
            sample["Conditional LogLikelihood"] = logp