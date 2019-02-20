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
#include "auxiliar.hpp"

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
		double diff = pd->temp[i] - s.U[p*s.N+q];
		ssn += diff*diff;
		//printf("Data: %d - Value: [%.4f, %.4f] %.20f = %.20f\n", i, pd->xPos[i], pd->yPos[i], pd->temp[i], s.U[p*s.N+q]);
	}

	double res = 0.5* (nData*log(2*M_PI) + nData*log(sigma2) + ssn/sigma2);

	printf("Sigma: %.11f - Res: %f\n", s.pars[0], res);

	free(s.U);
	free(s.f);
	free(s.smoothingTime);
	free(s.residualTime);
	free(s.restrictionTime);
	free(s.prolongTime);
	free(s.L2NormTime);

	return res;
}

Heat2DSetup::Heat2DSetup()
{
	iteration = 0;
  L2Norm = 0.0;
  L2NormPrev = std::numeric_limits<double>::max();
  L2NormDiff = std::numeric_limits<double>::max();
}

void Heat2DSetup::printResults()
{
	double* timePerGrid = (double*) calloc (sizeof(double), gridCount);
	double* timePerOp = (double*) calloc (sizeof(double), 5);
	double totalSmoothingTime = 0.0;
	double totalResidualTime = 0.0;
	double totalRestrictionTime = 0.0;
	double totalProlongTime = 0.0;
	double totalL2NormTime = 0.0;

	for (int i = 0; i < gridCount; i++) timePerGrid[i] = smoothingTime[i] + residualTime[i] + restrictionTime[i] + prolongTime[i] + L2NormTime[i];
	for (int i = 0; i < gridCount; i++) totalSmoothingTime += smoothingTime[i];
	for (int i = 0; i < gridCount; i++) totalResidualTime += residualTime[i];
	for (int i = 0; i < gridCount; i++) totalRestrictionTime += restrictionTime[i];
	for (int i = 0; i < gridCount; i++) totalProlongTime += prolongTime[i];
	for (int i = 0; i < gridCount; i++) totalL2NormTime += L2NormTime[i];

	double totalMeasured = totalSmoothingTime + totalResidualTime + totalRestrictionTime + totalProlongTime + totalL2NormTime;

	printf("   Time (s)    "); for (int i = 0; i < gridCount; i++) printf("Grid%d   ", i);                    printf("   Total  \n");
	printf("-------------|-"); for (int i = 0; i < gridCount; i++) printf("--------"); printf("|---------\n");
	printf("Smoothing    | "); for (int i = 0; i < gridCount; i++) printf("%2.3f   ", smoothingTime[i]);    printf("|  %2.3f  \n", totalSmoothingTime);
	printf("Residual     | "); for (int i = 0; i < gridCount; i++) printf("%2.3f   ", residualTime[i]);     printf("|  %2.3f  \n", totalResidualTime);
	printf("Restriction  | "); for (int i = 0; i < gridCount; i++) printf("%2.3f   ", restrictionTime[i]);  printf("|  %2.3f  \n", totalRestrictionTime);
	printf("Prolongation | "); for (int i = 0; i < gridCount; i++) printf("%2.3f   ", prolongTime[i]);      printf("|  %2.3f  \n", totalProlongTime);
	printf("L2Norm       | "); for (int i = 0; i < gridCount; i++) printf("%2.3f   ", L2NormTime[i]);       printf("|  %2.3f  \n", totalL2NormTime);
	printf("-------------|-"); for (int i = 0; i < gridCount; i++) printf("--------"); printf("|---------\n");
	printf("Total        | "); for (int i = 0; i < gridCount; i++) printf("%2.3f   ", timePerGrid[i]); printf("|  %2.3f  \n", totalMeasured);
	printf("-------------|-"); for (int i = 0; i < gridCount; i++) printf("--------"); printf("|---------\n");
	printf("\n");
	printf("Fine grid elements: %lu x %lu (n = %lu)\n", N, N, N0);
	printf("V-Cycle Iterations: %d\n", iteration);
	printf("Final L2 Residual : %e\n", L2Norm);
	printf("Convergence Rate  : %e\n", L2NormDiff);
	printf("Running Time      : %.3fs\n", totalTime);

	free(timePerGrid);
	free(timePerOp);
}

void Heat2DSetup::setGridCount(int count)
{
  gridCount = count;

  smoothingTime   = (double*) calloc (sizeof(double), gridCount);
  residualTime    = (double*) calloc (sizeof(double), gridCount);
  restrictionTime = (double*) calloc (sizeof(double), gridCount);
  prolongTime     = (double*) calloc (sizeof(double), gridCount);
  L2NormTime      = (double*) calloc (sizeof(double), gridCount);

  for (int i = 0; i < gridCount; i++) smoothingTime[i] = 0.0;
  for (int i = 0; i < gridCount; i++) residualTime[i] = 0.0;
  for (int i = 0; i < gridCount; i++) restrictionTime[i] = 0.0;
  for (int i = 0; i < gridCount; i++) prolongTime[i] = 0.0;
  for (int i = 0; i < gridCount; i++) L2NormTime[i] = 0.0;
}

void Heat2DSetup::applyGaussSeidel_(GridLevel* g, int l, int relaxations)
{
	auto t0 = std::chrono::system_clock::now();
	applyGaussSeidel(g, l, relaxations);
	auto t1 = std::chrono::system_clock::now();
  smoothingTime[l] += std::chrono::duration<double>(t1-t0).count();
}

void Heat2DSetup::applyProlongation_(GridLevel* g, int l)
{
  auto t0 = std::chrono::system_clock::now();
  applyProlongation(g, l);
	auto t1 = std::chrono::system_clock::now();
	prolongTime[l-1] += std::chrono::duration<double>(t1-t0).count();
}

void Heat2DSetup::applyRestriction_(GridLevel* g, int l)
{
  auto t0 = std::chrono::system_clock::now();
  applyRestriction(g, l);
	auto t1 = std::chrono::system_clock::now();
	restrictionTime[l-1] += std::chrono::duration<double>(t1-t0).count();
}

void Heat2DSetup::calculateResidual_(GridLevel* g, int l)
{
  auto t0 = std::chrono::system_clock::now();
  calculateResidual(g, l);
	auto t1 = std::chrono::system_clock::now();
	residualTime[l] += std::chrono::duration<double>(t1-t0).count();
}

void Heat2DSetup::calculateL2Norm_(GridLevel* g, int l)
{
  auto t0 = std::chrono::system_clock::now();
	L2Norm = calculateL2Norm(g, l);
	auto t1 = std::chrono::system_clock::now();
	L2NormTime[l] += std::chrono::duration<double>(t1-t0).count();

	L2NormDiff = abs(L2NormPrev - L2Norm);
  L2NormPrev = L2Norm;
	iteration++;
}

void Heat2DSetup::generateInitialConditions(double c1, double c3, double c4)
{
	N0 = 8; // 2^N0 + 1 elements per side
	N = pow(2, N0) + 1;
	U = (double*) calloc (sizeof(double), N*N);
	f = (double*) calloc (sizeof(double), N*N);
	tolerance = 1e-8;
	double c2 = 0.05; // width

	// Initial Guess
	for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) U[i*N + j] = 1.0;

	// Boundary Conditions
	for (int i = 0; i < N; i++) U[i]           = 0.0;
	for (int i = 0; i < N; i++) U[(N-1)*N + i] = 0.0;
	for (int i = 0; i < N; i++) U[i*N]         = 0.0;
	for (int i = 0; i < N; i++) U[i*N + N-1]   = 0.0;

	// F
	for (int i = 0; i < N; i++) for (int j = 0; j < N; j++)
	{
		double h = 1.0/(N-1);
		double x = i*h;
		double y = j*h;

		f[i*N +j] = 0.0;

		// Heat Source: Candle
		f[i*N + j] += -(4*c1*exp( -(pow(c3 - y, 2) + pow(c4 - x, 2)) / c2 ) * (pow(c3,2) - 2*c3*y + pow(c4,2) - 2*c4*x + pow(y,2) + pow(x,2) - c2))/pow(c2,2);
	}
}

