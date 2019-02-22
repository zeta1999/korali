/**********************************************************************/
// A now optimized Multigrid Solver for the Heat Equation             //
// Course Material for HPCSE-II, Spring 2019, ETH Zurich              //
// Authors: Sergio Martin, Georgios Arampatzis                        //
// License: Use if you like, but give us credit.                      //
/**********************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "heat2d.hpp"

double heat2DSolver(double* pars, int n, void* data)
{
	// Problem Parameters
	double intensity = pars[1];
	double xPos = pars[2];
	double yPos = pars[3];

	double L2Norm = 0.0; // L2 Norm of the residual
  double L2NormPrev = std::numeric_limits<double>::max(); // Previous L2 Norm
  double L2NormDiff = std::numeric_limits<double>::max(); // L2Norm Difference compared to previous step
  double tolerance = 1e-8; // L2 Difference Tolerance before reaching convergence.

  size_t N0 = 7; // 2^N0 + 1 elements per side
  size_t N = pow(2, N0) + 1;

	// Multigrid parameters -- Find the best configuration!
	int gridCount       = 6;     // Number of Multigrid levels to use
	int downRelaxations = 4; // Number of Relaxations before restriction
	int upRelaxations   = 1;   // Number of Relaxations after prolongation

	// Allocating Grids
	gridLevel* g = (gridLevel*) _mm_malloc(sizeof(gridLevel) * gridCount, 64);
	for (int i = 0; i < gridCount; i++)
	{
		g[i].N = pow(2, N0-i) + 1;
		g[i].h = 1.0/(g[i].N-1);

		g[i].U   = (double**) _mm_malloc(sizeof(double*) * g[i].N, 16); for (int j = 0; j < g[i].N ; j++)	g[i].U[j]   = (double*) _mm_malloc(sizeof(double) * g[i].N, 16);
		g[i].Res = (double**) _mm_malloc(sizeof(double*) * g[i].N, 16); for (int j = 0; j < g[i].N ; j++)	g[i].Res[j] = (double*) _mm_malloc(sizeof(double) * g[i].N, 16);
		g[i].f   = (double**) _mm_malloc(sizeof(double*) * g[i].N, 16); for (int j = 0; j < g[i].N ; j++)	g[i].f[j]   = (double*) _mm_malloc(sizeof(double) * g[i].N, 16);
	}

	generateInitialConditions(N, intensity, xPos, yPos, g[0].U, g[0].f);

	while (L2NormDiff > tolerance)  // Multigrid solver start
	{
		applyGaussSeidel(g, 0, downRelaxations); // Relaxing the finest grid first
		calculateResidual(g, 0); // Calculating Initial Residual

		for (int grid = 1; grid < gridCount; grid++) // Going down the V-Cycle
		{
			applyRestriction(g, grid); // Restricting the residual to the coarser grid's solution vector (f)
			applyGaussSeidel(g, grid, downRelaxations); // Smoothing coarser level
			calculateResidual(g, grid); // Calculating Coarse Grid Residual
		}

		for (int grid = gridCount-1; grid > 0; grid--) // Going up the V-Cycle
		{
			applyProlongation(g, grid); // Prolonging solution for coarser level up to finer level
			applyGaussSeidel(g, grid, upRelaxations); // Smoothing finer level
		}

		L2Norm = calculateL2Norm(g, 0); // Calculating Residual L2 Norm
		L2NormDiff = abs(L2NormPrev - L2Norm);
	  L2NormPrev = L2Norm;
	}  // Multigrid solver end

	pointsInfo* pd = (pointsInfo*) data;
	double h = 1.0/(N-1);
	for(int i = 0; i < pd->nPoints; i++)
	{
		int p = ceil(pd->xPos[i]/h);	int q = ceil(pd->yPos[i]/h);
		pd->simTemp[i] = g[0].U[p][q];
	}

  // Freeing grids
	for (int i = 0; i < gridCount; i++)
	{
		for (int j = 0; j < g[i].N ; j++) _mm_free(g[i].U[j]);
		for (int j = 0; j < g[i].N ; j++) _mm_free(g[i].f[j]);
		for (int j = 0; j < g[i].N ; j++) _mm_free(g[i].Res[j]);
		_mm_free(g[i].U);
		_mm_free(g[i].f);
		_mm_free(g[i].Res);
	}
	_mm_free(g);

	return Korali::GaussianDistribution::getError(pars[0], pd->nPoints, pd->refTemp, pd->simTemp);
}

void applyGaussSeidel(gridLevel* g, int l, int relaxations)
{
 double h2 = g[l].h*g[l].h;
 for (int r = 0; r < relaxations; r++)
 {
   for (int j = 1; j < g[l].N - 1; j++) // Gauss-Seidel Iteration -- Credit: Claudio Cannizzaro
   #pragma ivdep
   #pragma vector aligned
    for (int i = 1; i < g[l].N - 1; i++)
     g[l].U[i][j] = ((g[l].U[i - 1][j] + g[l].U[i + 1][j] + g[l].U[i][j - 1] + g[l].U[i][j + 1]) +  g[l].f[i][j] * h2 ) * 0.25;
 }
}

void calculateResidual(gridLevel* g, int l)
{
	double h2 = 1.0 / pow(g[l].h,2);

 #pragma vector aligned
 for (int i = 1; i < g[l].N-1; i++)
	#pragma ivdep
	for (int j = 1; j < g[l].N-1; j++)
	g[l].Res[i][j] = g[l].f[i][j] + (g[l].U[i-1][j] + g[l].U[i+1][j] - 4*g[l].U[i][j] + g[l].U[i][j-1] + g[l].U[i][j+1]) * h2;
}

double calculateL2Norm(gridLevel* g, int l)
{
  double tmp = 0.0;

	for (int i = 0; i < g[l].N; i++)
	 #pragma ivdep
	 #pragma vector aligned
	 for (int j = 0; j < g[l].N; j++)
		 g[l].Res[i][j] = g[l].Res[i][j]*g[l].Res[i][j];

	for (int i = 0; i < g[l].N; i++)
	 #pragma ivdep
	 #pragma vector aligned
	 for (int j = 0; j < g[l].N; j++)
		 tmp += g[l].Res[i][j];

 return sqrt(tmp);
}

void applyRestriction(gridLevel* g, int l)
{
	for (int i = 1; i < g[l].N-1; i++)
   #pragma ivdep
   #pragma vector aligned
	 for (int j = 1; j < g[l].N-1; j++)
				 g[l].f[i][j] = ( 1.0*( g[l-1].Res[2*i-1][2*j-1] + g[l-1].Res[2*i-1][2*j+1] + g[l-1].Res[2*i+1][2*j-1]   + g[l-1].Res[2*i+1][2*j+1] )   +
													2.0*( g[l-1].Res[2*i-1][2*j]   + g[l-1].Res[2*i][2*j-1]   + g[l-1].Res[2*i+1][2*j]     + g[l-1].Res[2*i][2*j+1] ) +
													4.0*( g[l-1].Res[2*i][2*j] ) ) * 0.0625;

	for (int i = 0; i < g[l].N; i++)
   #pragma ivdep
   #pragma vector aligned
	 for (int j = 0; j < g[l].N; j++) // Resetting U vector for the coarser level before smoothing -- Find out if this is really necessary.
		g[l].U[i][j] = 0;
}

void applyProlongation(gridLevel* g, int l)
{
	for (int i = 1; i < g[l].N-1; i++)
   #pragma ivdep
	 for (int j = 1; j < g[l].N-1; j++)
			g[l-1].U[2*i][2*j] += g[l].U[i][j];

  #pragma vector aligned
	for (int i = 1; i < g[l].N; i++)
   #pragma ivdep
	 for (int j = 1; j < g[l].N-1; j++)
			g[l-1].U[2*i-1][2*j] += ( g[l].U[i-1][j] + g[l].U[i][j] ) *0.5;

  #pragma vector aligned
	for (int i = 1; i < g[l].N-1; i++)
   #pragma ivdep
	 for (int j = 1; j < g[l].N; j++)
			g[l-1].U[2*i][2*j-1] += ( g[l].U[i][j-1] + g[l].U[i][j] ) *0.5;

  #pragma vector aligned
	for (int i = 1; i < g[l].N; i++)
   #pragma ivdep
	 for (int j = 1; j < g[l].N; j++)
			g[l-1].U[2*i-1][2*j-1] += ( g[l].U[i-1][j-1] + g[l].U[i-1][j] + g[l].U[i][j-1] + g[l].U[i][j] ) *0.25;
}

void generateInitialConditions(size_t N, double c1, double c2, double c3, double** U, double** f)
{
	double width = 0.05; // width

	// Initial Guess
	for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) U[i][j] = 1.0;

	// Boundary Conditions
	for (int i = 0; i < N; i++) U[0][i]   = 0.0;
	for (int i = 0; i < N; i++) U[N-1][i] = 0.0;
	for (int i = 0; i < N; i++) U[i][0]   = 0.0;
	for (int i = 0; i < N; i++) U[i][N-1] = 0.0;

	// F
	for (int i = 0; i < N; i++)
	for (int j = 0; j < N; j++)
	{
		double h = 1.0/(N-1);
		double x = i*h;
		double y = j*h;

		f[i][j] = 0.0;

		// Heat Source: Candle
		f[i][j] += -(4*c1*exp( -(pow(c2 - y, 2) + pow(c3 - x, 2)) / width ) * (pow(c2,2) - 2*c2*y + pow(c3,2) - 2*c3*x + pow(y,2) + pow(x,2) - width))/pow(width,2);
	}
}
