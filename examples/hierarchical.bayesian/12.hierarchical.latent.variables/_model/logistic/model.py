

import load_data
import ..utils

import numpy as np



def normalModel(x, theta):
    y = theta**2
    if np.isinf(y) or np.isnan(y):
        y = 1e300
    return y

def logisticModel(x, theta):
    assert len(theta) >= 3
    # logistic function
    f = np.exp(theta[2] * x)
    y = (theta[0] * theta[1] * f) / (theta[0] + theta[1] * (f - 1.))

    if np.isinf(y) or np.isnan(y):
        # set inf or nan to something large. otherwise nlmefitsa complains -- not sure if necessary in python, too. we'll see.
        y = 1e300
    return y


class NormalConditionalDistribution():
    ''' Model 6:
        Data generation process: yi = f(xi, theta) + eps,
                where eps ~ N(0, alpha) (alpha is the standard deviation and is known from the model.)
            Everything is one-dimensional.

    '''

    def __init__(self):
        self._p = load_data.NormalData()


    def conditional_p(self, sample):

        latent_vars = sample["Latent Variables"]
        dataPoint = sample["Data Point"]
        assert len(dataPoint) == 2
        x = dataPoint[0]
        y = dataPoint[1]
        assert len(latent_vars) == 1
        fx = normalModel(dataPoint, latent_vars[0])
        eps = 1e-10
        if self._p.error_model == "constant":
            err = (y - fx)**2
            det = self.alpha**2
        elif self._p.error_model == "proportional":
            y2 = max(y**2, eps )
            err = (y - fx)**2 / y2
            det = self.alpha**2 * y2
        else:
            raise ValueError(f"Unknown error model: {self._p.error_model}")

        if np.isinf(err) or np.isnan(err):
            sample["Conditional LogLikelihood"] =  -1.e200
        else:
            log2pi = 0.5 * np.log(2 * np.pi)
            logp = -log2pi - 0.5 * np.log(det) - 0.5 * err / self.alpha**2
            sample["Conditional LogLikelihood"] = logp
        # Relevant part in Matlab code:
        '''     theta = model.par_transf( theta ); # the inverse transfomr, z -> latent, This is already automated by Korali
                error = par.alpha * ones(1,data.Ni);
                error = model.err_par_transf( error );
                sigma2 = error(k)^2;
                
                ind = ( data.id == data.uid(k) ); 
                y = model.fun( theta(:,k), data.x(ind) );
                
            switch model.error_model
                case 'constant'
                    sserr(k) = sum( (y-data.y(ind)).^2 );
                    tmp = sigma2*ones(1,data.Ndi(k));
                case 'proportional'
                    y2 = max(eps,y.^2);
                    sserr(k) = sum(  ((y-data.y(ind)).^2) ./ y2   );
                    tmp = sigma2*y2;
                otherwise
                    error('Unknown error model')
            end
            
            if( isfinite(sserr(k)) )
                ll(k) = -data.Ndi(k)*log2pi - 0.5*sum(log(tmp)) - 0.5*sserr(k)/sigma2;   # THe last part is probably an implicit matrix inversion. Or elementwise division?
                            \_ the constant    \_ the determinant       \_ the error part
            else
                ll(k) = -1e200;
            end
        '''




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