#include "heat2d/heat2d.hpp"
#include "mpi.h"
#include <string.h>
#include "korali.h"

int main(int argc, char* argv[])
{
  int myRank, rankCount;

	MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Comm_size(MPI_COMM_WORLD, &rankCount);

  pointsInfo p;
  FILE *problemFile;

  if (myRank == 0)
  {
		int problemNumber = 1;
		for (int i = 0; i < argc; i++) if(!strcmp(argv[i], "-p")) problemNumber = atoi(argv[++i]);

		// Read solution from file
		char pfile[50];
		sprintf(pfile, "problem%d.in", problemNumber);
		printf("Running problem from file %s... \n", pfile);
		problemFile = fopen(pfile, "r");
		fscanf(problemFile, "%lu", &p.nPoints);
  }

  MPI_Bcast(&p.nPoints, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

  p.xPos    = (double*) calloc (sizeof(double), p.nPoints);
  p.yPos    = (double*) calloc (sizeof(double), p.nPoints);
  p.refTemp = (double*) calloc (sizeof(double), p.nPoints);
  p.simTemp = (double*) calloc (sizeof(double), p.nPoints);

  if (myRank == 0) for (int i = 0; i < p.nPoints; i++)
  {
  	fscanf(problemFile, "%le ", &p.xPos[i]);
  	fscanf(problemFile, "%le ", &p.yPos[i]);
  	fscanf(problemFile, "%le ", &p.refTemp[i]);
  }

  MPI_Bcast(p.xPos,    p.nPoints, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(p.yPos,    p.nPoints, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(p.refTemp, p.nPoints, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  auto problem = Korali::Likelihood(heat2DSolver);

  problem.setModelData(&p);
  problem.setReferenceData(p.nPoints, p.refTemp);

  Korali::Parameter par0("Intensity"); par0.setBounds(0.0, 50.0); problem.addParameter(par0);
  Korali::Parameter par1("PosX");      par1.setBounds(0.0, 1.0);  problem.addParameter(par1);
  Korali::Parameter par2("PosY");      par2.setBounds(0.0, 1.0);  problem.addParameter(par2);

  auto Solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);
	Solver.setStopMinDeltaX(1e-7);
	Solver.setLambda(32);

	Solver.run();
	MPI_Finalize();
	return 0;
}
