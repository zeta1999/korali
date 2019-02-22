#include "heat2d/heat2d.hpp"

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

  auto problem = Korali::Problem(heat2DWrapper);
  Korali::Parameter sigma, intensity, width, xPos, yPos;

  problem.setReferenceData(&p);
	sigma.setBounds(0, 5.0);
  intensity.setBounds(0.0, 50.0);
  width.setBounds(0.0, 0.1);
  xPos.setBounds(0.0, 1.0);
  yPos.setBounds(0.0, 1.0);

  problem.addParameter(sigma);
  problem.addParameter(intensity);
  problem.addParameter(xPos);
  problem.addParameter(yPos);

  auto Solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);
	Solver.setStopMinDeltaX(1e-9);
	Solver.setLambda(64);

	Solver.run();

	return 0;
}
