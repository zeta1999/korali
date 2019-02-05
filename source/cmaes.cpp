#include "cmaes.h"

Korali::KoraliCMAES::KoraliCMAES(size_t dim, double (*fun) (double*, int), size_t seed, MPI_Comm comm) : Korali::KoraliBase::KoraliBase(dim, fun, seed, comm)
{
	_stopFitnessEvalThreshold = std::numeric_limits<double>::min();
	_stopFitnessDiffThreshold = 1e-12;
	_stopFitnessDiffHistoryThreshold = 1e-13;
	_stopMinDeltaX = 0.0;
	_stopMaxStdDevXFactor = 1e+03;
	_stopMaxTimePerEigendecomposition = 1.0;
	_stopMinFitness = -std::numeric_limits<double>::max();

	_mu = 64;
	_muType = "Logarithmic";
	_muCovariance = -1;
	_diagonalCovarianceMatrixEvalFrequency = -1;
	_sigmaCumulationFactor = -1;
	_dampFactor = -1;
	_cumulativeCovariance = -1;
	_covarianceMatrixLearningRate = -1;
}



bool Korali::KoraliCMAES::cmaes_isFeasible(double *pop)
{
    for (int i = 0; i < _dimCount; i++)
    	if (pop[i] < _dims[i]._lowerBound || pop[i] > _dims[i]._upperBound) return false;
    return true;
}


void Korali::KoraliCMAES::Korali_InitializeInternalVariables()
{
    int i, j, N;
    double dtest, trace;

    // Initializing MU and its weights

  	_muWeights = new double[_mu];

    if (_muType == "LinearDecreasing") for (int i = 0; i < _mu; i++)  _muWeights[i] = _mu - i;
    if (_muType == "Equal")  for (int i = 0; i < _mu; i++)   _muWeights[i] = 1;
    if (_muType == "Logarithmic") for (int i = 0; i < _mu; i++)  _muWeights[i] = log(_mu+1.)-log(i+1.);

    /* normalize weights vector and set mueff */
    double s1 = 0.0;
    double s2 = 0.0;

    for (int i=0; i < _mu; i++)
    {
     s1 += _muWeights[i];
     s2 += _muWeights[i]*_muWeights[i];
    }

    _muEffective = s1*s1/s2;

    for (int i = 0; i < _mu; i++) _muWeights[i] /= s1;

    if(_mu < 1 || _mu > _lambda || (_mu == _lambda && _muWeights[0] == _muWeights[_mu-1]))
    { fprintf( stderr, "[Korali] Error: Invalid setting of Mu (%lu) and/or Lambda (%lu)\n", _mu, _lambda); exit(-1); }

    // Setting MU Covariance
    if (_muCovariance < 1) _muCovariance = _muEffective;

    // Checking Covariance Matrix Evaluation Frequency

    if  (_diagonalCovarianceMatrixEvalFrequency == -1)  _diagonalCovarianceMatrixEvalFrequency = 2 + 100. * _dimCount / sqrt((double)_lambda);
    if (_diagonalCovarianceMatrixEvalFrequency < 1)
    { fprintf( stderr, "[Korali] Error: Matrix covariance evaluation frequency is less than 1 (%lu)\n", _diagonalCovarianceMatrixEvalFrequency); exit(-1); }

    // Setting Sigma Cumulation Factor

    double tmpSigma = _sigmaCumulationFactor;
    if (tmpSigma > 0) _sigmaCumulationFactor *= (_muEffective + 2.0) / (_dimCount + _muEffective + 3.0);
    if (tmpSigma <= 0 || tmpSigma >= 1)  _sigmaCumulationFactor = (_muEffective + 2.) / (_dimCount + _muEffective + 3.0);

    // Setting Damping Factor

    if (_dampFactor < 0) _dampFactor = 1;
    _dampFactor = _dampFactor* (1 + 2*std::max(0.0, sqrt((_muEffective-1.0)/(_dimCount+1.0)) - 1))     /* basic factor */
        * std::max(0.3, 1. - (double)_dimCount / (1e-6+std::min(_maxGenerations, _maxFitnessEvaluations/_lambda)))
        + _sigmaCumulationFactor;                                                 /* minor increment */

    // Setting Cumulative Covariance

    if (_cumulativeCovariance <= 0 || _cumulativeCovariance> 1)  _cumulativeCovariance = 4. / (_dimCount + 4);

    // Set covariance Matrix Learning Rate

    double t1 = 2. / ((_dimCount+1.4142)*(_dimCount+1.4142));
    double t2 = (2.*_muEffective-1.) / ((_dimCount+2.)*(_dimCount+2.)+_muEffective);
    t2 = (t2 > 1) ? 1 : t2;
    t2 = (1./_muCovariance) * t1 + (1.-1./_muCovariance) * t2;

    double tmpCovarianceRate = _covarianceMatrixLearningRate;
    if (tmpCovarianceRate >= 0)   _covarianceMatrixLearningRate *= t2;
    if (tmpCovarianceRate < 0 || tmpCovarianceRate > 1)  _covarianceMatrixLearningRate = t2;

    // Setting cmaes_eigensystem evaluation Frequency
  	_covarianceEigensystemEvaluationFrequency = floor(1.0/(double)_covarianceMatrixLearningRate/((double)_dimCount)/10.0);

    N = _dimCount; /* for convenience */

    for (i = 0, trace = 0.; i < N; ++i)
        trace += _dims[i]._initialStdDev*_dims[i]._initialStdDev;
    sigma = sqrt(trace/N); /* _muEffective/(0.2*_muEffective+sqrt(N)) * sqrt(trace/N); */

    chiN = sqrt((double) N) * (1. - 1./(4.*N) + 1./(21.*N*N));
    flgEigensysIsUptodate = 1;

    for (dtest = 1.; dtest && dtest < 1.1 * dtest; dtest *= 2.)
        if (dtest == dtest + 1.)
            break;
    dMaxSignifKond = dtest / 1000.; /* not sure whether this is really save, 100 does not work well enough */

    gen = 0;
    countevals = 0;
    state = 0;

    rgpc = (double*) calloc (sizeof(double), N);
    rgps = (double*) calloc (sizeof(double), N);
    rgdTmp = (double*) calloc (sizeof(double), N+1);
    rgBDz = (double*) calloc (sizeof(double), N);
    rgxmean = (double*) calloc (sizeof(double), N+2); rgxmean[0] = N; ++rgxmean;
    rgxold = (double*) calloc (sizeof(double), N+2); rgxold[0] = N; ++rgxold;
    rgxbestever = (double*) calloc (sizeof(double), N+3); rgxbestever[0] = N; ++rgxbestever;
    rgout = (double*) calloc (sizeof(double), N+2); rgout[0] = N; ++rgout;
    rgD = (double*) calloc (sizeof(double), N);
    C = (double**) calloc (sizeof(double*), N);
    B = (double**)calloc (sizeof(double*), N);
    rgFuncValue = (double*) calloc (sizeof(double), _lambda+1);
    rgFuncValue[0]=_lambda; ++rgFuncValue;
    arFuncValueHist = (double*) calloc (sizeof(double), 10+(int)ceil(3.*10.*N/_lambda)+1);
    arFuncValueHist[0] = (double)(10+(int)ceil(3.*10.*N/_lambda));
    arFuncValueHist++;

    for (i = 0; i < N; ++i) {
        C[i] = (double*) calloc (sizeof(double), i+1);
        B[i] = (double*) calloc (sizeof(double), N);
    }
    index = (int *) calloc (sizeof(int*), _lambda);
    for (i = 0; i < _lambda; ++i)  index[i] = i; /* should not be necessary */
    curBest = (double *) calloc (sizeof(double), _lambda);

    /* Initialize newed space  */

    for (i = 0; i < N; ++i)
        for (j = 0; j < i; ++j)
            C[i][j] = B[i][j] = B[j][i] = 0.;

    for (i = 0; i < N; ++i)
    {
        B[i][i] = 1.;
        C[i][i] = rgD[i] = _dims[i]._initialStdDev * sqrt(N / trace);
        C[i][i] *= C[i][i];
        rgpc[i] = rgps[i] = 0.;
    }

    minEW = cmaes_doubleRangeMin(rgD, N); minEW = minEW * minEW;
    maxEW = cmaes_doubleRangeMax(rgD, N); maxEW = maxEW * maxEW;

    maxdiagC=C[0][0]; for(i=1;i<N;++i) if(maxdiagC<C[i][i]) maxdiagC=C[i][i];
    mindiagC=C[0][0]; for(i=1;i<N;++i) if(mindiagC>C[i][i]) mindiagC=C[i][i];

    /* set xmean */
    for (i = 0; i < N; ++i)  rgxmean[i] = rgxold[i] = _dims[i]._initialX;
}


void Korali::KoraliCMAES::Korali_GetSamplePopulation()
{
    int iNk, i, j, N=_dimCount;
    int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));
    double sum;
    double const *xmean = rgxmean;

    /* calculate cmaes_eigensystem  */
    if (!flgEigensysIsUptodate) {
        if (!flgdiag)
            cmaes_updateEigensystem(0);
        else {
            for (i = 0; i < N; ++i)
                rgD[i] = sqrt(C[i][i]);
            minEW = cmaes_doubleRangeMin(rgD, N) * cmaes_doubleRangeMin(rgD, N);
            maxEW = cmaes_doubleRangeMax(rgD, N) * cmaes_doubleRangeMin(rgD, N);
            flgEigensysIsUptodate = 1;
        }
    }

    /* treat minimal standard deviations and numeric problems */
    cmaes_testMinStdDevs();

    for (iNk = 0; iNk < _lambda; ++iNk)
    { /* generate scaled cmaes_random vector (D * z)    */
        for (i = 0; i < N; ++i)
            if (flgdiag)
                _samplePopulation[iNk * N + i] = xmean[i] + sigma * rgD[i] * _gaussianGenerator->getRandomNumber();
            else
                rgdTmp[i] = rgD[i] * _gaussianGenerator->getRandomNumber();
        if (!flgdiag)
            /* add mutation (sigma * B * (D*z)) */
            for (i = 0; i < N; ++i) {
                for (j = 0, sum = 0.; j < N; ++j)
                    sum += B[i][j] * rgdTmp[j];
                _samplePopulation[iNk * N + i] = xmean[i] + sigma * sum;
            }
    }

    if(state == 3 || gen == 0)   ++gen;
    state = 1;

    for(int i = 0; i < _lambda; ++i)	while( !cmaes_isFeasible(&_samplePopulation[i*N] )) cmaes_reSampleSingle(i );

} /* SamplePopulation() */


void Korali::KoraliCMAES::cmaes_reSampleSingle(int iindex)
{
    int i, j, N=_dimCount;
    double *rgx;
    double sum;
    char s[99];

    if (iindex < 0 || iindex >= _lambda) {
        sprintf(s, "index==%d must be between 0 and %lu", iindex, _lambda);
        fprintf(stderr, "[CMAES] Error: cmaes_reSampleSingle(): Population member \n");
    }
    rgx = &_samplePopulation[iindex*N];

    for (i = 0; i < N; ++i)
        rgdTmp[i] = rgD[i] * _gaussianGenerator->getRandomNumber();
    /* add mutation (sigma * B * (D*z)) */
    for (i = 0; i < N; ++i) {
        for (j = 0, sum = 0.; j < N; ++j)
            sum += B[i][j] * rgdTmp[j];
        rgx[i] = rgxmean[i] + sigma * sum;
    }
}


void Korali::KoraliCMAES::Korali_UpdateDistribution(const double *fitnessVector)
{
    int i, j, iNk, hsig, N=_dimCount;
    int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));
    double sum;
    double psxps;

    if(state == 3)
        fprintf(stderr, "[CMAES] Error: updateDistribution(): You need to call SamplePopulation() before update can take place.");
    if(fitnessVector == NULL)
        fprintf(stderr, "[CMAES] Error: updateDistribution(): Fitness function value array input is missing.");

    if(state == 1)  /* function values are delivered here */
        countevals += _lambda;
    else
        fprintf(stderr, "[CMAES] Error: updateDistribution(): unexpected state");

    /* assign function values */
    for (i=0; i < _lambda; i++)	curBest[i] = rgFuncValue[i] = fitnessVector[i];

    /* Generate index */
    cmaes_sorted_index(fitnessVector, index, _lambda);

    /* Test if function values are identical, escape flat fitness */
    if (rgFuncValue[index[0]] ==
            rgFuncValue[index[(int)_lambda/2]]) {
        sigma *= exp(0.2+_sigmaCumulationFactor/_dampFactor);
        fprintf(stderr, "[CMAES] Error: Warning: sigma increased due to equal function values. Reconsider the formulation of the objective function\n");
    }

    /* update function value history */
			for(i = (int)*(arFuncValueHist-1)-1; i > 0; --i)
					arFuncValueHist[i] = arFuncValueHist[i-1];
			arFuncValueHist[0] = fitnessVector[index[0]];

    /* update xbestever */
    if ((rgxbestever[N] > curBest[index[0]] || gen == 1))
    {
        for (i = 0; i < N; ++i) rgxbestever[i] = _samplePopulation[index[0]*N + i];
        rgxbestever[N] = curBest[index[0]];
        rgxbestever[N+1] = countevals;
    }

    /* calculate xmean and rgBDz~N(0,C) */
    for (i = 0; i < N; ++i) {
        rgxold[i] = rgxmean[i];
        rgxmean[i] = 0.;
        for (iNk = 0; iNk < _mu; ++iNk)
            rgxmean[i] += _muWeights[iNk] * _samplePopulation[index[iNk]*N + i];
        rgBDz[i] = sqrt(_muEffective)*(rgxmean[i] - rgxold[i])/sigma;
    }

    /* calculate z := D^(-1) * B^(-1) * rgBDz into rgdTmp */
    for (i = 0; i < N; ++i) {
        if (!flgdiag)
            for (j = 0, sum = 0.; j < N; ++j)
                sum += B[j][i] * rgBDz[j];
        else
            sum = rgBDz[i];
        rgdTmp[i] = sum / rgD[i];
    }

    /* cumulation for sigma (ps) using B*z */
    for (i = 0; i < N; ++i) {
        if (!flgdiag)
            for (j = 0, sum = 0.; j < N; ++j)
                sum += B[i][j] * rgdTmp[j];
        else
            sum = rgdTmp[i];
        rgps[i] = (1. - _sigmaCumulationFactor) * rgps[i] +
            sqrt(_sigmaCumulationFactor * (2. - _sigmaCumulationFactor)) * sum;
    }

    /* calculate norm(ps)^2 */
    for (i = 0, psxps = 0.; i < N; ++i)
        psxps += rgps[i] * rgps[i];

    /* cumulation for covariance matrix (pc) using B*D*z~N(0,C) */
    hsig = sqrt(psxps) / sqrt(1. - pow(1.-_sigmaCumulationFactor, 2*gen)) / chiN
        < 1.4 + 2./(N+1);
    for (i = 0; i < N; ++i) {
        rgpc[i] = (1. - _cumulativeCovariance) * rgpc[i] +
            hsig * sqrt(_cumulativeCovariance * (2. - _cumulativeCovariance)) * rgBDz[i];
    }

    /* update of C  */

    cmaes_adaptC2(hsig);

    /* update of sigma */
    sigma *= exp(((sqrt(psxps)/chiN)-1.)*_sigmaCumulationFactor/_dampFactor);

    state = 3;
}


void Korali::KoraliCMAES::cmaes_adaptC2(int hsig)
{
    int i, j, k, N=_dimCount;
    int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));

    if (_covarianceMatrixLearningRate != 0.0) {

        /* definitions for speeding up inner-most loop */
        double ccov1 = std::min(_covarianceMatrixLearningRate * (1./_muCovariance) * (flgdiag ? (N+1.5) / 3. : 1.), 1.);
        double ccovmu = std::min(_covarianceMatrixLearningRate * (1-1./_muCovariance)* (flgdiag ? (N+1.5) / 3. : 1.), 1.-ccov1);
        double sigmasquare = sigma * sigma;

        flgEigensysIsUptodate = 0;

        /* update covariance matrix */
        for (i = 0; i < N; ++i)
            for (j = flgdiag ? i : 0; j <= i; ++j) {
                C[i][j] = (1 - ccov1 - ccovmu) * C[i][j]
                    + ccov1
                    * (rgpc[i] * rgpc[j]
                            + (1-hsig)*_cumulativeCovariance*(2.-_cumulativeCovariance) * C[i][j]);
                for (k = 0; k < _mu; ++k) { /* additional rank mu update */
                    C[i][j] += ccovmu * _muWeights[k]
                        * (_samplePopulation[index[k]*N + i] - rgxold[i])
                        * (_samplePopulation[index[k]*N + j] - rgxold[j])
                        / sigmasquare;
                }
            }
        /* update maximal and minimal diagonal value */
        maxdiagC = mindiagC = C[0][0];
        for (i = 1; i < N; ++i) {
            if (maxdiagC < C[i][i])
                maxdiagC = C[i][i];
            else if (mindiagC > C[i][i])
                mindiagC = C[i][i];
        }
    } /* if ccov... */
}


void Korali::KoraliCMAES::cmaes_testMinStdDevs()
    /* increases sigma */
{
    int i, N = _dimCount;

    for (i = 0; i < N; ++i)
        while (sigma * sqrt(C[i][i]) < _dims[i]._minStdDevChange)
            sigma *= exp(0.05+_sigmaCumulationFactor/_dampFactor);

} /* cmaes_cmaes_testMinStdDevs() */


void Korali::KoraliCMAES::Korali_PrintResults()

    /* this hack reads key words from input key for data to be written to
     * a file, see file signals.par as input file. The length of the keys
     * is mostly fixed, see key += number in the code! If the key phrase
     * does not match the expectation the output might be strange.  for
     * must be zero terminated.
     */
{
    int i, k, N=_dimCount;

    printf("(%lu,%lu)-CMA-ES(mu_eff=%.1f), dimension=%lu, diagonalIterations=%lu, randomSeed=%lu\n", _mu, _lambda, _muEffective,  _dimCount, _diagonalCovarianceMatrixEvalFrequency,  _seed);

		printf(" N %d\n", N);
		printf(" seed %lu\n", _seed);
		printf("function evaluations %.0f\n", countevals);
		printf("function value f(x)=%g\n", curBest[index[0]]);
		printf("maximal standard deviation %g\n", sigma*sqrt(maxdiagC));
		printf("minimal standard deviation %g\n", sigma*sqrt(mindiagC));
		printf("sigma %g\n", sigma);
		printf("axisratio %g\n", cmaes_doubleRangeMax(rgD, N)/cmaes_doubleRangeMin(rgD, N));
		printf("xbestever found after %.0f evaluations, function value %g\n",	rgxbestever[N+1], rgxbestever[N]);

		for(i=0; i<N; ++i) printf(" %12g%c", rgxbestever[i], (i%5==4||i==N-1)?'\n':' ');

		printf("xbest (of last generation, function value %g)\n",	curBest[index[0]]);

		for(i=0; i<N; ++i) printf(" %12g%c", _samplePopulation[index[0]*N + i],(i%5==4||i==N-1)?'\n':' ');

		printf("xmean \n");
		for(i=0; i<N; ++i) printf(" %12g%c", rgxmean[i],	(i%5==4||i==N-1)?'\n':' ');

		printf("Standard deviation of coordinate axes (sigma*sqrt(diag(C)))\n");
		for(i=0; i<N; ++i) printf(" %12g%c", sigma*sqrt(C[i][i]),	(i%5==4||i==N-1)?'\n':' ');

		printf("Main axis lengths of mutation ellipsoid (sigma*diag(D))\n");
		for (i = 0; i < N; ++i)	rgdTmp[i] = rgD[i];
		for(i=0; i<N; ++i)	printf(" %12g%c", sigma*rgdTmp[N-1-i],(i%5==4||i==N-1)?'\n':' ');

		printf("Longest axis (b_i where d_ii=max(diag(D))\n");
		k = cmaes_maxIdx(rgD, N);

		for(i=0; i<N; ++i) printf(" %12g%c", B[i][k], (i%5==4||i==N-1)?'\n':' ');
		printf("Shortest axis (b_i where d_ii=max(diag(D))\n");
		k = cmaes_minIdx(rgD, N);

		for(i=0; i<N; ++i) printf(" %12g%c", B[i][k], (i%5==4||i==N-1)?'\n':' ');
}

double Korali::KoraliCMAES::cmaes_function_value_difference()
{
    return std::max(cmaes_doubleRangeMax(arFuncValueHist, (int)std::min(gen,*(arFuncValueHist-1))),
    		cmaes_doubleRangeMax(rgFuncValue, _lambda)) -
        std::min(cmaes_doubleRangeMin(arFuncValueHist, (int)std::min(gen, *(arFuncValueHist-1))),
               cmaes_doubleRangeMin(rgFuncValue, _lambda));
}

bool Korali::KoraliCMAES::Korali_CheckTermination()
{

    double range, fac;
    int iAchse, iKoo;
    int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));
    int i, cTemp, N=_dimCount;
    bool terminate = false;

    /* function value reached */
    if ((gen > 1 || state > 1) &&   rgFuncValue[index[0]] <= _stopMinFitness)
    {
        terminate = true; printf( "Fitness: function value %7.2e <= stopFitness (%7.2e)\n",
                rgFuncValue[index[0]], _stopMinFitness);
    }

    /* TolFun */
    range = cmaes_function_value_difference();

    if (gen > 0 && range <= _stopFitnessDiffThreshold) {
        terminate = true; printf(
                "TolFun: function value differences %7.2e < _stopFitnessDiffThreshold=%7.2e\n",
                range, _stopFitnessDiffThreshold);
    }

    /* TolFunHist */
    if (gen > *(arFuncValueHist-1)) {
        range = cmaes_doubleRangeMax(arFuncValueHist, (int)*(arFuncValueHist-1))
            - cmaes_doubleRangeMin(arFuncValueHist, (int)*(arFuncValueHist-1));
        if (range <= _stopFitnessDiffHistoryThreshold)
        {
            terminate = true; printf(
                    "TolFunHist: history of function value changes %7.2e _stopFitnessDiffHistoryThreshold=%7.2e",
                    range, _stopFitnessDiffHistoryThreshold);
        }
    }

    /* TolX */
    for(i=0, cTemp=0; i<N; ++i) {
        cTemp += (sigma * sqrt(C[i][i]) < _stopMinDeltaX) ? 1 : 0;
        cTemp += (sigma * rgpc[i] < _stopMinDeltaX) ? 1 : 0;
    }
    if (cTemp == 2*N) {
        terminate = true; printf(
                "TolX: object variable changes below %7.2e \n",
                _stopMinDeltaX);
    }

    /* TolUpX */
    for(i=0; i<N; ++i) {
        if (sigma * sqrt(C[i][i]) > _stopMaxStdDevXFactor * _dims[i]._initialStdDev)
            break;
    }

    if (i < N) {
        terminate = true; printf(
                "TolUpX: standard deviation increased by more than %7.2e, larger initial standard deviation recommended \n",
								_stopMaxStdDevXFactor);
    }

    /* Condition of C greater than dMaxSignifKond */
    if (maxEW >= minEW * dMaxSignifKond) {
        terminate = true; printf(
                "ConditionNumber: maximal condition number %7.2e reached. maxEW=%7.2e,minEW=%7.2e,maxdiagC=%7.2e,mindiagC=%7.2e\n",
                dMaxSignifKond, maxEW, minEW, maxdiagC, mindiagC);
    } /* if */

    /* Principal axis i has no effect on xmean, ie.
       x == x + 0.1 * sigma * rgD[i] * B[i] */
    if (!flgdiag) {
        for (iAchse = 0; iAchse < N; ++iAchse)
        {
            fac = 0.1 * sigma * rgD[iAchse];
            for (iKoo = 0; iKoo < N; ++iKoo){
                if (rgxmean[iKoo] != rgxmean[iKoo] + fac * B[iKoo][iAchse])
                    break;
            }
            if (iKoo == N)
            {
                /* sigma *= exp(0.2+_sigmaCumulationFactor/_dampFactor); */
                terminate = true; printf(
                        "NoEffectAxis: standard deviation 0.1*%7.2e in principal axis %d without effect\n",
                        fac/0.1, iAchse);
                break;
            } /* if (iKoo == N) */
        } /* for iAchse             */
    } /* if flgdiag */

    /* Component of xmean is not changed anymore */
    for (iKoo = 0; iKoo < N; ++iKoo)
    {
        if (rgxmean[iKoo] == rgxmean[iKoo] +
                0.2*sigma*sqrt(C[iKoo][iKoo]))
        {
            /* C[iKoo][iKoo] *= (1 + _covarianceMatrixLearningRate); */
            /* flg = 1; */
            terminate = true; printf(
                    "NoEffectCoordinate: standard deviation 0.2*%7.2e in coordinate %d without effect\n",
                    sigma*sqrt(C[iKoo][iKoo]), iKoo);
            break;
        }

    } /* for iKoo */
    /* if (flg) sigma *= exp(0.05+_sigmaCumulationFactor/_dampFactor); */

    if(countevals >= _maxFitnessEvaluations)
    {
        terminate = true; printf( "MaxFunEvals: conducted function evaluations %.0f >= %lu\n",
                countevals, _maxFitnessEvaluations); }

    if(gen >= _maxGenerations)
    {
        terminate = true; printf( "MaxIter: number of iterations %.0f >= %lu\n",
                gen, _maxGenerations);
    }

    return terminate;

} /* cmaes_Test() */


void Korali::KoraliCMAES::cmaes_updateEigensystem(int flgforce)
{
    int N = _dimCount;

    if(flgforce == 0) if (flgEigensysIsUptodate == 1) return;

    cmaes_eigen( N, C, rgD, B);

    /* find largest and smallest cmaes_eigenvalue, they are supposed to be sorted anyway */
    minEW = cmaes_doubleRangeMin(rgD, N);
    maxEW = cmaes_doubleRangeMax(rgD, N);

    for (int i = 0; i < N; ++i) 	rgD[i] = sqrt(rgD[i]);

    flgEigensysIsUptodate = 1;
}

void Korali::KoraliCMAES::cmaes_eigen( int N,  double **C, double *diag, double **Q)
    /*
       Calculating cmaes_eigenvalues and vectors.
       Input:
       N: dimension.
       C: symmetric (1:N)xN-matrix, solely used to copy data to Q
       niter: number of maximal iterations for QL-Algorithm.
       Output:
       diag: N cmaes_eigenvalues.
       Q: Columns are normalized cmaes_eigenvectors.
       */
{
  double* data = (double*) malloc (sizeof(double) * N * N);

  for (int i = 0; i <  N; i++)
  for (int j = 0; j <= i; j++)
  {
  	data[i*N + j] = C[i][j];
  	data[j*N + i] = C[i][j];
  }

  gsl_matrix_view m  = gsl_matrix_view_array (data, N, N);

  gsl_vector *eval = gsl_vector_alloc (N);
  gsl_matrix *evec = gsl_matrix_alloc (N, N);
  gsl_eigen_symmv_workspace * w =  gsl_eigen_symmv_alloc (N);
  gsl_eigen_symmv (&m.matrix, eval, evec, w);
  gsl_eigen_symmv_free (w);
  gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_ASC);
	for (int i = 0; i < N; i++)
	{
		double eval_i = gsl_vector_get (eval, i);
		gsl_vector_view evec_i = gsl_matrix_column (evec, i);
		for (int j = 0; j < N; j++)	Q[j][i] =  -gsl_vector_get (&evec_i.vector, j);
	}

	for (int i = 0; i < N; i++)	diag[i] = gsl_vector_get (eval, i);

  gsl_vector_free (eval);
  gsl_matrix_free (evec);
  free(data);

}

int Korali::KoraliCMAES::cmaes_maxIdx(const double *rgd, int len)
{
    int i, res;
    for(i=1, res=0; i<len; ++i)
        if(rgd[i] > rgd[res])
            res = i;
    return res;
}

int Korali::KoraliCMAES::cmaes_minIdx(const double *rgd, int len)
{
    int i, res;
    for(i=1, res=0; i<len; ++i)
        if(rgd[i] < rgd[res])
            res = i;
    return res;
}

/* dirty index sort */
void Korali::KoraliCMAES::cmaes_sorted_index(const double *fitnessVector, int *iindex, int n)
{
    int i, j;
    for (i=1, iindex[0]=0; i<n; ++i) {
        for (j=i; j>0; --j) {
            if (fitnessVector[iindex[j-1]] < fitnessVector[i])
                break;
            iindex[j] = iindex[j-1]; /* shift up */
        }
        iindex[j] = i; /* insert i */
    }
}


double Korali::KoraliCMAES::cmaes_doubleRangeMax(const double *rgd, int len)
{
    int i;
    double max = rgd[0];
    for (i = 1; i < len; ++i)
        max = (max < rgd[i]) ? rgd[i] : max;
    return max;
}

double Korali::KoraliCMAES::cmaes_doubleRangeMin(const double *rgd, int len)
{
    int i;
    double min = rgd[0];
    for (i = 1; i < len; ++i)
        min = (min > rgd[i]) ? rgd[i] : min;
    return min;
}
