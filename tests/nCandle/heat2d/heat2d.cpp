#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "heat2d.hpp"

class GridLevel
{
public:
	size_t N; // Number of points per dimension in the grid level
	double h; // DeltaX = DeltaY, the distance between points in the discretized [0,1]x[0,1] domain
	double** f; // Right hand side (external heat sources)
	double** U; // Main grid
	double** Res; // Residual Grid
};

void heat2DSolver(Heat2DSetup& s)
{
	// Problem Parameters
	double intensity = s.pars[1];
	double xPos = s.pars[2];
	double yPos = s.pars[3];

	s.generateInitialConditions(intensity, xPos, yPos);

	// Multigrid parameters -- Find the best configuration!
	s.gridCount       = 6;     // Number of Multigrid levels to use
	s.downRelaxations = 4; // Number of Relaxations before restriction
	s.upRelaxations   = 1;   // Number of Relaxations after prolongation

	// Allocating Grids -- Is there a better way to allocate these grids?
	GridLevel* g = (GridLevel*) _mm_malloc(sizeof(GridLevel) * s.gridCount, 64);
	for (int i = 0; i < s.gridCount; i++)
	{
		g[i].N = pow(2, s.N0-i) + 1;
		g[i].h = 1.0/(g[i].N-1);

		g[i].U   = (double**) _mm_malloc(sizeof(double*) * g[i].N, 16); for (int j = 0; j < g[i].N ; j++)	g[i].U[j]   = (double*) _mm_malloc(sizeof(double) * g[i].N, 16);
		g[i].Res = (double**) _mm_malloc(sizeof(double*) * g[i].N, 16); for (int j = 0; j < g[i].N ; j++)	g[i].Res[j] = (double*) _mm_malloc(sizeof(double) * g[i].N, 16);
		g[i].f   = (double**) _mm_malloc(sizeof(double*) * g[i].N, 16); for (int j = 0; j < g[i].N ; j++)	g[i].f[j]   = (double*) _mm_malloc(sizeof(double) * g[i].N, 16);
	}

	// Setting up problem.
	for (int i = 0; i < s.N; i++) for (int j = 0; j < s.N; j++) g[0].U[i][j] = s.U[i][j];
	for (int i = 0; i < s.N; i++) for (int j = 0; j < s.N; j++) g[0].f[i][j] = s.f[i][j];

	while (s.L2NormDiff > s.tolerance)  // Multigrid solver start
	{
		applyGaussSeidel(g, 0, s.downRelaxations); // Relaxing the finest grid first
		calculateResidual(g, 0); // Calculating Initial Residual

		for (int grid = 1; grid < s.gridCount; grid++) // Going down the V-Cycle
		{
			applyRestriction(g, grid); // Restricting the residual to the coarser grid's solution vector (f)
			applyGaussSeidel(g, grid, s.downRelaxations); // Smoothing coarser level
			calculateResidual(g, grid); // Calculating Coarse Grid Residual
		}

		for (int grid = s.gridCount-1; grid > 0; grid--) // Going up the V-Cycle
		{
			applyProlongation(g, grid); // Prolonging solution for coarser level up to finer level
			applyGaussSeidel(g, grid, s.upRelaxations); // Smoothing finer level
		}

		s.L2Norm = calculateL2Norm(g, 0); // Calculating Residual L2 Norm
		s.L2NormDiff = abs(s.L2NormPrev - s.L2Norm);
	  s.L2NormPrev = s.L2Norm;
	}  // Multigrid solver end

	// Saving solution before returning
	for (int i = 0; i < g[0].N; i++) for (int j = 0; j < g[0].N; j++) s.U[i][j] = g[0].U[i][j];

  // Freeing grids
	for (int i = 0; i < s.gridCount; i++)
	{
		for (int j = 0; j < g[i].N ; j++) _mm_free(g[i].U[j]);
		for (int j = 0; j < g[i].N ; j++) _mm_free(g[i].f[j]);
		for (int j = 0; j < g[i].N ; j++) _mm_free(g[i].Res[j]);
		_mm_free(g[i].U);
		_mm_free(g[i].f);
		_mm_free(g[i].Res);
	}
	_mm_free(g);
}

void applyGaussSeidel(GridLevel* g, int l, int relaxations)
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

void calculateResidual(GridLevel* g, int l)
{
	double h2 = 1.0 / pow(g[l].h,2);

 #pragma vector aligned
 for (int i = 1; i < g[l].N-1; i++)
	#pragma ivdep
	for (int j = 1; j < g[l].N-1; j++)
	g[l].Res[i][j] = g[l].f[i][j] + (g[l].U[i-1][j] + g[l].U[i+1][j] - 4*g[l].U[i][j] + g[l].U[i][j-1] + g[l].U[i][j+1]) * h2;
}

double calculateL2Norm(GridLevel* g, int l)
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

void applyRestriction(GridLevel* g, int l)
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

void applyProlongation(GridLevel* g, int l)
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

void Heat2DSetup::generateInitialConditions(double c1, double c3, double c4)
{
	N0 = 7; // 2^N0 + 1 elements per side
	N = pow(2, N0) + 1;

	U = (double**) _mm_malloc(sizeof(double*) * N, 16); for (int i = 0; i < N ; i++)	U[i] = (double*) _mm_malloc(sizeof(double) * N, 16);
	f = (double**) _mm_malloc(sizeof(double*) * N, 16); for (int i = 0; i < N ; i++)	f[i] = (double*) _mm_malloc(sizeof(double) * N, 16);

	tolerance = 1e-8;
	double c2 = 0.05; // width

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
		f[i][j] += -(4*c1*exp( -(pow(c3 - y, 2) + pow(c4 - x, 2)) / c2 ) * (pow(c3,2) - 2*c3*y + pow(c4,2) - 2*c4*x + pow(y,2) + pow(x,2) - c2))/pow(c2,2);
	}
}
