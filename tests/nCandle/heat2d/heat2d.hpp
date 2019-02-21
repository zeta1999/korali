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

class GridLevel;

class Heat2DSetup
{
public:

  double** f;
  double** U;
  double* pars;

  size_t N0;
  size_t N; // 2^N0 + 1 elements per side

  int gridCount; // Number of multigrid levels to use
  int downRelaxations; // Number of Jacobi iterations before restriction
  int upRelaxations; // Number of Jacobi iterations after prolongation

  double L2Norm; // L2 Norm of the residual
  double L2NormPrev; // Previous L2 Norm
  double L2NormDiff; // L2Norm Difference compared to previous step
  double tolerance; // L2 Difference Tolerance before reaching convergence.

  Heat2DSetup();
  ~Heat2DSetup();
  void generateInitialConditions(double c1, double c3, double c4);
};

double heat2DWrapper(double* pars, int n, void* data);
void heat2DSolver(Heat2DSetup& s);

// Solver functions
void applyGaussSeidel(GridLevel* g, int l, int relaxations);
void calculateResidual(GridLevel* g, int l);
void applyRestriction(GridLevel* g, int l);
void applyProlongation(GridLevel* g, int l);
double calculateL2Norm(GridLevel* g, int l);

#endif // _HEAT2D_H_
