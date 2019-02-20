#ifndef _HEAT2D_H_
#define _HEAT2D_H_

typedef struct pointStruct {
    double x;
    double y;
} Point;

class GridLevel;

class Heat2DSetup
{
	private:

  double* f;
  double* sol;

	public:

  double* U;
  double* pars;

  // Timekeeping variables:
  double* smoothingTime;
  double* residualTime;
  double* restrictionTime;
  double* prolongTime;
  double* L2NormTime;
  double totalTime;

  size_t N0;
  size_t N; // 2^N0 + 1 elements per side
  int iteration;

  int gridCount; // Number of multigrid levels to use
  int downRelaxations; // Number of Jacobi iterations before restriction
  int upRelaxations; // Number of Jacobi iterations after prolongation

  double L2Norm; // L2 Norm of the residual
  double L2NormPrev; // Previous L2 Norm
  double L2NormDiff; // L2Norm Difference compared to previous step
  double tolerance; // L2 Difference Tolerance before reaching convergence.

  // Setup and finish functions
  Heat2DSetup();
  void generateInitialConditions(double c1, double c2, double c3, double c4);
  void setGridCount(int count);
  double getInitial(size_t x, size_t y) { return U[x*N + y]; }
  double getRHS(size_t x, size_t y) { return f[x*N + y]; }
  void saveSolution(size_t x, size_t y, double val) { U[x*N + y] = val; }

  // Timekeeping surrogates for the solver functions
  void applyGaussSeidel_(GridLevel* g, int l, int relaxations);
  void applyProlongation_(GridLevel* g, int l);
  void applyRestriction_(GridLevel* g, int l);
  void calculateResidual_(GridLevel* g, int l);
  void calculateL2Norm_(GridLevel* g, int l);

  // Helper Functions
  void verifySolution();
  void printHelp();
  void printResults();
  void printGrid(double** g, int N);
};

void heat2DWrapper(double* pars, int n, double* results);
void heat2DSolver(Heat2DSetup& s);

// Solver functions
void applyGaussSeidel(GridLevel* g, int l, int relaxations);
void calculateResidual(GridLevel* g, int l);
void applyRestriction(GridLevel* g, int l);
void applyProlongation(GridLevel* g, int l);
double calculateL2Norm(GridLevel* g, int l);

#endif // _HEAT2D_H_
