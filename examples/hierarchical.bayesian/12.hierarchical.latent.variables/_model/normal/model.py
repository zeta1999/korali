import numpy as np
import sys, os


scriptdir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(scriptdir, '../'))
import utils
import load_data



def normalModel(x, theta):
    y = theta**2
    if np.any(np.isinf(y) | np.isnan(y)):
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
        assert len(dataPoint) == 3, "Expected id, x and y values as 'data point'"
        x = dataPoint[1]
        y = dataPoint[2]
        assert len(latent_vars) == self._p.nLatentSpaceDimensions == 2, f"Latent variable vector has wrong length. " \
                                                                        f"Was: {len(latent_vars)}, should be: {2}"
        fx = normalModel(x, latent_vars[0])
        sigma2 = latent_vars[-1]**2
        eps = 1e-10
        if sample["Current Generation"] > 200:
            import pdb
            pdb.set_trace()
        if self._p.error_model == "constant":
            err = (y - fx) ** 2
            det = sigma2
        elif self._p.error_model == "proportional":
            y2 = max(y ** 2, eps)
            err = (y - fx) ** 2 / y2
            det = sigma2 * y2
        else:
            raise ValueError(f"Unknown error model: {self._p.error_model}")

        if np.isinf(err) or np.isnan(err):
            sample["Conditional LogLikelihood"] = -1.e200
        else:
            log2pi = 0.5 * np.log(2 * np.pi)
            logp = -log2pi - 0.5 * np.log(det) - 0.5 * err / sigma2
            sample["Conditional LogLikelihood"] = logp

        # Relevant part in Matlab code:
        '''    # We have 2 x a set of parameters here it seems:
                First, theta, the model parameters for f(x, theta) 
                Second, the standard deviation of the noise around f(x, theta) -- here, one-dimensional.
         
                theta = model.par_transf( theta ); # the inverse transfomr, z -> latent, This is already automated by Korali
                error = z(par.N,:); # N = len(beta), seems to be the latent space dimensions.
                                    # What's the second dimension index for in z?
                                    # Seems it's the number of individuals. 
                error = model.err_par_transf( error ); # --> lognormal "error"=covariance; so  
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
