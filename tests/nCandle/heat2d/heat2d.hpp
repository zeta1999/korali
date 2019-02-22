#ifndef _HEAT2D_H_
#define _HEAT2D_H_

#include "korali.h"

typedef struct pointsInfoStruct {
    size_t nPoints;
    double* xPos;
    double* yPos;
    double* refTemp;
    double* simTemp;
} pointsInfo;

typedef struct gridLevelStruct {
	size_t N; // Number of points per dimension in the grid level
	double h; // DeltaX = DeltaY, the distance between points in the discretized [0,1]x[0,1] domain
	double** f; // Right hand side (external heat sources)
	double** U; // Main grid
	double** Res; // Residual Grid
} gridLevel;

void generateInitialConditions(size_t N, double c1, double c2, double c3, double** U, double** f);
double heat2DSolver(double* pars, int n, void* data);

// Solver functions
void applyGaussSeidel(gridLevel* g, int l, int relaxations);
void calculateResidual(gridLevel* g, int l);
void applyRestriction(gridLevel* g, int l);
void applyProlongation(gridLevel* g, int l);
double calculateL2Norm(gridLevel* g, int l);

#endif // _HEAT2D_H_
