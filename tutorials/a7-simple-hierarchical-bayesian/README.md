# A.7 - Simple Hierarchical Bayesian Inference

In this tutorial we show how to do hierarchical Bayesian inference.


## Problem Setup

In a hierarchical Bayes problem, a "second layer of randomness" is added to the
problem formulation: We assume that the parameters of the probability distribution
of the data are itself random variables, distributed according to another (possibly
  parameterized) probability distribution.  

That is, we have a parameterized data distribution `data ~ p(X; V)` with parameters `V`,
where $V ~ p(V; \psi)$ depends on hyperparameters $\psi$. This leads to a
'hierarchy'  

 $$\psi \rightarrow V \rightarrow X$$.  

 Note: The parameters, here `V`, are also called `Theta` in multiple locations.  

For example, we
might be sampling values from different individuals from a population. Then,
parameters `V` might be different for each individual, but distributed according
to a common population-wide distribution parameterized by $\psi$.  

Our task now is to ... TODO

TODO: Find out what role the different datasets take. Different individuals?  


**Hierarchical Bayesian Inference is set up in 3 phases:**
 - sample the posterior distributions conditioned on each data set
    - ?? the posterior distribution of the parameters `V`? ??
 - sample the hyper parameter
 - sample the posterior given hyperparameter and one (a) data set or  (b) data sets combined.

For each phase we provided an individual `python` file.

In phase 0 we generate synthetic data.

## Phase 0
In [phase 0](phase0.py), we define a problem of type `Execution/Model` as follows:

```python
e["Problem"]["Type"] = "Execution/Model"
e["Problem"]["Execution Model"] = lambda modelData: normal_rnds(modelData, Ns)

e["Variables"][0]["Name"] = "V1"
e["Variables"][1]["Name"] = "V2"
e["Variables"][0]["Loaded Values"] = np.random.normal( 10, 4, N ).tolist()
e["Variables"][1]["Loaded Values"] = np.random.lognormal( 0, 1, N ).tolist()

e["Solver"]["Type"] = "Executor"
e["Solver"]["Executions Per Generation"] = 1
```
What we pass here as `Loaded Values` will later become different values for
two different parameters, `V1` and `V2`. Samples for `V1` are drawn from a [random normal](https://docs.scipy.org/doc/numpy-1.15.0/reference/generated/numpy.random.normal.html)
distribution with a mean of 10 and a standard deviation ("sd") of 4. Samples for `V2` are drawn
from a [log-normal](https://docs.scipy.org/doc/numpy-1.15.1/reference/generated/numpy.random.lognormal.html) distribution where `log(V2)` has mean 0 and sd 1.

When this problem is run by Korali, the execution model `normal_rnds` will be
evaluated a number of times
<!-- `??how often, and is this right?? Question: What
defines "Max Generations" of the solver? It seems to be set to max()... ??`  -->
for each pair of parameter values we
passed to it, to generate one dataset per pair. The function `normal_rnds`,
defined in [model.py](setup/model/model.py), takes two parameters: A `Sample`
object `s`, which will contain one value for each of the `Variables` we defined
(`V1` and `V2`) as `Parameters`, and the number of data points to generate, `Ns`.
We [see](setup/model/model.py) that the data is then generated according to a normal distribution with mean `th1 := V1` and sd `th2 := V2`:
 ```python
  y = np.random.normal( th1, th2, Ns )
 ```
`normal_rnds` directly stores the data to disk.



## Phase 1: Sampling the Parameters Theta (our `V`)
In [phase 1](phase1.py), we run 5 experiments; one for each dataset created in
phase 0. We set the problem type to `Evaluation/Bayesian/Inference/Approximate`,
the likelihood model to `Normal` and use as solver `Sampler/TMCMC`,

```python
 e["Problem"]["Type"] = "Evaluation/Bayesian/Inference/Approximate"
 e["Problem"]["Likelihood Model"] = "Normal"
 # ...
 e["Solver"]["Type"] = "Sampler/TMCMC"
```

`??Probably, this means that we determine approximate parameter values, assuming
that each dataset is normally distributed. So, we would sample mean $\mu$ and
sd $\sigma$ via Transitional Markov Chain Monte Carlo (TMCMC) sampling, multiple
times for each dataset.  ??`  

?? TODO: What does this do? (Why) do we sample instead of finding the parameters
that maximize the data likelihood? Aha, maybe we sample so that we have multiple
parameter-datapoints in the next step to do maximum LLH estimation *there* instead?   

We need to pass `Reference Data` points to Korali,
```python
 e["Problem"]["Reference Data"] = getReferenceData("setup/data/",i);
```
\- this will load the values we generated `~ $\mathcal{N}(V1, V2)$` in phase 0.
The likelihood of those points
is a function of the parameters we want to sample (the parameters resulting in
a higher likelihood are more likely to be correct; TMCMC will use this to
generate samples.

After running `phase1.py`, samples will be stored in folder `setup/results_phase_1`.



## Phase 2: Sampling Hyperparameters Psi -- TODO: Reformulate

In [phase 2](phase2.py), we generate samples of the hyperparameters $\psi$,
given the samples of parameters `V` from phase 1.  

We have four hyperparameters $\psi$, or `Psi`:  
When generating the datasets in pahse 0, `V1` were drawn from a normal
 distribution with mean `Psi 1 == 10` and sd `Psi 2 == 4`;
`V2` was log-normally distributed with `log(V2)` having mean `Psi 3 == 0` and sd
`Psi 4 == 1`. (Comparing to the range of uniform priors of each variable,
we see that range each includes the "true" hyperparameter. For example `Psi 1`: 4 lies
in [0,20]. We guessed our priors well, didn't we!  )  

-- Has subproblems

-- Todo: continue reformulating below here

<!-- psi1, psi2, psi3, psi4: mean and sd of the normal and lognormal distributions, so
psi-i are the hyperparameters to be estimated; `V2` had a log-normal distribution.

Interesting priors for the psi's:
psi1: actual: 10 prior: [0, 20]
psi2: actual: 4 prior:  [0, 20]
psi3: actual 0, prior: [-2, +2]
psi4: actual 1, prior:  [0,  5]
okay, works
-->

We tell Korali our models for the `Conditional Priors`, which ??are the distributions
of our parameters depending on hyperparameters??. We have two conditional priors:
One for the mean `V1`, which we know is normally distributed, and one for the sd `V2`,
with a log-normal distribution:

```python
e["Problem"]["Conditional Priors"] = [ "Conditional 0", "Conditional 1" ]

e["Distributions"][0]["Name"] = "Conditional 0"
e["Distributions"][0]["Type"] = "Univariate/Normal"
e["Distributions"][0]["Mean"] = "Psi 1"
e["Distributions"][0]["Standard Deviation"] = "Psi 2"

e["Distributions"][1]["Name"] = "Conditional 1"
e["Distributions"][1]["Type"] = "Univariate/LogNormal"
e["Distributions"][1]["Mu"]    = "Psi 3"
e["Distributions"][1]["Sigma"] = "Psi 4"
```

"Conditional priors" are the two distributions for `V1` and `V2`, so the psi's
define the *prior* distribution of V only.
I guess "conditional priors" means $p(V | \psi)$, so the probability of the
parameters, given the hyperparameters.

Now we sample, and what do we sample? We sample what is "Variable", so the
psi-i.

Probably the problem type  `Evaluation/Bayesian/Hierarchical/Psi` has an internal
model of the form   
$$ p(data, V, \psi) = p(data | V, \psi) * p(V | \psi) * p_{prior}(\psi) $$
--> and then approximate:
$$ p(\psi | data) ~= p(\psi | samples of V) ~ p(samples of V | \psi) * p_{prior}(\psi) $$
This is where we need the "conditional priors", $p(V | \psi)$.

We use `Sampler/TMCMC` to get samples of parameters `Psi 1` to `Psi 4`, which
will be stored to files in folder `setup/results_phase_2/`.

```python
```

```python
```

```python
```


## Phase 3a: "ThetaNew"
In [phase 3 a](phase3a.py), we again sample parameters `Theta` (our `V`) a
 second time. But this time, sampling is based entirely on our samples of the four `Psi`
 variables generated in phase 2, and uniform priors for `V`.
 ```python
  e["Problem"]["Type"]  = "Evaluation/Bayesian/Hierarchical/ThetaNew"
  e["Problem"]["Psi Problem Path"] = 'setup/results_phase_2'
  # ...
  e["Solver"]["Type"] = "Sampler/TMCMC"
 ```

 The purpose of doing this ?? is resampling new datasets for simulation.
 For example, if each of our datasets corresponded to one individual, we now simulate
 additional individuals, given our inferred population hyperparameters. ??

`?? There might be additional special treatment for this case, because
 otherwise we would not need the special problem type
  `Evaluation/Bayesian/Hierarchical/ThetaNew` . What distribution
 is sampling based on? The conditional priors were normal and log-normal before,
 we don't seem to be using this information now. ??`

<!-- Problem type: `Evaluation/Bayesian/Hierarchical/ThetaNew`
From this, I'd infer we want to estimate theta, or `V`, a second time now.-->

We pass as `Psi Problem Path` the path to the samples of the four `Psi`
variables generated in phase 2.

Since we again want to estimate (sample, that is) the parameters `V`, we define
two variables `mu` (`V1`) and `sigma` (`V2`),

```python
e["Variables"][0]["Name"] = "mu"
e["Variables"][1]["Name"] = "sigma"

e["Variables"][0]["Prior Distribution"] = "Uniform 0"
e["Variables"][1]["Prior Distribution"] = "Uniform 1"
```

Uniform prior distributions of `mu` and `sigma` are a bit wider here, since
their true values are random; `mu` varying around 10, and `sigma` around 1 (log(sigma) was distributed around 0 = log(1)).  

Running `phase3a.py` will output mean (and covariance) of sampled values for `mu`
and `sigma`.

?? Statistics of the TMCMC runs are stored in folder `setup/results_phase_3a/`. ??


## Phase 3b: "Theta"

[Phase 3 b](phase3b.py) combines sampled `Phi` hyperparameter values from phase
2 with our samples of `V` (that is, `mu` and `sigma`) generated in phase 1.

This is a problem of type `Evaluation/Bayesian/Hierarchical/Theta`, the
same as in phase ... Ah, no, we didn't have that one yet.
"Theta" --> This step will also try to sample Theta. But how?

<!--"Theta Problem Path" comes from phase 1
"Psi Problem Path" comes from phase 2. What do those two do?
Also, "Theta" accepts a "Psi" problem.
(Psi) problems require at least one conditional prior"
"(Psi) problem requires defining at least two executed sub-problems "
sub-problems need to be of type 'Evaluation/Bayesian/Inference'
(psi estimates the hyperparameters.) -->

????? What does phase 3b do? ?????

-----------------------------

## C++ Code: Phase 3a "ThetaNew"
- Takes as input a Psi Problem Engine (from phase 2), loaded from disk

Three main functions:  
```C++
void evaluateLogLikelihood(korali::Sample& sample) override;
void evaluateThetaLikelihood(korali::Sample& sample);
void initialize() override;
```
**Open questions**:  
- Does this use the fact that we know the form of p(theta | psi)?
These are called "conditional priors" it seems.
- How can we use these three functions to generate samples? Sampler `TMCMC` might
be accessing the function `evaluateLogLikelihood`. Checked: No, it doesnt use this.
Nor evaluateThetaLikelihood.


What `initialize()` does:
- initialize a "Hierarchical Bayes" problem (the "main" problem?)
- initializes a "Psi Problem Engine", which must have type "Psi"
- the Psi Problem Engine has to have one conditional prior for each variable (
  variables here are Theta / `V`)
  ...


## C++ Code: Phase 3b "Theta"
```c++
// Theta Problem's "Evaluate Log-Likelihood"
void korali::problem::evaluation::bayesian::hierarchical::Theta::evaluateLogLikelihood(korali::Sample& sample)
{
 double logLikelihood = 0.0;

 std::vector<double> logValues;
 logValues.resize(_psiProblemSampleCount);

/* We use all samples of Psi.
   In initialize(), we loaded the complete Problem objects from phases 1 () and 2 () */
 for (size_t i = 0; i < _psiProblemSampleCount; i++)
 {
   korali::Sample psiSample;
   psiSample["Parameters"] = _psiProblemSampleCoordinates[i];

   _psiProblem->updateConditionalPriors(psiSample);

   double logConditionalPrior = 0.;
   for (size_t k = 0; k < _thetaVariableCount; k++)
     logConditionalPrior += _psiProblemEngine._distributions[_psiProblem->_conditionalPriorIndexes[k]]->getLogDensity(sample["Parameters"][k]);

   logValues[i] = logConditionalPrior - _precomputedLogDenominator[i];
 }

 sample["logLikelihood"] = -log(_psiProblemSampleCount) + logSumExp(logValues);
}
```

```python
```


###  Running

All 3 phases can be run with the shell script

```bash
./run-hierarchical.sh
```

The results are saved in sub dirs of the folder `/setup`.


## References

TMCMC paper: [Ching2007](https://ascelibrary.org/doi/abs/10.1061/%28ASCE%290733-9399%282007%29133%3A7%28816%29)
[Slides on TMCMC](https://www.cse-lab.ethz.ch/wp-content/uploads/2014/09/HPCSE_II-week7b.pdf)
[Readme in Korali describing TMCMC](../../source/solver/sampler/TMCMC/README.md)
