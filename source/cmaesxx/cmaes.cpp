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
#include "cmaes.h"

/* ------------------- Locally visibly ----------------------- */

void TestMinStdDevs();

void Eigen( int N,  double **C, double *diag, double **Q, double *rgtmp);
void Adapt_C2(int hsig);
void   Sorted_index( const double *rgFunVal, int *index, int n);
double rgdouMax( const double *rgd, int len);
double rgdouMin( const double *rgd, int len);
int    MaxIdx( const double *rgd, int len);
int    MinIdx( const double *rgd, int len);


double * cmaes_init()
{
    int i, j, N;
    double dtest, trace;

    N = kb->_dimCount; /* for convenience */

    for (i = 0, trace = 0.; i < N; ++i)
        trace += kb->_dims[i]._initialStdDev*kb->_dims[i]._initialStdDev;
    kb->sigma = sqrt(trace/N); /* kb->_muEffective/(0.2*kb->_muEffective+sqrt(N)) * sqrt(trace/N); */

    kb->chiN = sqrt((double) N) * (1. - 1./(4.*N) + 1./(21.*N*N));
    kb->flgEigensysIsUptodate = 1;

    for (dtest = 1.; dtest && dtest < 1.1 * dtest; dtest *= 2.)
        if (dtest == dtest + 1.)
            break;
    kb->dMaxSignifKond = dtest / 1000.; /* not sure whether this is really save, 100 does not work well enough */

    kb->gen = 0;
    kb->countevals = 0;
    kb->state = 0;

    kb->rgpc = (double*) calloc (sizeof(double), N);
    kb->rgps = (double*) calloc (sizeof(double), N);
    kb->rgdTmp = (double*) calloc (sizeof(double), N+1);
    kb->rgBDz = (double*) calloc (sizeof(double), N);
    kb->rgxmean = (double*) calloc (sizeof(double), N+2); kb->rgxmean[0] = N; ++kb->rgxmean;
    kb->rgxold = (double*) calloc (sizeof(double), N+2); kb->rgxold[0] = N; ++kb->rgxold;
    kb->rgxbestever = (double*) calloc (sizeof(double), N+3); kb->rgxbestever[0] = N; ++kb->rgxbestever;
    kb->rgout = (double*) calloc (sizeof(double), N+2); kb->rgout[0] = N; ++kb->rgout;
    kb->rgD = (double*) calloc (sizeof(double), N);
    kb->C = (double**) calloc (sizeof(double*), N);
    kb->B = (double**)calloc (sizeof(double*), N);
    kb->publicFitness = (double*) calloc (sizeof(double), kb->_lambda);
    kb->rgFuncValue = (double*) calloc (sizeof(double), kb->_lambda+1);
    kb->rgFuncValue[0]=kb->_lambda; ++kb->rgFuncValue;
    kb->arFuncValueHist = (double*) calloc (sizeof(double), 10+(int)ceil(3.*10.*N/kb->_lambda)+1);
    kb->arFuncValueHist[0] = (double)(10+(int)ceil(3.*10.*N/kb->_lambda));
    kb->arFuncValueHist++;

    for (i = 0; i < N; ++i) {
        kb->C[i] = (double*) calloc (sizeof(double), i+1);
        kb->B[i] = (double*) calloc (sizeof(double), N);
    }
    kb->index = (int *) calloc (sizeof(int*), kb->_lambda);
    for (i = 0; i < kb->_lambda; ++i)
        kb->index[i] = i; /* should not be necessary */
    kb->rgrgx = (double **) calloc (sizeof(double*), kb->_lambda);
    for (i = 0; i < kb->_lambda; ++i) {
        kb->rgrgx[i] = (double*) calloc (sizeof(double), N+2);
        kb->rgrgx[i][0] = N;
        kb->rgrgx[i]++;
    }

    /* Initialize newed space  */

    for (i = 0; i < N; ++i)
        for (j = 0; j < i; ++j)
            kb->C[i][j] = kb->B[i][j] = kb->B[j][i] = 0.;

    for (i = 0; i < N; ++i)
    {
        kb->B[i][i] = 1.;
        kb->C[i][i] = kb->rgD[i] = kb->_dims[i]._initialStdDev * sqrt(N / trace);
        kb->C[i][i] *= kb->C[i][i];
        kb->rgpc[i] = kb->rgps[i] = 0.;
    }

    kb->minEW = rgdouMin(kb->rgD, N); kb->minEW = kb->minEW * kb->minEW;
    kb->maxEW = rgdouMax(kb->rgD, N); kb->maxEW = kb->maxEW * kb->maxEW;

    kb->maxdiagC=kb->C[0][0]; for(i=1;i<N;++i) if(kb->maxdiagC<kb->C[i][i]) kb->maxdiagC=kb->C[i][i];
    kb->mindiagC=kb->C[0][0]; for(i=1;i<N;++i) if(kb->mindiagC>kb->C[i][i]) kb->mindiagC=kb->C[i][i];

    /* set xmean */
    for (i = 0; i < N; ++i)
        kb->rgxmean[i] = kb->rgxold[i] = (*kb)[i]->_initialX;


    return (kb->publicFitness);

} /* cmaes_init_final() */




/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * const * cmaes_SamplePopulation()
{
    int iNk, i, j, N=kb->_dimCount;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= kb->gen));
    double sum;
    double const *xmean = kb->rgxmean;

    /* calculate eigensystem  */
    if (!kb->flgEigensysIsUptodate) {
        if (!flgdiag)
            cmaes_UpdateEigensystem(0);
        else {
            for (i = 0; i < N; ++i)
                kb->rgD[i] = sqrt(kb->C[i][i]);
            kb->minEW = rgdouMin(kb->rgD, N) * rgdouMin(kb->rgD, N);
            kb->maxEW = rgdouMax(kb->rgD, N) * rgdouMin(kb->rgD, N);
            kb->flgEigensysIsUptodate = 1;
        }
    }

    /* treat minimal standard deviations and numeric problems */
    TestMinStdDevs();

    for (iNk = 0; iNk < kb->_lambda; ++iNk)
    { /* generate scaled cmaes_random vector (D * z)    */
        for (i = 0; i < N; ++i)
            if (flgdiag)
                kb->rgrgx[iNk][i] = xmean[i] + kb->sigma * kb->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
            else
                kb->rgdTmp[i] = kb->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
        if (!flgdiag)
            /* add mutation (sigma * B * (D*z)) */
            for (i = 0; i < N; ++i) {
                for (j = 0, sum = 0.; j < N; ++j)
                    sum += kb->B[i][j] * kb->rgdTmp[j];
                kb->rgrgx[iNk][i] = xmean[i] + kb->sigma * sum;
            }
    }
    if(kb->state == 3 || kb->gen == 0)
        ++kb->gen;
    kb->state = 1;

    return(kb->rgrgx);
} /* SamplePopulation() */


double * const * cmaes_ReSampleSingle(int iindex)
{
    int i, j, N=kb->_dimCount;
    double *rgx; 
    double sum; 
    char s[99];

    if (iindex < 0 || iindex >= kb->_lambda) {
        sprintf(s, "index==%d must be between 0 and %d", iindex, kb->_lambda);
        fprintf(stderr, "[CMAES] Error: cmaes_ReSampleSingle(): Population member ",s,0,0);
    }
    rgx = kb->rgrgx[iindex];

    for (i = 0; i < N; ++i)
        kb->rgdTmp[i] = kb->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
    /* add mutation (sigma * B * (D*z)) */
    for (i = 0; i < N; ++i) {
        for (j = 0, sum = 0.; j < N; ++j)
            sum += kb->B[i][j] * kb->rgdTmp[j];
        rgx[i] = kb->rgxmean[i] + kb->sigma * sum;
    }
    return(kb->rgrgx);
}

double * cmaes_SampleSingleInto(double *rgx)
{
    int i, j, N=kb->_dimCount;
    double sum; 

    if (rgx == NULL)
        rgx = (double*) calloc (sizeof(double), N);

    for (i = 0; i < N; ++i)
        kb->rgdTmp[i] = kb->rgD[i] * kb->_gaussianGenerator->getRandomNumber();
    /* add mutation (sigma * B * (D*z)) */
    for (i = 0; i < N; ++i) {
        for (j = 0, sum = 0.; j < N; ++j)
            sum += kb->B[i][j] * kb->rgdTmp[j];
        rgx[i] = kb->rgxmean[i] + kb->sigma * sum;
    }
    return rgx;
}

double * cmaes_UpdateDistribution(int save_hist, const double *rgFunVal)
{
    int i, j, iNk, hsig, N=kb->_dimCount;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= kb->gen));
    double sum; 
    double psxps; 

    if(kb->state == 3)
        fprintf(stderr, "[CMAES] Error: cmaes_UpdateDistribution(): You need to call \n",
                "SamplePopulation() before update can take place.");
    if(rgFunVal == NULL) 
        fprintf(stderr, "[CMAES] Error: cmaes_UpdateDistribution(): ",
                "Fitness function value array input is missing.");

    if(save_hist && kb->state == 1)  /* function values are delivered here */
        kb->countevals += kb->_lambda;
    else
        fprintf(stderr, "[CMAES] Error: cmaes_UpdateDistribution(): unexpected state");

    /* assign function values */
    for (i=0; i < kb->_lambda; ++i)
        kb->rgrgx[i][N] = kb->rgFuncValue[i] = rgFunVal[i];


    /* Generate index */
    Sorted_index(rgFunVal, kb->index, kb->_lambda);

    /* Test if function values are identical, escape flat fitness */
    if (kb->rgFuncValue[kb->index[0]] ==
            kb->rgFuncValue[kb->index[(int)kb->_lambda/2]]) {
        kb->sigma *= exp(0.2+kb->_sigmaCumulationFactor/kb->_dampFactor);
        fprintf(stderr, "[CMAES] Error: Warning: sigma increased due to equal function values. Reconsider the formulation of the objective function\n");
    }

    /* update function value history */
    if (save_hist) {
        for(i = (int)*(kb->arFuncValueHist-1)-1; i > 0; --i)
            kb->arFuncValueHist[i] = kb->arFuncValueHist[i-1];
        kb->arFuncValueHist[0] = rgFunVal[kb->index[0]];
    }
    
    /* update xbestever */
    if (save_hist && (kb->rgxbestever[N] > kb->rgrgx[kb->index[0]][N] || kb->gen == 1))
        for (i = 0; i <= N; ++i) {
            kb->rgxbestever[i] = kb->rgrgx[kb->index[0]][i];
            kb->rgxbestever[N+1] = kb->countevals;
        }

    /* calculate xmean and rgBDz~N(0,C) */
    for (i = 0; i < N; ++i) {
        kb->rgxold[i] = kb->rgxmean[i];
        kb->rgxmean[i] = 0.;
        for (iNk = 0; iNk < kb->_mu; ++iNk)
            kb->rgxmean[i] += kb->_muWeights[iNk] * kb->rgrgx[kb->index[iNk]][i];
        kb->rgBDz[i] = sqrt(kb->_muEffective)*(kb->rgxmean[i] - kb->rgxold[i])/kb->sigma;
    }

    /* calculate z := D^(-1) * B^(-1) * rgBDz into rgdTmp */
    for (i = 0; i < N; ++i) {
        if (!flgdiag)
            for (j = 0, sum = 0.; j < N; ++j)
                sum += kb->B[j][i] * kb->rgBDz[j];
        else
            sum = kb->rgBDz[i];
        kb->rgdTmp[i] = sum / kb->rgD[i];
    }

    /* TODO?: check length of kb->rgdTmp and set an upper limit, e.g. 6 stds */
    /* in case of manipulation of arx, 
       this can prevent an increase of sigma by several orders of magnitude
       within one step; a five-fold increase in one step can still happen. 
       */ 
    /*
       for (j = 0, sum = 0.; j < N; ++j)
       sum += kb->rgdTmp[j] * kb->rgdTmp[j];
       if (sqrt(sum) > chiN + 6. * sqrt(0.5)) {
       rgdTmp length should be set to upper bound and hsig should become zero 
       }
       */

    /* cumulation for sigma (ps) using B*z */
    for (i = 0; i < N; ++i) {
        if (!flgdiag)
            for (j = 0, sum = 0.; j < N; ++j)
                sum += kb->B[i][j] * kb->rgdTmp[j];
        else
            sum = kb->rgdTmp[i];
        kb->rgps[i] = (1. - kb->_sigmaCumulationFactor) * kb->rgps[i] +
            sqrt(kb->_sigmaCumulationFactor * (2. - kb->_sigmaCumulationFactor)) * sum;
    }

    /* calculate norm(ps)^2 */
    for (i = 0, psxps = 0.; i < N; ++i)
        psxps += kb->rgps[i] * kb->rgps[i];

    /* cumulation for covariance matrix (pc) using B*D*z~N(0,C) */
    hsig = sqrt(psxps) / sqrt(1. - pow(1.-kb->_sigmaCumulationFactor, 2*kb->gen)) / kb->chiN
        < 1.4 + 2./(N+1);
    for (i = 0; i < N; ++i) {
        kb->rgpc[i] = (1. - kb->_cumulativeCovariance) * kb->rgpc[i] +
            hsig * sqrt(kb->_cumulativeCovariance * (2. - kb->_cumulativeCovariance)) * kb->rgBDz[i];
    }

    /* update of C  */

    Adapt_C2(hsig);

    /* update of sigma */
    kb->sigma *= exp(((sqrt(psxps)/kb->chiN)-1.)*kb->_sigmaCumulationFactor/kb->_dampFactor);

    kb->state = 3;

    return (kb->rgxmean);

} /* cmaes_UpdateDistribution() */

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void Adapt_C2(int hsig)
{
    int i, j, k, N=kb->_dimCount;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= kb->gen));

    if (kb->_covarianceMatrixLearningRate != 0.0) {

        /* definitions for speeding up inner-most loop */
        double ccov1 = std::min(kb->_covarianceMatrixLearningRate * (1./kb->_muCovariance) * (flgdiag ? (N+1.5) / 3. : 1.), 1.);
        double ccovmu = std::min(kb->_covarianceMatrixLearningRate * (1-1./kb->_muCovariance)* (flgdiag ? (N+1.5) / 3. : 1.), 1.-ccov1);
        double sigmasquare = kb->sigma * kb->sigma;

        kb->flgEigensysIsUptodate = 0;

        /* update covariance matrix */
        for (i = 0; i < N; ++i)
            for (j = flgdiag ? i : 0; j <= i; ++j) {
                kb->C[i][j] = (1 - ccov1 - ccovmu) * kb->C[i][j]
                    + ccov1
                    * (kb->rgpc[i] * kb->rgpc[j]
                            + (1-hsig)*kb->_cumulativeCovariance*(2.-kb->_cumulativeCovariance) * kb->C[i][j]);
                for (k = 0; k < kb->_mu; ++k) { /* additional rank mu update */
                    kb->C[i][j] += ccovmu * kb->_muWeights[k]
                        * (kb->rgrgx[kb->index[k]][i] - kb->rgxold[i])
                        * (kb->rgrgx[kb->index[k]][j] - kb->rgxold[j])
                        / sigmasquare;
                }
            }
        /* update maximal and minimal diagonal value */
        kb->maxdiagC = kb->mindiagC = kb->C[0][0];
        for (i = 1; i < N; ++i) {
            if (kb->maxdiagC < kb->C[i][i])
                kb->maxdiagC = kb->C[i][i];
            else if (kb->mindiagC > kb->C[i][i])
                kb->mindiagC = kb->C[i][i];
        }
    } /* if ccov... */
}


void TestMinStdDevs()
    /* increases sigma */
{
    int i, N = kb->_dimCount;

    for (i = 0; i < N; ++i)
        while (kb->sigma * sqrt(kb->C[i][i]) < kb->_dims[i]._minStdDevChange)
            kb->sigma *= exp(0.05+kb->_sigmaCumulationFactor/kb->_dampFactor);

} /* cmaes_TestMinStdDevs() */



/* --------------------------------------------------------- */
void cmaes_PrintResults()

    /* this hack reads key words from input key for data to be written to
     * a file, see file signals.par as input file. The length of the keys
     * is mostly fixed, see key += number in the code! If the key phrase
     * does not match the expectation the output might be strange.  for
     * must be zero terminated.
     */
{ 
    int i, k, N=kb->_dimCount;

		printf(" N %d\n", N);
		printf(" seed %d\n", kb->_seed);
		printf("function evaluations %.0f\n", kb->countevals);
		printf("function value f(x)=%g\n", kb->rgrgx[kb->index[0]][N]);
		printf("maximal standard deviation %g\n", kb->sigma*sqrt(kb->maxdiagC));
		printf("minimal standard deviation %g\n", kb->sigma*sqrt(kb->mindiagC));
		printf("sigma %g\n", kb->sigma);
		printf("axisratio %g\n", rgdouMax(kb->rgD, N)/rgdouMin(kb->rgD, N));
		printf("xbestever found after %.0f evaluations, function value %g\n",	kb->rgxbestever[N+1], kb->rgxbestever[N]);

		for(i=0; i<N; ++i) printf(" %12g%c", kb->rgxbestever[i], (i%5==4||i==N-1)?'\n':' ');

		printf("xbest (of last generation, function value %g)\n",	kb->rgrgx[kb->index[0]][N]);

		for(i=0; i<N; ++i) printf(" %12g%c", kb->rgrgx[kb->index[0]][i],(i%5==4||i==N-1)?'\n':' ');

		printf("xmean \n");
		for(i=0; i<N; ++i) printf(" %12g%c", kb->rgxmean[i],	(i%5==4||i==N-1)?'\n':' ');

		printf("Standard deviation of coordinate axes (sigma*sqrt(diag(C)))\n");
		for(i=0; i<N; ++i) printf(" %12g%c", kb->sigma*sqrt(kb->C[i][i]),	(i%5==4||i==N-1)?'\n':' ');

		printf("Main axis lengths of mutation ellipsoid (sigma*diag(D))\n");
		for (i = 0; i < N; ++i)	kb->rgdTmp[i] = kb->rgD[i];
		for(i=0; i<N; ++i)	printf(" %12g%c", kb->sigma*kb->rgdTmp[N-1-i],(i%5==4||i==N-1)?'\n':' ');

		printf("Longest axis (b_i where d_ii=max(diag(D))\n");
		k = MaxIdx(kb->rgD, N);

		for(i=0; i<N; ++i) printf(" %12g%c", kb->B[i][k], (i%5==4||i==N-1)?'\n':' ');
		printf("Shortest axis (b_i where d_ii=max(diag(D))\n");
		k = MinIdx(kb->rgD, N);

		for(i=0; i<N; ++i) printf(" %12g%c", kb->B[i][k], (i%5==4||i==N-1)?'\n':' ');
}

double function_value_difference() {
    return std::max(rgdouMax(kb->arFuncValueHist, (int)std::min(kb->gen,*(kb->arFuncValueHist-1))),
    		rgdouMax(kb->rgFuncValue, kb->_lambda)) -
        std::min(rgdouMin(kb->arFuncValueHist, (int)std::min(kb->gen, *(kb->arFuncValueHist-1))),
               rgdouMin(kb->rgFuncValue, kb->_lambda));
}

bool cmaes_TestForTermination()
{

    double range, fac;
    int iAchse, iKoo;
    int flgdiag = ((kb->_diagonalCovarianceMatrixEvalFrequency== 1) || (kb->_diagonalCovarianceMatrixEvalFrequency>= kb->gen));
    int i, cTemp, N=kb->_dimCount;
    bool terminate = false;

    /* function value reached */
    if ((kb->gen > 1 || kb->state > 1) &&   kb->rgFuncValue[kb->index[0]] <= kb->_stopMinFitness)
    {
        terminate = true; printf( "Fitness: function value %7.2e <= stopFitness (%7.2e)\n",
                kb->rgFuncValue[kb->index[0]], kb->_stopMinFitness);
    }

    /* TolFun */
    range = function_value_difference();

    if (kb->gen > 0 && range <= kb->_stopFitnessDiffThreshold) {
        terminate = true; printf(
                "TolFun: function value differences %7.2e < kb->_stopFitnessDiffThreshold=%7.2e\n",
                range, kb->_stopFitnessDiffThreshold);
    }

    /* TolFunHist */
    if (kb->gen > *(kb->arFuncValueHist-1)) {
        range = rgdouMax(kb->arFuncValueHist, (int)*(kb->arFuncValueHist-1))
            - rgdouMin(kb->arFuncValueHist, (int)*(kb->arFuncValueHist-1));
        if (range <= kb->_stopFitnessDiffHistoryThreshold)
        {
            terminate = true; printf(
                    "TolFunHist: history of function value changes %7.2e kb->_stopFitnessDiffHistoryThreshold=%7.2e",
                    range, kb->_stopFitnessDiffHistoryThreshold);
        }
    }

    /* TolX */
    for(i=0, cTemp=0; i<N; ++i) {
        cTemp += (kb->sigma * sqrt(kb->C[i][i]) < kb->_stopMinDeltaX) ? 1 : 0;
        cTemp += (kb->sigma * kb->rgpc[i] < kb->_stopMinDeltaX) ? 1 : 0;
    }
    if (cTemp == 2*N) {
        terminate = true; printf(
                "TolX: object variable changes below %7.2e \n", 
                kb->_stopMinDeltaX);
    }

    /* TolUpX */
    for(i=0; i<N; ++i) {
        if (kb->sigma * sqrt(kb->C[i][i]) > kb->_stopMaxStdDevXFactor * kb->_dims[i]._initialStdDev)
            break;
    }

    if (i < N) {
        terminate = true; printf(
                "TolUpX: standard deviation increased by more than %7.2e, larger initial standard deviation recommended \n", 
								kb->_stopMaxStdDevXFactor);
    }

    /* Condition of C greater than dMaxSignifKond */
    if (kb->maxEW >= kb->minEW * kb->dMaxSignifKond) {
        terminate = true; printf(
                "ConditionNumber: maximal condition number %7.2e reached. maxEW=%7.2e,minEW=%7.2e,maxdiagC=%7.2e,mindiagC=%7.2e\n",
                kb->dMaxSignifKond, kb->maxEW, kb->minEW, kb->maxdiagC, kb->mindiagC);
    } /* if */

    /* Principal axis i has no effect on xmean, ie. 
       x == x + 0.1 * sigma * rgD[i] * B[i] */
    if (!flgdiag) {
        for (iAchse = 0; iAchse < N; ++iAchse)
        {
            fac = 0.1 * kb->sigma * kb->rgD[iAchse];
            for (iKoo = 0; iKoo < N; ++iKoo){ 
                if (kb->rgxmean[iKoo] != kb->rgxmean[iKoo] + fac * kb->B[iKoo][iAchse])
                    break;
            }
            if (iKoo == N)        
            {
                /* kb->sigma *= exp(0.2+kb->_sigmaCumulationFactor/kb->_dampFactor); */
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
        if (kb->rgxmean[iKoo] == kb->rgxmean[iKoo] +
                0.2*kb->sigma*sqrt(kb->C[iKoo][iKoo]))
        {
            /* kb->C[iKoo][iKoo] *= (1 + kb->_covarianceMatrixLearningRate); */
            /* flg = 1; */
            terminate = true; printf(
                    "NoEffectCoordinate: standard deviation 0.2*%7.2e in coordinate %d without effect\n", 
                    kb->sigma*sqrt(kb->C[iKoo][iKoo]), iKoo);
            break;
        }

    } /* for iKoo */
    /* if (flg) kb->sigma *= exp(0.05+kb->_sigmaCumulationFactor/kb->_dampFactor); */

    if(kb->countevals >= kb->_maxFitnessEvaluations)
    {
        terminate = true; printf( "MaxFunEvals: conducted function evaluations %.0f >= %lu\n",
                kb->countevals, kb->_maxFitnessEvaluations); }

    if(kb->gen >= kb->_maxGenerations)
    {
        terminate = true; printf( "MaxIter: number of iterations %.0f >= %lu\n",
                kb->gen, kb->_maxGenerations);
    }

    return terminate;

} /* cmaes_Test() */


void cmaes_UpdateEigensystem(int flgforce)
{
    int N = kb->_dimCount;

    if(flgforce == 0) if (kb->flgEigensysIsUptodate == 1) return;

    Eigen( N, kb->C, kb->rgD, kb->B, kb->rgdTmp);

    /* find largest and smallest eigenvalue, they are supposed to be sorted anyway */
    kb->minEW = rgdouMin(kb->rgD, N);
    kb->maxEW = rgdouMax(kb->rgD, N);

    for (int i = 0; i < N; ++i) 	kb->rgD[i] = sqrt(kb->rgD[i]);

    kb->flgEigensysIsUptodate = 1;
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



