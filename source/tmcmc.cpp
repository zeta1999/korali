#include "tmcmc.h"
#include "problem.h"

Korali::KoraliTMCMC* _kt;

Korali::KoraliTMCMC::KoraliTMCMC(Problem* problem, MPI_Comm comm) //: Korali::KoraliBase::KoraliBase(problem, comm)
{
  _problem = problem;
	_comm = comm;

	_popSize = -1;
	_rankId = 0;
	_rankCount = 1;

  _bcastFuture = upcxx::make_future();
  _continueEvaluations = true;

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

void Korali::KoraliTMCMC::run()
{
//	_kt = this;
//	upcxx::init();
//	_rankId = upcxx::rank_me();
//	_rankCount = upcxx::rank_n();

  // Verifying Parameter correctness.
	char errorString[500];
  if(Korali_VerifyParameters(errorString)) { if (_rankId == 0) fprintf(stderr, "%s", errorString); exit(-1); }

  if (_rankId == 0) Korali_SupervisorThread(); else Korali_WorkerThread();
//
//	upcxx::barrier();
//  upcxx::finalize();
}

void Korali::KoraliTMCMC::Korali_SupervisorThread()
{

}

void Korali::KoraliTMCMC::Korali_WorkerThread()
{

}

bool Korali::KoraliTMCMC::Korali_VerifyParameters(char* errorString)
{
	// Checking Problem's settings
	if (_problem->evaluateSettings(errorString)) return true;

	for (int i = 0; i < _problem->_parameterCount; i++)
	if (_problem->_parameters[i]._boundsSet == false)
	{
		sprintf(errorString, "[Korali] Error: Bounds for parameter \'%s\' have not been set.\n", _problem->_parameters[i]._name.c_str());
		return true;
	}

  for (int i = 0; i < _problem->_parameterCount; i++)
	if (_problem->_parameters[i]._priorSet == false)
	{
		sprintf(errorString, "[Korali] Error: Prior for parameter \'%s\' have not been set.\n", _problem->_parameters[i]._name.c_str());
		return true;
	}

  // Checking TMCMC Settings

  if(_popSize < 32 )
  {
  	sprintf(errorString, "[Korali] Error: Population Size (%lu) should be higher than 32.\n", _popSize);
  	return true;
  }

  return false;
}

void Korali::KoraliTMCMC::Korali_InitializeInternalVariables()
{
	int nDim = _problem->_parameterCount;


	// Initializing TMCMC Leaders
	data.nChains = _popSize;
  leaders    = (cgdbp_t*) calloc (sizeof(cgdbp_t), _popSize);
  for (int i = 0; i< _popSize; ++i) leaders[i].point = (double*) calloc (sizeof(double), nDim);

	// Initializing Data Variables
  data.Num = (int*) calloc (sizeof(int), data.MaxStages);
  for (int i = 0; i < data.MaxStages; ++i) {
  	data.Num[i] = _popSize;
  }

  double *LCmem  = (double*) calloc (sizeof(double), _popSize*nDim*nDim);
  data.local_cov = (double**) calloc (sizeof(double*), _popSize);
  for (int pos=0; pos < _popSize; ++pos)
  {
  	data.local_cov[pos] = LCmem + pos*nDim*nDim;
    for (int i=0; i<nDim; ++i) data.local_cov[pos][i*nDim+i] = 1;
  }

  // Initializing Databases
  curgen_db.entries = 0;
  curres_db.entries = 0;
  full_db.entries = 0;

  curgen_db.entry = (cgdbp_t *)calloc(1, (data.MinChainLength+1)*_popSize*sizeof(cgdbp_t));
	curgen_db.entry = (cgdbp_t *)calloc(1, (data.MinChainLength+1)*_popSize*sizeof(cgdbp_t));
  full_db.entry   =   (dbp_t *)calloc(1, data.MaxStages*_popSize*sizeof(dbp_t));

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
    for (int i = 0; i < _popSize; i++) for (int d = 0; d < nDim; d++)
    	leaders[i].point[d] = _problem->_parameters[d].getRandomNumber();
    return;
	}
}

int Korali::KoraliTMCMC::prepareNewGeneration(int nchains, cgdbp_t *leaders)
{
    /* process curgen_db -> calculate statitics */
    /* compute probs based on F values */
    /* draw new samples (nchains or user-specified) */
    /* find unique samples: fill the (new) leaders table */
    /* count how many times they appear -> nsteps */
    /* return the new sample size (number of chains) */

//    int i, p;
//
//    int n = curgen_db.entries;
//
//    unsigned int *sel = new unsigned int[n];
//
//    double **g_x = new double*[data.Nth];
//    for (i = 0; i < data.Nth; ++i) g_x[i] = new double[n];
//
//
//    /* calculate uniques & acceptance rate */
//    {
//        double * uf = new double[n];
//        double **uniques = g_x;
//        int un = 0, unflag, j;
//
//        uf[un] = curgen_db.entry[0].F;
//        for( p = 0; p < data.Nth; ++p )
//            uniques[p][un] = curgen_db.entry[0].point[p];
//
//        un++;
//        for (i = 1; i < n; ++i) {
//            double xi[data.Nth];
//            double fi = curgen_db.entry[i].F;
//            for (p = 0; p < data.Nth; ++p) xi[p] = curgen_db.entry[i].point[p];
//
//            unflag = 1;                 /* is this point unique? */
//            for (j = 0; j < un; ++j) {  /* compare with  previous uniques */
//                for (p = 0; p < data.Nth; ++p) {
//
//                    /* do they differ in position? */
//                    if (fabs(xi[p]-uniques[p][j]) > 1e-8) break; /* check next */
//
//                    /* do they differ in fun eval? */
//                    if (fabs(fi - uf[j]) > 1e-8) break; /* check next */
//
//                    unflag = 0;         /* not unique */
//                }
//
//                if (unflag == 0) break; /* not unique - stop comparison */
//            }
//
//            if (unflag) {               /* unique, put it in the table */
//                uf[un] = fi;
//                for (p = 0; p < data.Nth; ++p) uniques[p][un] = xi[p];
//                un++;
//            }
//        }
//
//        runinfo.currentuniques[runinfo.Gen] = un;
//        runinfo.acceptance[runinfo.Gen]     = (1.0*runinfo.currentuniques[runinfo.Gen])/data.Num[runinfo.Gen]; /* check this*/
//
//        if(data.options.Display) {
//
//            double meanu[data.Nth], stdu[data.Nth];
//            for (p = 0; p < data.Nth; ++p) {
//                meanu[p] = gsl_stats_mean(uniques[p], 1, n);
//                stdu[p]  = gsl_stats_sd_m(uniques[p], 1, n, meanu[p]);
//            }
//
//            printf("prepare_newgen: CURGEN DB (UNIQUES) %d\n", runinfo.Gen);
//            print_matrix("means", meanu, data.Nth);
//            print_matrix("std", stdu, data.Nth);
//        }
//    }
//    /* end block*/
//
//
//    /* calculate statistics */
//    {
//        double *fj = new double[n];
//        //double t0  = torc_gettime();
//        for (i = 0; i < n; ++i)
//            fj[i] = curgen_db.entry[i].F;    /* separate point from F ?*/
//        //double t1 = torc_gettime();
//        calculate_statistics(fj, data.Num[runinfo.Gen], runinfo.Gen, sel);
//        //double t2 = torc_gettime();
//        //printf("prepare_newgen: init + calc stats : %lf + %lf = %lf seconds\n", t2-t1, t1-t0, t2-t0);
//        delete[] fj;
//    }
//
//
//    int newchains = 0;
//    sort_t *list = new sort_t[n];
//    for (i = 0; i < n; ++i) {
//        list[i].idx  = i;
//        list[i].nsel = sel[i];
//        list[i].F    = curgen_db.entry[i].F;
//        if (sel[i] != 0) newchains++;
//    }
//
//#if VERBOSE
//    printf("Points before qsort\n");
//    for (i = 0; i < n; ++i)
//        printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);
//
//#endif
//
//    qsort(list, n, sizeof(sort_t), compar_desc);
//
//#if VERBOSE
//    printf("Points after qsort\n");
//    for (i = 0; i < n; ++i)
//        printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);
//
//#endif
//
//    /* UPPER THRESHOLD */
//    /* splitting long chains */
//    //TODO: untested feature (DW)
//    if (data.MaxChainLength > 0) {
//        int initial_newchains = newchains;
//        int h_threshold = data.MaxChainLength;
//        for (i = 0; i < initial_newchains; ++i) {
//            if (list[i].nsel > h_threshold) {
//                while (list[i].nsel > h_threshold) {
//                    list[newchains] = list[i];
//                    list[newchains].nsel = h_threshold;
//                    list[i].nsel = list[i].nsel - h_threshold;
//                    newchains++;
//                }
//            }
//        }
//
//        qsort(list, n, sizeof(sort_t), compar_desc);
//
//#if VERBOSE
//        printf("Points broken\n");
//        for (i = 0; i < n; ++i)
//            printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);
//
//#endif
//    }
//
//    /* LOWER THRESHOLD */
//    /* setting min chain length */
//    //TODO: untested feature (DW)
//    if (data.MinChainLength > 0) {
//        int l_threshold = data.MinChainLength;
//        for (i = 0; i < newchains; ++i) {
//            if ((list[i].nsel > 0)&&(list[i].nsel < l_threshold)) {
//                list[i].nsel = l_threshold;
//            }
//        }
//
//        qsort(list, n, sizeof(sort_t), compar_desc);
//
//#if VERBOSE
//        printf("Points advanced\n");
//        for (i = 0; i < n; ++i) {
//            printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);
//        }
//#endif
//    }
//
//    int ldi = 0;                    /* leader index */
//    for (i = 0; i < n; ++i) {       /* newleader */
//        if (list[i].nsel != 0) {
//            int idx = list[i].idx;
//            for (p = 0; p < data.Nth ; p++) {
//                leaders[ldi].point[p] = curgen_db.entry[idx].point[p];
//            }
//            leaders[ldi].F = curgen_db.entry[idx].F;
//            leaders[ldi].nsel = list[i].nsel;
//            ldi++;
//        }
//    }
//
//    delete[] list;
//
//    for (i = 0; i < newchains; ++i) leaders[i].queue = -1;    /* rr*/
//
//    if (1) {
//        double **x = g_x;
//        for (i = 0; i < newchains; ++i) {
//            for (p = 0; p < data.Nth; p++) {
//                x[p][i] = leaders[i].point[p];
//            }
//        }
//
//        double meanx[data.Nth], stdx[data.Nth];
//        for (p = 0; p < data.Nth; p++) {
//            meanx[p] = gsl_stats_mean(x[p], 1, newchains);
//            stdx[p]  = gsl_stats_sd_m(x[p], 1, newchains, meanx[p]);
//        }
//
//        printf("prepare_newgen: CURGEN DB (LEADER) %d: [nlead=%d]\n", runinfo.Gen, newchains);
//        if(data.options.Display) {
//            print_matrix("means", meanx, data.Nth);
//            print_matrix("std", stdx, data.Nth);
//        }
//    }
//
//    if (data.use_local_cov)
//        precompute_chain_covariances(leaders, data.init_mean, data.local_cov, newchains);
//
//    curgen_db.entries = 0;
//    printf("prepare_newgen: newchains=%d\n", newchains);
//
//    for (i = 0; i < data.Nth; ++i) delete g_x[i];
//
//    delete[] g_x;
//    delete[] sel;
//
//    return newchains;
	return 0;
}
