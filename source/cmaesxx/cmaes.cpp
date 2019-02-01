/* --------------------------------------------------------- */
/* --- File: cmaes.c  -------- Author: Nikolaus Hansen   --- */
/* --------------------------------------------------------- */
/*   
     CMA-ES for non-linear function minimization. 

     Copyright 1996, 2003, 2007, 2013 Nikolaus Hansen
     e-mail: hansen .AT. lri.fr

LICENSE: this library is free/open software and may be used 
either under theApache License 2.0 or under the GNU Lesser General
Public License 2.1 or laterwhichever suits best.

See also the LICENSE file
https://github.com/cma-es/c-cma-es/blob/master/LICENSE 
*/

#include <math.h>   /* sqrt() */
#include <stddef.h> /* size_t */
#include <stdlib.h> /* NULL, free */
#include <string.h> /* strlen() */
#include <stdio.h>  /* sprintf(), NULL? */
#include "cmaes.h" /* <time.h> via cmaes_types.h */

/* ------------------- Locally visibly ----------------------- */

void TestMinStdDevs( cmaes_t *);
/* void WriteMaxErrorInfo( cmaes_t *); */
void Eigen( int N,  double **C, double *diag, double **Q, double *rgtmp);
void Adapt_C2(cmaes_t *t, int hsig);
void   Sorted_index( const double *rgFunVal, int *index, int n);
double rgdouMax( const double *rgd, int len);
double rgdouMin( const double *rgd, int len);
int    MaxIdx( const double *rgd, int len);
int    MinIdx( const double *rgd, int len);
const char * c_cmaes_version = "3.20.00.beta";

/* --------------------------------------------------------- */
/* ---------------- Functions: cmaes_t --------------------- */
/* --------------------------------------------------------- */

char * cmaes_SayHello(cmaes_t *t)
{
    sprintf(t->sOutString, 
            "(%d,%d)-CMA-ES(mu_eff=%.1f), Ver=\"%s\", dimension=%d, diagonalIterations=%ld, randomSeed=%d",
            kb->_mu, kb->_lambda, kb->_muEffective, c_cmaes_version, kb->_dimCount, kb->_diagonalCovarianceMatrixEvalFrequency,
            kb->_seed);

    return t->sOutString; 
}

double * cmaes_init(cmaes_t *t /* "this" */)
{
    int i, j, N;
    double dtest, trace;

    N = kb->_dimCount; /* for convenience */

    for (i = 0, trace = 0.; i < N; ++i)
        trace += kb->_dims[i]._initialStdDev*kb->_dims[i]._initialStdDev;
    t->sigma = sqrt(trace/N); /* kb->_muEffective/(0.2*kb->_muEffective+sqrt(N)) * sqrt(trace/N); */

    t->chiN = sqrt((double) N) * (1. - 1./(4.*N) + 1./(21.*N*N));
    t->flgEigensysIsUptodate = 1;
    t->genOfEigensysUpdate = 0;

    t->flgIniphase = 0; /* do not use iniphase, hsig does the job now */
    t->flgStop = 0;

    for (dtest = 1.; dtest && dtest < 1.1 * dtest; dtest *= 2.)
        if (dtest == dtest + 1.)
            break;
    t->dMaxSignifKond = dtest / 1000.; /* not sure whether this is really save, 100 does not work well enough */

    t->gen = 0;
    t->countevals = 0;
    t->state = 0;
    t->dLastMinEWgroesserNull = 1.0;

    t->rgpc = (double*) calloc (sizeof(double), N);
    t->rgps = (double*) calloc (sizeof(double), N);
    t->rgdTmp = (double*) calloc (sizeof(double), N+1);
    t->rgBDz = (double*) calloc (sizeof(double), N);
    t->rgxmean = (double*) calloc (sizeof(double), N+2); t->rgxmean[0] = N; ++t->rgxmean;
    t->rgxold = (double*) calloc (sizeof(double), N+2); t->rgxold[0] = N; ++t->rgxold;
    t->rgxbestever = (double*) calloc (sizeof(double), N+3); t->rgxbestever[0] = N; ++t->rgxbestever;
    t->rgout = (double*) calloc (sizeof(double), N+2); t->rgout[0] = N; ++t->rgout;
    t->rgD = (double*) calloc (sizeof(double), N);
    t->C = (double**) calloc (sizeof(double*), N);
    t->B = (double**)calloc (sizeof(double*), N);
    t->publicFitness = (double*) calloc (sizeof(double), kb->_lambda);
    t->rgFuncValue = (double*) calloc (sizeof(double), kb->_lambda+1);
    t->rgFuncValue[0]=kb->_lambda; ++t->rgFuncValue;
    t->arFuncValueHist = (double*) calloc (sizeof(double), 10+(int)ceil(3.*10.*N/kb->_lambda)+1);
    t->arFuncValueHist[0] = (double)(10+(int)ceil(3.*10.*N/kb->_lambda));
    t->arFuncValueHist++; 

    for (i = 0; i < N; ++i) {
        t->C[i] = (double*) calloc (sizeof(double), i+1);
        t->B[i] = (double*) calloc (sizeof(double), N);
    }
    t->index = (int *) calloc (sizeof(int*), kb->_lambda);
    for (i = 0; i < kb->_lambda; ++i)
        t->index[i] = i; /* should not be necessary */
    t->rgrgx = (double **) calloc (sizeof(double*), kb->_lambda);
    for (i = 0; i < kb->_lambda; ++i) {
        t->rgrgx[i] = (double*) calloc (sizeof(double), N+2);
        t->rgrgx[i][0] = N; 
        t->rgrgx[i]++;
    }

    /* Initialize newed space  */

    for (i = 0; i < N; ++i)
        for (j = 0; j < i; ++j)
            t->C[i][j] = t->B[i][j] = t->B[j][i] = 0.;

    for (i = 0; i < N; ++i)
    {
        t->B[i][i] = 1.;
        t->C[i][i] = t->rgD[i] = kb->_dims[i]._initialStdDev * sqrt(N / trace);
        t->C[i][i] *= t->C[i][i];
        t->rgpc[i] = t->rgps[i] = 0.;
    }

    t->minEW = rgdouMin(t->rgD, N); t->minEW = t->minEW * t->minEW;
    t->maxEW = rgdouMax(t->rgD, N); t->maxEW = t->maxEW * t->maxEW;

    t->maxdiagC=t->C[0][0]; for(i=1;i<N;++i) if(t->maxdiagC<t->C[i][i]) t->maxdiagC=t->C[i][i];
    t->mindiagC=t->C[0][0]; for(i=1;i<N;++i) if(t->mindiagC>t->C[i][i]) t->mindiagC=t->C[i][i];

    /* set xmean */
    for (i = 0; i < N; ++i)
        t->rgxmean[i] = t->rgxold[i] = (*kb)[i]->_initialX;


    return (t->publicFitness); 

} /* cmaes_init_final() */




/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * const * cmaes_SamplePopulation(cmaes_t *t)
{
    int iNk, i, j, N=kb->_dimCount;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= t->gen));
    double sum;
    double const *xmean = t->rgxmean; 

    /* calculate eigensystem  */
    if (!t->flgEigensysIsUptodate) {
        if (!flgdiag)
            cmaes_UpdateEigensystem(t, 0);
        else {
            for (i = 0; i < N; ++i)
                t->rgD[i] = sqrt(t->C[i][i]);
            t->minEW = rgdouMin(t->rgD, N) * rgdouMin(t->rgD, N);
            t->maxEW = rgdouMax(t->rgD, N) * rgdouMin(t->rgD, N);
            t->flgEigensysIsUptodate = 1;
        }
    }

    /* treat minimal standard deviations and numeric problems */
    TestMinStdDevs(t); 

    for (iNk = 0; iNk < kb->_lambda; ++iNk)
    { /* generate scaled cmaes_random vector (D * z)    */
        for (i = 0; i < N; ++i)
            if (flgdiag)
                t->rgrgx[iNk][i] = xmean[i] + t->sigma * t->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
            else
                t->rgdTmp[i] = t->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
        if (!flgdiag)
            /* add mutation (sigma * B * (D*z)) */
            for (i = 0; i < N; ++i) {
                for (j = 0, sum = 0.; j < N; ++j)
                    sum += t->B[i][j] * t->rgdTmp[j];
                t->rgrgx[iNk][i] = xmean[i] + t->sigma * sum;
            }
    }
    if(t->state == 3 || t->gen == 0)
        ++t->gen;
    t->state = 1; 

    return(t->rgrgx);
} /* SamplePopulation() */


double * const * cmaes_ReSampleSingle(cmaes_t *t, int iindex)
{
    int i, j, N=kb->_dimCount;
    double *rgx; 
    double sum; 
    char s[99];

    if (iindex < 0 || iindex >= kb->_lambda) {
        sprintf(s, "index==%d must be between 0 and %d", iindex, kb->_lambda);
        fprintf(stderr, "[CMAES] Error: cmaes_ReSampleSingle(): Population member ",s,0,0);
    }
    rgx = t->rgrgx[iindex];

    for (i = 0; i < N; ++i)
        t->rgdTmp[i] = t->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
    /* add mutation (sigma * B * (D*z)) */
    for (i = 0; i < N; ++i) {
        for (j = 0, sum = 0.; j < N; ++j)
            sum += t->B[i][j] * t->rgdTmp[j];
        rgx[i] = t->rgxmean[i] + t->sigma * sum;
    }
    return(t->rgrgx);
}

double * cmaes_SampleSingleInto(cmaes_t *t, double *rgx)
{
    int i, j, N=kb->_dimCount;
    double sum; 

    if (rgx == NULL)
        rgx = (double*) calloc (sizeof(double), N);

    for (i = 0; i < N; ++i)
        t->rgdTmp[i] = t->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
    /* add mutation (sigma * B * (D*z)) */
    for (i = 0; i < N; ++i) {
        for (j = 0, sum = 0.; j < N; ++j)
            sum += t->B[i][j] * t->rgdTmp[j];
        rgx[i] = t->rgxmean[i] + t->sigma * sum;
    }
    return rgx;
}

double * cmaes_UpdateDistribution(int save_hist, cmaes_t *t, const double *rgFunVal)
{
    int i, j, iNk, hsig, N=kb->_dimCount;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= t->gen));
    double sum; 
    double psxps; 

    if(t->state == 3)
        fprintf(stderr, "[CMAES] Error: cmaes_UpdateDistribution(): You need to call \n",
                "SamplePopulation() before update can take place.");
    if(rgFunVal == NULL) 
        fprintf(stderr, "[CMAES] Error: cmaes_UpdateDistribution(): ",
                "Fitness function value array input is missing.");

    if(save_hist && t->state == 1)  /* function values are delivered here */
        t->countevals += kb->_lambda;
    else
        fprintf(stderr, "[CMAES] Error: cmaes_UpdateDistribution(): unexpected state");

    /* assign function values */
    for (i=0; i < kb->_lambda; ++i)
        t->rgrgx[i][N] = t->rgFuncValue[i] = rgFunVal[i];


    /* Generate index */
    Sorted_index(rgFunVal, t->index, kb->_lambda);

    /* Test if function values are identical, escape flat fitness */
    if (t->rgFuncValue[t->index[0]] == 
            t->rgFuncValue[t->index[(int)kb->_lambda/2]]) {
        t->sigma *= exp(0.2+kb->_sigmaCumulationFactor/kb->_dampFactor);
        fprintf(stderr, "[CMAES] Error: Warning: sigma increased due to equal function values. Reconsider the formulation of the objective function\n");
    }

    /* update function value history */
    if (save_hist) {
        for(i = (int)*(t->arFuncValueHist-1)-1; i > 0; --i)
            t->arFuncValueHist[i] = t->arFuncValueHist[i-1];
        t->arFuncValueHist[0] = rgFunVal[t->index[0]];
    }
    
    /* update xbestever */
    if (save_hist && (t->rgxbestever[N] > t->rgrgx[t->index[0]][N] || t->gen == 1))
        for (i = 0; i <= N; ++i) {
            t->rgxbestever[i] = t->rgrgx[t->index[0]][i];
            t->rgxbestever[N+1] = t->countevals;
        }

    /* calculate xmean and rgBDz~N(0,C) */
    for (i = 0; i < N; ++i) {
        t->rgxold[i] = t->rgxmean[i]; 
        t->rgxmean[i] = 0.;
        for (iNk = 0; iNk < kb->_mu; ++iNk)
            t->rgxmean[i] += kb->_muWeights[iNk] * t->rgrgx[t->index[iNk]][i];
        t->rgBDz[i] = sqrt(kb->_muEffective)*(t->rgxmean[i] - t->rgxold[i])/t->sigma;
    }

    /* calculate z := D^(-1) * B^(-1) * rgBDz into rgdTmp */
    for (i = 0; i < N; ++i) {
        if (!flgdiag)
            for (j = 0, sum = 0.; j < N; ++j)
                sum += t->B[j][i] * t->rgBDz[j];
        else
            sum = t->rgBDz[i];
        t->rgdTmp[i] = sum / t->rgD[i];
    }

    /* TODO?: check length of t->rgdTmp and set an upper limit, e.g. 6 stds */
    /* in case of manipulation of arx, 
       this can prevent an increase of sigma by several orders of magnitude
       within one step; a five-fold increase in one step can still happen. 
       */ 
    /*
       for (j = 0, sum = 0.; j < N; ++j)
       sum += t->rgdTmp[j] * t->rgdTmp[j];
       if (sqrt(sum) > chiN + 6. * sqrt(0.5)) {
       rgdTmp length should be set to upper bound and hsig should become zero 
       }
       */

    /* cumulation for sigma (ps) using B*z */
    for (i = 0; i < N; ++i) {
        if (!flgdiag)
            for (j = 0, sum = 0.; j < N; ++j)
                sum += t->B[i][j] * t->rgdTmp[j];
        else
            sum = t->rgdTmp[i];
        t->rgps[i] = (1. - kb->_sigmaCumulationFactor) * t->rgps[i] +
            sqrt(kb->_sigmaCumulationFactor * (2. - kb->_sigmaCumulationFactor)) * sum;
    }

    /* calculate norm(ps)^2 */
    for (i = 0, psxps = 0.; i < N; ++i)
        psxps += t->rgps[i] * t->rgps[i];

    /* cumulation for covariance matrix (pc) using B*D*z~N(0,C) */
    hsig = sqrt(psxps) / sqrt(1. - pow(1.-kb->_sigmaCumulationFactor, 2*t->gen)) / t->chiN
        < 1.4 + 2./(N+1);
    for (i = 0; i < N; ++i) {
        t->rgpc[i] = (1. - kb->_cumulativeCovariance) * t->rgpc[i] +
            hsig * sqrt(kb->_cumulativeCovariance * (2. - kb->_cumulativeCovariance)) * t->rgBDz[i];
    }

    /* stop initial phase */
    if (t->flgIniphase && 
            t->gen > std::min(1/kb->_sigmaCumulationFactor, 1+N/kb->_muCovariance))
    {
        if (psxps / kb->_dampFactor / (1.-pow((1. - kb->_sigmaCumulationFactor), t->gen))
                < N * 1.05) 
            t->flgIniphase = 0;
    }

    /* update of C  */

    Adapt_C2(t, hsig);

    /* update of sigma */
    t->sigma *= exp(((sqrt(psxps)/t->chiN)-1.)*kb->_sigmaCumulationFactor/kb->_dampFactor);

    t->state = 3;

    return (t->rgxmean);

} /* cmaes_UpdateDistribution() */

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void Adapt_C2(cmaes_t *t, int hsig)
{
    int i, j, k, N=kb->_dimCount;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= t->gen));

    if (kb->_covarianceMatrixLearningRate != 0. && t->flgIniphase == 0) {

        /* definitions for speeding up inner-most loop */
        double ccov1 = std::min(kb->_covarianceMatrixLearningRate * (1./kb->_muCovariance) * (flgdiag ? (N+1.5) / 3. : 1.), 1.);
        double ccovmu = std::min(kb->_covarianceMatrixLearningRate * (1-1./kb->_muCovariance)* (flgdiag ? (N+1.5) / 3. : 1.), 1.-ccov1);
        double sigmasquare = t->sigma * t->sigma; 

        t->flgEigensysIsUptodate = 0;

        /* update covariance matrix */
        for (i = 0; i < N; ++i)
            for (j = flgdiag ? i : 0; j <= i; ++j) {
                t->C[i][j] = (1 - ccov1 - ccovmu) * t->C[i][j] 
                    + ccov1
                    * (t->rgpc[i] * t->rgpc[j] 
                            + (1-hsig)*kb->_cumulativeCovariance*(2.-kb->_cumulativeCovariance) * t->C[i][j]);
                for (k = 0; k < kb->_mu; ++k) { /* additional rank mu update */
                    t->C[i][j] += ccovmu * kb->_muWeights[k]
                        * (t->rgrgx[t->index[k]][i] - t->rgxold[i]) 
                        * (t->rgrgx[t->index[k]][j] - t->rgxold[j])
                        / sigmasquare;
                }
            }
        /* update maximal and minimal diagonal value */
        t->maxdiagC = t->mindiagC = t->C[0][0];
        for (i = 1; i < N; ++i) {
            if (t->maxdiagC < t->C[i][i])
                t->maxdiagC = t->C[i][i];
            else if (t->mindiagC > t->C[i][i])
                t->mindiagC = t->C[i][i];
        }
    } /* if ccov... */
}


void TestMinStdDevs(cmaes_t *t)
    /* increases sigma */
{
    int i, N = kb->_dimCount;

    for (i = 0; i < N; ++i)
        while (t->sigma * sqrt(t->C[i][i]) < kb->_dims[i]._minStdDevChange)
            t->sigma *= exp(0.05+kb->_sigmaCumulationFactor/kb->_dampFactor);

} /* cmaes_TestMinStdDevs() */



/* --------------------------------------------------------- */
void cmaes_PrintResults(cmaes_t *t)

    /* this hack reads key words from input key for data to be written to
     * a file, see file signals.par as input file. The length of the keys
     * is mostly fixed, see key += number in the code! If the key phrase
     * does not match the expectation the output might be strange.  for
     * cmaes_t *t == NULL it solely prints key as a header line. Input key
     * must be zero terminated.
     */
{ 
    int i, k, N=kb->_dimCount;

		printf(" N %d\n", N);
		printf(" seed %d\n", kb->_seed);
		printf("function evaluations %.0f\n", t->countevals);
		printf("function value f(x)=%g\n", t->rgrgx[t->index[0]][N]);
		printf("maximal standard deviation %g\n", t->sigma*sqrt(t->maxdiagC));
		printf("minimal standard deviation %g\n", t->sigma*sqrt(t->mindiagC));
		printf("sigma %g\n", t->sigma);
		printf("axisratio %g\n", rgdouMax(t->rgD, N)/rgdouMin(t->rgD, N));
		printf("xbestever found after %.0f evaluations, function value %g\n",	t->rgxbestever[N+1], t->rgxbestever[N]);

		for(i=0; i<N; ++i) printf(" %12g%c", t->rgxbestever[i], (i%5==4||i==N-1)?'\n':' ');

		printf("xbest (of last generation, function value %g)\n",	t->rgrgx[t->index[0]][N]);

		for(i=0; i<N; ++i) printf(" %12g%c", t->rgrgx[t->index[0]][i],(i%5==4||i==N-1)?'\n':' ');

		printf("xmean \n");
		for(i=0; i<N; ++i) printf(" %12g%c", t->rgxmean[i],	(i%5==4||i==N-1)?'\n':' ');

		printf("Standard deviation of coordinate axes (sigma*sqrt(diag(C)))\n");
		for(i=0; i<N; ++i) printf(" %12g%c", t->sigma*sqrt(t->C[i][i]),	(i%5==4||i==N-1)?'\n':' ');

		printf("Main axis lengths of mutation ellipsoid (sigma*diag(D))\n");
		for (i = 0; i < N; ++i)	t->rgdTmp[i] = t->rgD[i];
		for(i=0; i<N; ++i)	printf(" %12g%c", t->sigma*t->rgdTmp[N-1-i],(i%5==4||i==N-1)?'\n':' ');

		printf("Longest axis (b_i where d_ii=max(diag(D))\n");
		k = MaxIdx(t->rgD, N);

		for(i=0; i<N; ++i) printf(" %12g%c", t->B[i][k], (i%5==4||i==N-1)?'\n':' ');
		printf("Shortest axis (b_i where d_ii=max(diag(D))\n");
		k = MinIdx(t->rgD, N);

		for(i=0; i<N; ++i) printf(" %12g%c", t->B[i][k], (i%5==4||i==N-1)?'\n':' ');
}

double function_value_difference(cmaes_t *t) {
    return std::max(rgdouMax(t->arFuncValueHist, (int)std::min(t->gen,*(t->arFuncValueHist-1))),
    		rgdouMax(t->rgFuncValue, kb->_lambda)) -
        std::min(rgdouMin(t->arFuncValueHist, (int)std::min(t->gen, *(t->arFuncValueHist-1))),
               rgdouMin(t->rgFuncValue, kb->_lambda));
}

const char * cmaes_TestForTermination( cmaes_t *t)
{

    double range, fac;
    int iAchse, iKoo;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= t->gen));
    char sTestOutString[3024];
    char * cp = sTestOutString;
    int i, cTemp, N=kb->_dimCount;
    cp[0] = '\0';

    /* function value reached */
    if ((t->gen > 1 || t->state > 1) &&   t->rgFuncValue[t->index[0]] <= kb->_stopMinFitness)
        cp += sprintf(cp, "Fitness: function value %7.2e <= stopFitness (%7.2e)\n", 
                t->rgFuncValue[t->index[0]], kb->_stopMinFitness);

    /* TolFun */
    range = function_value_difference(t);

    if (t->gen > 0 && range <= kb->_stopFitnessDiffThreshold) {
        cp += sprintf(cp, 
                "TolFun: function value differences %7.2e < kb->_stopFitnessDiffThreshold=%7.2e\n",
                range, kb->_stopFitnessDiffThreshold);
    }

    /* TolFunHist */
    if (t->gen > *(t->arFuncValueHist-1)) {
        range = rgdouMax(t->arFuncValueHist, (int)*(t->arFuncValueHist-1)) 
            - rgdouMin(t->arFuncValueHist, (int)*(t->arFuncValueHist-1));
        if (range <= kb->_stopFitnessDiffHistoryThreshold)
            cp += sprintf(cp, 
                    "TolFunHist: history of function value changes %7.2e kb->_stopFitnessDiffHistoryThreshold=%7.2e",
                    range, kb->_stopFitnessDiffHistoryThreshold);
    }

    /* TolX */
    for(i=0, cTemp=0; i<N; ++i) {
        cTemp += (t->sigma * sqrt(t->C[i][i]) < kb->_stopMinDeltaX) ? 1 : 0;
        cTemp += (t->sigma * t->rgpc[i] < kb->_stopMinDeltaX) ? 1 : 0;
    }
    if (cTemp == 2*N) {
        cp += sprintf(cp, 
                "TolX: object variable changes below %7.2e \n", 
                kb->_stopMinDeltaX);
    }

    /* TolUpX */
    for(i=0; i<N; ++i) {
        if (t->sigma * sqrt(t->C[i][i]) > kb->_stopMaxStdDevXFactor * kb->_dims[i]._initialStdDev)
            break;
    }
    if (i < N) {
        cp += sprintf(cp, 
                "TolUpX: standard deviation increased by more than %7.2e, larger initial standard deviation recommended \n", 
								kb->_stopMaxStdDevXFactor);
    }

    /* Condition of C greater than dMaxSignifKond */
    if (t->maxEW >= t->minEW * t->dMaxSignifKond) {
        cp += sprintf(cp,
                "ConditionNumber: maximal condition number %7.2e reached. maxEW=%7.2e,minEW=%7.2e,maxdiagC=%7.2e,mindiagC=%7.2e\n",
                t->dMaxSignifKond, t->maxEW, t->minEW, t->maxdiagC, t->mindiagC);
    } /* if */

    /* Principal axis i has no effect on xmean, ie. 
       x == x + 0.1 * sigma * rgD[i] * B[i] */
    if (!flgdiag) {
        for (iAchse = 0; iAchse < N; ++iAchse)
        {
            fac = 0.1 * t->sigma * t->rgD[iAchse];
            for (iKoo = 0; iKoo < N; ++iKoo){ 
                if (t->rgxmean[iKoo] != t->rgxmean[iKoo] + fac * t->B[iKoo][iAchse])
                    break;
            }
            if (iKoo == N)        
            {
                /* t->sigma *= exp(0.2+kb->_sigmaCumulationFactor/kb->_dampFactor); */
                cp += sprintf(cp, 
                        "NoEffectAxis: standard deviation 0.1*%7.2e in principal axis %d without effect\n", 
                        fac/0.1, iAchse);
                break;
            } /* if (iKoo == N) */
        } /* for iAchse             */
    } /* if flgdiag */
    /* Component of xmean is not changed anymore */
    for (iKoo = 0; iKoo < N; ++iKoo)
    {
        if (t->rgxmean[iKoo] == t->rgxmean[iKoo] + 
                0.2*t->sigma*sqrt(t->C[iKoo][iKoo]))
        {
            /* t->C[iKoo][iKoo] *= (1 + kb->_covarianceMatrixLearningRate); */
            /* flg = 1; */
            cp += sprintf(cp, 
                    "NoEffectCoordinate: standard deviation 0.2*%7.2e in coordinate %d without effect\n", 
                    t->sigma*sqrt(t->C[iKoo][iKoo]), iKoo); 
            break;
        }

    } /* for iKoo */
    /* if (flg) t->sigma *= exp(0.05+kb->_sigmaCumulationFactor/kb->_dampFactor); */

    if(t->countevals >= kb->_maxFitnessEvaluations)
        cp += sprintf(cp, "MaxFunEvals: conducted function evaluations %.0f >= %lu\n",
                t->countevals, kb->_maxFitnessEvaluations);
    if(t->gen >= kb->_maxGenerations)
        cp += sprintf(cp, "MaxIter: number of iterations %.0f >= %lu\n",
                t->gen, kb->_maxGenerations);
    if(t->flgStop)
        cp += sprintf(cp, "Manual: stop signal read\n");

    if (cp - sTestOutString>320)
        fprintf(stderr, "[CMAES] Error: Bug in cmaes_t:Test(): sTestOutString too short");

    if (cp != sTestOutString) {
        return sTestOutString;
    }

    return(NULL);

} /* cmaes_Test() */


void cmaes_UpdateEigensystem(cmaes_t *t, int flgforce)
{
    int N = kb->_dimCount;

    if(flgforce == 0) if (t->flgEigensysIsUptodate == 1) return;

    Eigen( N, t->C, t->rgD, t->B, t->rgdTmp);

    /* find largest and smallest eigenvalue, they are supposed to be sorted anyway */
    t->minEW = rgdouMin(t->rgD, N);
    t->maxEW = rgdouMax(t->rgD, N);

    for (int i = 0; i < N; ++i) 	t->rgD[i] = sqrt(t->rgD[i]);

    t->flgEigensysIsUptodate = 1;
    t->genOfEigensysUpdate = t->gen; 
}

void Eigen( int N,  double **C, double *diag, double **Q, double *rgtmp)
    /* 
       Calculating eigenvalues and vectors. 
       Input: 
       N: dimension.
       C: symmetric (1:N)xN-matrix, solely used to copy data to Q
       niter: number of maximal iterations for QL-Algorithm. 
       rgtmp: N+1-dimensional vector for temporal use. 
       Output: 
       diag: N eigenvalues. 
       Q: Columns are normalized eigenvectors.
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

double rgdouMax(const double *rgd, int len)
{
    int i;
    double max = rgd[0];
    for (i = 1; i < len; ++i)
        max = (max < rgd[i]) ? rgd[i] : max;
    return max;
}

double rgdouMin(const double *rgd, int len)
{
    int i;
    double min = rgd[0];
    for (i = 1; i < len; ++i)
        min = (min > rgd[i]) ? rgd[i] : min;
    return min;
}

int MaxIdx(const double *rgd, int len)
{
    int i, res;
    for(i=1, res=0; i<len; ++i)
        if(rgd[i] > rgd[res])
            res = i;
    return res;
}

int MinIdx(const double *rgd, int len)
{
    int i, res;
    for(i=1, res=0; i<len; ++i)
        if(rgd[i] < rgd[res])
            res = i;
    return res;
}

/* dirty index sort */
void Sorted_index(const double *rgFunVal, int *iindex, int n)
{
    int i, j;
    for (i=1, iindex[0]=0; i<n; ++i) {
        for (j=i; j>0; --j) {
            if (rgFunVal[iindex[j-1]] < rgFunVal[i])
                break;
            iindex[j] = iindex[j-1]; /* shift up */
        }
        iindex[j] = i; /* insert i */
    }
}



