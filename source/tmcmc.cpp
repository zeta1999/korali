#include "tmcmc.h"
#include "problem.h"

Korali::KoraliTMCMC::KoraliTMCMC(Problem* problem, MPI_Comm comm) : Korali::KoraliBase::KoraliBase(problem, comm)
{
	data.MaxStages = -1;

	data.MinChainLength = 0;
	data.MaxChainLength = 0;

	data.TolCOV  = -1;
	data.MinStep = 1e-6;
	data.bbeta   = -1;
	data.burn_in = -1;
	data.use_local_cov = false;

	data.options.MaxIter    = -1;
	data.options.Tol        = -1;
	data.options.Display    = false;
	data.options.Step       = -1;
	data.options.LowerBound = 0.0;
	data.options.UpperBound = 1.0;
}

void Korali::KoraliTMCMC::Korali_InitializeInternalVariables()
{
	int nDim = _problem->_parameterCount;

	// Initializing TMCMC Leaders
	data.nChains = _lambda;
  leaders    = (cgdbp_t*) calloc (sizeof(cgdbp_t), _lambda);
  for (int i = 0; i< _lambda; ++i) leaders[i].point = (double*) calloc (sizeof(double), nDim);

	// Initializing Data Variables
  data.Num = (int*) calloc (sizeof(int), data.MaxStages);
  for (int i = 0; i < data.MaxStages; ++i) {
  	data.Num[i] = _lambda;
  }

  double *LCmem  = (double*) calloc (sizeof(double), _lambda*nDim*nDim);
  data.local_cov = (double**) calloc (sizeof(double*), _lambda);
  for (int pos=0; pos < _lambda; ++pos)
  {
  	data.local_cov[pos] = LCmem + pos*nDim*nDim;
    for (int i=0; i<nDim; ++i) data.local_cov[pos][i*nDim+i] = 1;
  }

  // Initializing Databases
  curgen_db.entries = 0;
  curres_db.entries = 0;
  full_db.entries = 0;

  curgen_db.entry = (cgdbp_t *)calloc(1, (data.MinChainLength+1)*_lambda*sizeof(cgdbp_t));
	curgen_db.entry = (cgdbp_t *)calloc(1, (data.MinChainLength+1)*_lambda*sizeof(cgdbp_t));
  full_db.entry   =   (dbp_t *)calloc(1, data.MaxStages*_lambda*sizeof(dbp_t));

  // Initializing Run Variables
  runinfo.CoefVar        = (double*) calloc (sizeof(double), data.MaxStages+1);
	runinfo.p              = (double*) calloc (sizeof(double), data.MaxStages+1);
	runinfo.currentuniques = (int*) calloc (sizeof(int), data.MaxStages);
	runinfo.logselections  = (double*) calloc (sizeof(double), data.MaxStages);
	runinfo.acceptance     = (double*) calloc (sizeof(double), data.MaxStages);

	runinfo.Gen = 0;
	runinfo.CoefVar[0] = std::numeric_limits<double>::infinity();

	double *SSmem = new double[nDim*nDim];
	runinfo.SS = new double*[nDim];
	for(int i = 0; i< nDim; ++i) runinfo.SS[i] = SSmem + i*nDim;

	runinfo.meantheta = new double*[data.MaxStages+1];
	for(int i = 0; i < data.MaxStages+1; ++i) runinfo.meantheta[i] = new double[nDim];

	// TODO: Ensure proper memory deallocation
}

void Korali::KoraliTMCMC::Korali_GetSamplePopulation()
{
	int nDim = _problem->_parameterCount;

	// Initialize first sample distribution from prior distribution
	if (runinfo.Gen == 0)
	{
    for (int i = 0; i < _lambda; i++) for (int d = 0; d < nDim; d++)
   	_samplePopulation[i*nDim + d] = _problem->_parameters[d].getRandomNumber();


    printf("Lambda: %ld, nDim: %d\n", _lambda, nDim);

  	for (int i = 0; i < _lambda; i++)
  	{
  		printf("[");
  		for (int j = 0; j < nDim; j++)		printf("%f, ", _samplePopulation[i*nDim + j]);
  		printf("]\n");
  	}runinfo.Gen++;


    return;
	}



}
