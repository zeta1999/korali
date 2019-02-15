#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits>
#include <chrono>
#include "auxiliar.hpp"

int main(int argc, char** argv)
{
	// User-defined Parameters
	Heat2DSetup s;

	for (int i = 0; i < argc; i++)
	{
			if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--save"))   { s.saveOutput = true; }
			if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))   { s.printHelp(); exit(0); }
	}

  s.loadProblem();

  auto start = std::chrono::system_clock::now();
	heat2DSolver(s);
  auto end = std::chrono::system_clock::now();
	s.totalTime = std::chrono::duration<double>(end-start).count();

	double* timePerGrid = (double*) calloc (sizeof(double), s.gridCount);
	double* timePerOp = (double*) calloc (sizeof(double), 5);
	double totalSmoothingTime = 0.0;
	double totalResidualTime = 0.0;
	double totalRestrictionTime = 0.0;
	double totalProlongTime = 0.0;
	double totalL2NormTime = 0.0;

	for (int i = 0; i < s.gridCount; i++) timePerGrid[i] = s.smoothingTime[i] + s.residualTime[i] + s.restrictionTime[i] + s.prolongTime[i] + s.L2NormTime[i];
	for (int i = 0; i < s.gridCount; i++) totalSmoothingTime += s.smoothingTime[i];
	for (int i = 0; i < s.gridCount; i++) totalResidualTime += s.residualTime[i];
	for (int i = 0; i < s.gridCount; i++) totalRestrictionTime += s.restrictionTime[i];
	for (int i = 0; i < s.gridCount; i++) totalProlongTime += s.prolongTime[i];
	for (int i = 0; i < s.gridCount; i++) totalL2NormTime += s.L2NormTime[i];

	double totalMeasured = totalSmoothingTime + totalResidualTime + totalRestrictionTime + totalProlongTime + totalL2NormTime;

	printf("   Time (s)    "); for (int i = 0; i < s.gridCount; i++) printf("Grid%d   ", i);                    printf("   Total  \n");
	printf("-------------|-"); for (int i = 0; i < s.gridCount; i++) printf("--------"); printf("|---------\n");
	printf("Smoothing    | "); for (int i = 0; i < s.gridCount; i++) printf("%2.3f   ", s.smoothingTime[i]);    printf("|  %2.3f  \n", totalSmoothingTime);
	printf("Residual     | "); for (int i = 0; i < s.gridCount; i++) printf("%2.3f   ", s.residualTime[i]);     printf("|  %2.3f  \n", totalResidualTime);
	printf("Restriction  | "); for (int i = 0; i < s.gridCount; i++) printf("%2.3f   ", s.restrictionTime[i]);  printf("|  %2.3f  \n", totalRestrictionTime);
	printf("Prolongation | "); for (int i = 0; i < s.gridCount; i++) printf("%2.3f   ", s.prolongTime[i]);      printf("|  %2.3f  \n", totalProlongTime);
	printf("L2Norm       | "); for (int i = 0; i < s.gridCount; i++) printf("%2.3f   ", s.L2NormTime[i]);       printf("|  %2.3f  \n", totalL2NormTime);
	printf("-------------|-"); for (int i = 0; i < s.gridCount; i++) printf("--------"); printf("|---------\n");
	printf("Total        | "); for (int i = 0; i < s.gridCount; i++) printf("%2.3f   ", timePerGrid[i]); printf("|  %2.3f  \n", totalMeasured);
	printf("-------------|-"); for (int i = 0; i < s.gridCount; i++) printf("--------"); printf("|---------\n");
	printf("\n");
	printf("Fine grid elements: %lu x %lu (n = %lu)\n", s.N, s.N, s.N0);
	printf("V-Cycle Iterations: %d\n", s.iteration);
	printf("Final L2 Residual : %e\n", s.L2Norm);
	printf("Convergence Rate  : %e\n", s.L2NormDiff);
	printf("Running Time      : %.3fs\n", s.totalTime);

  if (s.saveOutput) s.outputSolution();

	return 0;
}

Heat2DSetup::Heat2DSetup()
{
	iteration = 0;

  saveOutput = false; // Write output to file flag

  L2Norm = 0.0;
  L2NormPrev = std::numeric_limits<double>::max();
  L2NormDiff = std::numeric_limits<double>::max();
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

void Heat2DSetup::applyJacobi_(GridLevel* g, int l, int relaxations)
{
	auto t0 = std::chrono::system_clock::now();
	applyJacobi(g, l, relaxations);
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


void Heat2DSetup::printHelp()
{
    printf("/**********************************************************************/\n");
    printf("// A still unoptimized Multigrid Solver for the Heat Equation         //\n");
    printf("// Course Material for HPCSE-II, Spring 2019, ETH Zurich              //\n");
    printf("// Authors: Sergio Martin, Georgios Arampatzis                        //\n");
    printf("// License: Use if you like, but give us credit.                      //\n");
    printf("/**********************************************************************/\n");
    printf("\n");
    printf("Usage:\n");
    printf("  -s or --save - Saves computed solution problemN.cpt\n");
    printf("  -h  -  Prints this help info.\n");
}

void Heat2DSetup::loadProblem()
{
	N0 = 7; // 2^N0 + 1 elements per side
	N = pow(2, N0) + 1;
	U = (double*) calloc (sizeof(double), N*N);
	f = (double*) calloc (sizeof(double), N*N);
	tolerance = 1e-6;

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
			double c1, c2, c3, c4;
			double h = 1.0/(N-1);
			double x = i*h;
			double y = j*h;

			f[i*N +j] = 0.0;

			// Heat Source: Candle 1
			c1 = 30; // intensity
			c2 = 0.02; // variance
			c3 = 0.1; // x0
			c4 = 0.8; // y0
			f[i*N + j] += -(4*c1*exp( -(pow(c3 - y, 2) + pow(c4 - x, 2)) / c2 ) * (pow(c3,2) - 2*c3*y + pow(c4,2) - 2*c4*x + pow(y,2) + pow(x,2) - c2))/pow(c2,2);
	}
}


void Heat2DSetup::outputSolution()
{
		FILE *fid;
		char pfile[50];
		sprintf(pfile, "solution.out");

    printf("Writing computed solution to file %s...\n", pfile);

    fid = fopen(pfile, "w");

    // U
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fprintf(fid, "%22.15e ", U[i*N + j]);
        }
        fprintf(fid, "\n");
    }

    fclose(fid);
}
