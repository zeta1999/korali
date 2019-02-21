/**********************************************************************/
// A still unoptimized Multigrid Solver for the Heat Equation         //
// Course Material for HPCSE-II, Spring 2019, ETH Zurich              //
// Authors: Sergio Martin, Georgios Arampatzis                        //
// License: Use if you like, but give us credit.                      //
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits>
#include <chrono>
#include "heat2d.hpp"

double heat2DWrapper(double* pars, int n, void* data)
{
	// User-defined Parameters
	Heat2DSetup s;
  s.pars = pars;

	// Problem Parameters
  auto start = std::chrono::system_clock::now();
	heat2DSolver(s);
  auto end = std::chrono::system_clock::now();
	s.totalTime = std::chrono::duration<double>(end-start).count();

	pointsInfo* pd = (pointsInfo*) data;

	double h = 1.0/(s.N-1);
	double sigma2 = s.pars[0]*s.pars[0];
	double ssn = 0.0;
	int nData = pd->nPoints;

	for(int i = 0; i < nData; i++)
	{
		int p = ceil(pd->xPos[i]/h);	int q = ceil(pd->yPos[i]/h);
		double diff = pd->temp[i] - s.U[p][q];
		ssn += diff*diff;
		//printf("Data: %d - Value: [%.4f, %.4f] %.20f = %.20f\n", i, pd->xPos[i], pd->yPos[i], pd->temp[i], s.U[p*s.N+q]);
	}

	double res = 0.5* (nData*log(2*M_PI) + nData*log(sigma2) + ssn/sigma2);

	printf("Sigma: %.11f - Res: %f\n", s.pars[0], res);

	return res;
}

Heat2DSetup::Heat2DSetup()
{
	iteration = 0;
  L2Norm = 0.0;
  L2NormPrev = std::numeric_limits<double>::max();
  L2NormDiff = std::numeric_limits<double>::max();
}

Heat2DSetup::~Heat2DSetup()
{
	for (int i = 0; i < N ; i++) _mm_free(U[i]);
	for (int i = 0; i < N ; i++) _mm_free(f[i]);
	_mm_free(U);
	_mm_free(f);
}

void Heat2DSetup::printResults()
{
	printf("Fine grid elements: %lu x %lu (n = %lu)\n", N, N, N0);
	printf("V-Cycle Iterations: %d\n", iteration);
	printf("Final L2 Residual : %e\n", L2Norm);
	printf("Convergence Rate  : %e\n", L2NormDiff);
	printf("Running Time      : %.3fs\n", totalTime);
}

