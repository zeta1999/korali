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
#include "heat2d.hpp"

double heat2DWrapper(double* pars, int n, void* data)
{
	// User-defined Parameters
	Heat2DSetup s;
  s.pars = pars;
	heat2DSolver(s);

	pointsInfo* pd = (pointsInfo*) data;
	double h = 1.0/(s.N-1);
	for(int i = 0; i < pd->nPoints; i++)
	{
		int p = ceil(pd->xPos[i]/h);	int q = ceil(pd->yPos[i]/h);
		pd->simTemp[i] = s.U[p][q];
	}

	for (int i = 0; i < s.N ; i++) _mm_free(s.U[i]);
	_mm_free(s.U);
	return Korali::GaussianDistribution::getError(s.pars[0], pd->nPoints, pd->refTemp, pd->simTemp);
}

Heat2DSetup::Heat2DSetup()
{
  L2Norm = 0.0;
  L2NormPrev = std::numeric_limits<double>::max();
  L2NormDiff = std::numeric_limits<double>::max();
}
