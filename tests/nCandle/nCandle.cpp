#include "korali.h"
#include "heat2d/heat2d.hpp"

int main(int argc, char* argv[])
{
	int problemNumber = 1;
	for (int i = 0; i < argc; i++) if(!strcmp(argv[i], "-p")) problemNumber = atoi(argv[++i]);

  // Read solution from file
  FILE *fid;
  char pfile[50];
  sprintf(pfile, "auxiliar/problem%d.in", problemNumber);
  printf("Running problem from file %s... \n", pfile);
  fid = fopen(pfile, "r");

  pointsInfo p;
  fscanf(fid, "%lu\n", &p.nPoints);
  p.xPos = (double*) calloc (sizeof(double), p.nPoints);
  p.yPos = (double*) calloc (sizeof(double), p.nPoints);
  p.temp = (double*) calloc (sizeof(double), p.nPoints);

  for (int i = 0; i < p.nPoints; i++)
  {
  	fscanf(fid, "%le ", &p.xPos[i]);
  	fscanf(fid, "%le ", &p.yPos[i]);
  	fscanf(fid, "%le ", &p.temp[i]);
    fscanf(fid, "\n");
  }

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
