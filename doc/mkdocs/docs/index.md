# Welcome to Korali

Korali is a high performance computing framework for optimization, sampling and Bayesian uncertainty quantification of large scale computational models.

Korali is based on [UPCXX](https://bitbucket.org/berkeleylab/upcxx/wiki/Home)

<br><br>


# What Korali does for you

1. **Optimize**: given a cost function $F(\vartheta)$ find
	$$
	\vartheta^\star = \mathop{\arg\min}\limits_{\vartheta}  \,\,\, F(\vartheta) \,.
	$$

2. **Sample**: given the density of a probability distribution $p_{\vartheta}$ draw samples,
	$$
		\vartheta^{(k)} \sim p_\vartheta, \quad k=1,\ldots,N_s \, .
	$$

3. **Uncertainty Quantification**: given a set of data $d$, the output of the model $f(x;\vartheta)$ a likelihood function $p(d|\vartheta)$ and a prior probablity density $p(\vartheta)$ sample the posterior distribution,
	$$
	p(\vartheta | d) = \frac{p(d | \vartheta) \, p(\vartheta)}{p(d)}\, .
	$$
The model output $f$ depends on a set of input parameters $x$.

After [installing](installation.md) the software, please have a look at the [examples](./examples/sampling.md) and learn how to run your own code.





<br><br><br><br><br><br>

!!! warning
    The software and the documentation page are under continuous development. New pages and new feature will be constantly added.
