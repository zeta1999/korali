#include "tmcmc.h"
#include <numeric>
#include <chrono>

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>

Korali::KoraliTMCMC* _kt;

Korali::KoraliTMCMC::KoraliTMCMC(Problem* problem, MPI_Comm comm) //: Korali::KoraliBase::KoraliBase(problem, comm)
{
  _problem = problem;
	_comm = comm;

	_popSize = 1000;
	_rankId = 0;
	_rankCount = 1;

  _bcastFuture = upcxx::make_future();
  _continueEvaluations = true;

	data.MaxStages = 20;

	data.MinChainLength = 1;
	data.MaxChainLength = 5;

	data.TolCOV  = 1;
	data.MinStep = 1e-9;
	data.bbeta   = 0.005;
	data.burn_in = 0;
	data.use_local_cov = false;

	data.options.MaxIter    = 1000;
	data.options.Tol        = 1e-12;
	data.options.Display    = true;
	data.options.Step       = 1e-8;
	data.options.LowerBound = -10.0;
	data.options.UpperBound = 10.0;

	range = gsl_rng_alloc (gsl_rng_default);
	gsl_rng_set(range, _problem->_seed+500);
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
	int nDim = _problem->_parameterCount;

	Korali_InitializeInternalVariables();

	for (int i = 0; i < _popSize; i++)
	{

		leaders[i].F = _problem->evaluateFitness(leaders[i].point);
	  double logprior = _problem->getPriorsLogProbabilityDensity(leaders[i].point);

	  //printf("[");
		//for (int d = 0; d < nDim; d++)	printf("%.3f, ", leaders[i].point[d]);
		//printf("] = [%f, %f]\n", leaders[i].F, logprior);

	  /* update current db entry */
	  update_curgen_db(leaders[i].point, leaders[i].F, logprior);
	}


  while(runinfo.p[runinfo.Gen] < 1.0 && ++runinfo.Gen < data.MaxStages) {
      data.nChains = prepareNewGeneration(data.nChains, leaders);
      evalGen();
      if (data.options.Display) print_runinfo();
  }

	dump_curgen_db();

   printf("Acceptance rate         :  %lf \n", runinfo.acceptance[runinfo.Gen]) ;
   printf("Annealing exponent      :  %lf \n", runinfo.p[runinfo.Gen]) ;
   printf("Coeficient of Variation :  %lf \n", runinfo.CoefVar[runinfo.Gen]) ;
   printf("----------------------------------------------------------------\n");
   return;
}

void Korali::KoraliTMCMC::evalGen()
{
	int nDim = _problem->_parameterCount;

    int nsteps;
   auto  gt0 = std::chrono::system_clock::now();

		int winfo[4];
		double in_tparam[nDim];
		double init_mean[nDim];
		double chain_cov[nDim*nDim];


		for (int i = 0; i < data.nChains; ++i)
		{
				winfo[0] = runinfo.Gen;
				winfo[1] = i;
				winfo[2] = -1;    /* not used */
				winfo[3] = -1;    /* not used */

				for(int d = 0; d < nDim; ++d)
						in_tparam[d] = leaders[i].point[d];

				nsteps = leaders[i].nsel;

				if (data.use_local_cov) {
						for (int d = 0; d < nDim*nDim; ++d)
								chain_cov[d] = data.local_cov[i][d];

						for (int d = 0; d < nDim; ++d) {
								if (data.use_proposal_cma)
										init_mean[d] = data.init_mean[i][d];
								else
										init_mean[d] = leaders[i].point[d];
						}
				} else {
						for (int d = 0; d < nDim; ++d)
								for (int e = 0; e < nDim; ++e)
										chain_cov[d*nDim+e]=
												data.bbeta*runinfo.SS[d][e];

						for (int d = 0; d < nDim; ++d)
								init_mean[d] = in_tparam[d];
				}

				double* out_tparam = &leaders[i].F;    /* loglik_leader...*/

				chaintask( in_tparam, &nsteps, out_tparam, winfo, init_mean, chain_cov );
		}



   auto gt1 = std::chrono::system_clock::now();
    printf("evalGen: Generation %d - ", runinfo.Gen);
    printf("generation elapsed time = %lf secs\n",
    		std::chrono::duration<double>(gt1-gt0).count());

    //if (data.icdump) dump_curgen_db();
    //if (data.ifdump) dump_full_db();

    //runinfo_t::save(runinfo, nDim, data.MaxStages);

    //if (data.restart) check_for_exit();
}

void Korali::KoraliTMCMC::dump_curgen_db()
{
	int pos;
	FILE *fp;
	int nDim = _problem->_parameterCount;

	fp = fopen("tmcmc.txt", "w");
	printf("Writing File, Entries: %d\n", curgen_db.entries);
	for (pos = 0; pos < curgen_db.entries; pos++) {
		int i;

		for (i = 0; i < nDim; i++) {
			fprintf(fp, "%3.12lf, ", curgen_db.entry[pos].point[i]);
		}
		fprintf(fp, "%3.12lf\n", curgen_db.entry[pos].F);
/*		fprintf(fp, "\n");*/
	}
	fclose(fp);
}

void Korali::KoraliTMCMC::chaintask(double in_tparam[], int *pnsteps, double *out_tparam, int winfo[4], double *init_mean, double *chain_cov)
{
	int nDim = _problem->_parameterCount;
    int nsteps   = *pnsteps;
    int gen_id   = winfo[0];
    int chain_id = winfo[1];

    double leader[nDim], loglik_leader, logprior_leader;            /* old*/
    double candidate[nDim], loglik_candidate, logprior_candidate;   /* new*/

    // get initial leader and its value
    for (int i = 0; i < nDim; ++i) leader[i] = in_tparam[i];
    loglik_leader   = *out_tparam;
    logprior_leader = _problem->getPriorsLogProbabilityDensity(leader);

    double pj = runinfo.p[runinfo.Gen];

    int burn_in = data.burn_in;

    for (int step = 0; step < nsteps + burn_in; ++step) {
        double chain_mean[nDim];
        if (step == 0)
            for (int i = 0; i < nDim; ++i) chain_mean[i] = init_mean[i];
        else
            for (int i = 0; i < nDim; ++i) chain_mean[i] = leader[i];

        if(compute_candidate(candidate, chain_mean))
        {
        //printf("Leader: [");
        //for (int i = 0; i < nDim; i++) printf("%.3f, ", leader[i]);
        //printf("]\n");

        //printf("Candidate: [");
        //for (int i = 0; i < nDim; i++) printf("%.3f, ", candidate[i]);
        //printf("]\n");

          	loglik_candidate = _problem->evaluateFitness(candidate);
            logprior_candidate = _problem->getPriorsLogProbabilityDensity(candidate);

            double L = exp((logprior_candidate-logprior_leader)+(loglik_candidate-loglik_leader)*pj);
            double P = uniformrand(0,1, range);

            //printf("P: %f - L: %f\n", P, L);

            if (P < L) {
            	//   printf("Accept\n");
                /* accept new leader */
                for (int i = 0; i < nDim; ++i) leader[i] = candidate[i];
                loglik_leader = loglik_candidate;

            }
            //else printf("Reject\n");
        }

        /* increase counter or add the leader again in curgen_db */
        if (step >= burn_in) {
            logprior_leader = logprior_candidate = _problem->getPriorsLogProbabilityDensity(leader);
            update_curgen_db(leader, loglik_leader, logprior_leader);
        }
    }

    return;
}


bool Korali::KoraliTMCMC::compute_candidate(double candidate[], double chain_mean[])
{
	int nDim = _problem->_parameterCount;
    double bSS[nDim*nDim];

    for (int i = 0; i < nDim; ++i)
        for (int j = 0; j < nDim; ++j)
            bSS[i*nDim+j]= data.bbeta*runinfo.SS[i][j];


    mvnrnd(chain_mean, (double *)bSS, candidate, nDim, range);

    int idx = 0;
    for (; idx < nDim; ++idx) {
        if (isnan(candidate[idx])) {
            printf("!!!!  compute_candidate: isnan in candidate point!\n");
            break;
        }
        if ((candidate[idx] < _problem->_parameters[idx]._lowerBound) || (candidate[idx] > _problem->_parameters[idx]._upperBound)) break;
    }

    if (idx < nDim) return false;

    return true;// all good
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

void Korali::KoraliTMCMC::update_curgen_db(double point[], double F, double prior)
{
	int nDim = _problem->_parameterCount;
  int pos = curgen_db.entries;
  curgen_db.entries++;

  if (curgen_db.entry[pos].point == NULL) curgen_db.entry[pos].point = (double *)malloc(nDim*sizeof(double));
  for (int i = 0; i < nDim; ++i) curgen_db.entry[pos].point[i] = point[i];
  curgen_db.entry[pos].F = F;
  curgen_db.entry[pos].prior = prior;
}


void Korali::KoraliTMCMC::Korali_WorkerThread()
{

}

void Korali::KoraliTMCMC::print_runinfo()
{
	int nDim = _problem->_parameterCount;
    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("Current DB Entries      :  %d  \n", curgen_db.entries);
    printf("runinfo.Gen = \n\n   %d\n\n", runinfo.Gen);
    print_matrix("runinfo.p", runinfo.p, runinfo.Gen+1);
    print_matrix_2d("runinfo.SS", runinfo.SS, nDim, nDim);
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
}


void Korali::KoraliTMCMC::Korali_InitializeInternalVariables()
{
	int nDim = _problem->_parameterCount;

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

	// Initializing TMCMC Leaders
	leaders    = (cgdbp_t*) calloc (sizeof(cgdbp_t), _popSize);
  for (int i = 0; i< _popSize; ++i) leaders[i].point = (double*) calloc (sizeof(double), nDim);

  // First definition of chains and their leaders
  data.nChains = _popSize;
  for (int i = 0; i < _popSize; i++) for (int d = 0; d < nDim; d++)	leaders[i].point[d] = _problem->_parameters[d].getRandomNumber();

	// TODO: Ensure proper memory deallocation
}

int Korali::KoraliTMCMC::prepareNewGeneration(int nchains, cgdbp_t *leaders)
{
	/* process curgen_db -> calculate statitics */
	/* compute probs based on F values */
	/* draw new samples (data.nChains or user-specified) */
	/* find unique samples: fill the (new) leaders table */
	/* count how many times they appear -> nsteps */
	/* return the new sample size (number of chains) */

	int i, p;

	 curres_db.entries = 0;
	int nDim = _problem->_parameterCount;
	int n = curgen_db.entries;

	unsigned int *sel = new unsigned int[n];

	double **g_x = new double*[nDim];
	for (i = 0; i < nDim; ++i) g_x[i] = new double[n];


	/* calculate uniques & acceptance rate */
	{
			double * uf = new double[n];
			double **uniques = g_x;
			int un = 0, unflag, j;

			uf[un] = curgen_db.entry[0].F;
			for( p = 0; p < nDim; ++p )
					uniques[p][un] = curgen_db.entry[0].point[p];

			un++;
			for (i = 1; i < n; ++i) {
					double xi[nDim];
					double fi = curgen_db.entry[i].F;
					for (p = 0; p < nDim; ++p) xi[p] = curgen_db.entry[i].point[p];

					unflag = 1;                 /* is this point unique? */
					for (j = 0; j < un; ++j) {  /* compare with  previous uniques */
							for (p = 0; p < nDim; ++p) {

									/* do they differ in position? */
									if (fabs(xi[p]-uniques[p][j]) > 1e-8) break; /* check next */

									/* do they differ in fun eval? */
									if (fabs(fi - uf[j]) > 1e-8) break; /* check next */

									unflag = 0;         /* not unique */
							}

							if (unflag == 0) break; /* not unique - stop comparison */
					}

					if (unflag) {               /* unique, put it in the table */
							uf[un] = fi;
							for (p = 0; p < nDim; ++p) uniques[p][un] = xi[p];
							un++;
					}
			}

			printf("runinfo.Gen: %d\n", runinfo.Gen);
			runinfo.currentuniques[runinfo.Gen] = un;
			runinfo.acceptance[runinfo.Gen]     = (1.0*runinfo.currentuniques[runinfo.Gen])/data.Num[runinfo.Gen]; /* check this*/

			if(data.options.Display) {

					double meanu[nDim], stdu[nDim];
					for (p = 0; p < nDim; ++p) {
							meanu[p] = gsl_stats_mean(uniques[p], 1, n);
							stdu[p]  = gsl_stats_sd_m(uniques[p], 1, n, meanu[p]);
					}

					printf("prepare_newgen: CURGEN DB (UNIQUES) %d\n", runinfo.Gen);
					print_matrix("means", meanu, nDim);
					print_matrix("std", stdu, nDim);
			}
	}
	/* end block*/


	/* calculate statistics */
	{
			double *fj = new double[n];
			//double t0  = torc_gettime();
			for (i = 0; i < n; ++i)
					fj[i] = curgen_db.entry[i].F;    /* separate point from F ?*/
			//double t1 = torc_gettime();
			calculate_statistics(fj, data.Num[runinfo.Gen], runinfo.Gen, sel);
			//double t2 = torc_gettime();
			//printf("prepare_newgen: init + calc stats : %lf + %lf = %lf seconds\n", t2-t1, t1-t0, t2-t0);
			delete[] fj;
	}


	int newchains = 0;
	sort_t *list = new sort_t[n];
	for (i = 0; i < n; ++i) {
			list[i].idx  = i;
			list[i].nsel = sel[i];
			list[i].F    = curgen_db.entry[i].F;
			if (sel[i] != 0) newchains++;
	}

#if VERBOSE
	printf("Points before qsort\n");
	for (i = 0; i < n; ++i)
			printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);

#endif

	qsort(list, n, sizeof(sort_t), compar_desc);

#if VERBOSE
	printf("Points after qsort\n");
	for (i = 0; i < n; ++i)
			printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);

#endif

	/* UPPER THRESHOLD */
	/* splitting long chains */
	//TODO: untested feature (DW)
	if (data.MaxChainLength > 0) {
			int initial_newchains = newchains;
			int h_threshold = data.MaxChainLength;
			for (i = 0; i < initial_newchains; ++i) {
					if (list[i].nsel > h_threshold) {
							while (list[i].nsel > h_threshold) {
									list[newchains] = list[i];
									list[newchains].nsel = h_threshold;
									list[i].nsel = list[i].nsel - h_threshold;
									newchains++;
							}
					}
			}

			qsort(list, n, sizeof(sort_t), compar_desc);

#if VERBOSE
			printf("Points broken\n");
			for (i = 0; i < n; ++i)
					printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);

#endif
	}

	/* LOWER THRESHOLD */
	/* setting min chain length */
	//TODO: untested feature (DW)
	if (data.MinChainLength > 0) {
			int l_threshold = data.MinChainLength;
			for (i = 0; i < newchains; ++i) {
					if ((list[i].nsel > 0)&&(list[i].nsel < l_threshold)) {
							list[i].nsel = l_threshold;
					}
			}

			qsort(list, n, sizeof(sort_t), compar_desc);

#if VERBOSE
			printf("Points advanced\n");
			for (i = 0; i < n; ++i) {
					printf("%d: %d %d %f\n", i, list[i].idx, list[i].nsel, list[i].F);
			}
#endif
	}

	int ldi = 0;                    /* leader index */
	for (i = 0; i < n; ++i) {       /* newleader */
			if (list[i].nsel != 0) {
					int idx = list[i].idx;
					for (p = 0; p < nDim ; p++) {
							leaders[ldi].point[p] = curgen_db.entry[idx].point[p];
					}
					leaders[ldi].F = curgen_db.entry[idx].F;
					leaders[ldi].nsel = list[i].nsel;
					ldi++;
			}
	}

	delete[] list;

	for (i = 0; i < newchains; ++i) leaders[i].queue = -1;    /* rr*/

	if (1) {
			double **x = g_x;
			for (i = 0; i < newchains; ++i) {
					for (p = 0; p < nDim; p++) {
							x[p][i] = leaders[i].point[p];
					}
			}

			double meanx[nDim], stdx[nDim];
			for (p = 0; p < nDim; p++) {
					meanx[p] = gsl_stats_mean(x[p], 1, newchains);
					stdx[p]  = gsl_stats_sd_m(x[p], 1, newchains, meanx[p]);
			}

			printf("prepare_newgen: CURGEN DB (LEADER) %d: [nlead=%d]\n", runinfo.Gen, newchains);
			if(data.options.Display) {
					print_matrix("means", meanx, nDim);
					print_matrix("std", stdx, nDim);
			}
	}

	if (data.use_local_cov)
			precompute_chain_covariances(leaders, data.init_mean, data.local_cov, newchains);

	curgen_db.entries = 0;
	printf("prepare_newgen: newchains=%d\n", newchains);

	for (i = 0; i < nDim; ++i) delete g_x[i];

	delete[] g_x;
	delete[] sel;

	return newchains;
}

void Korali::KoraliTMCMC::calculate_statistics(double flc[], int nselections, int gen, unsigned int sel[])
{
    int display = data.options.Display;

    double *coefVar       = runinfo.CoefVar;
    double *p             = runinfo.p;
    double *logselections = runinfo.logselections;

    double fmin = 0, xmin = 0;
    bool conv = 0;

#ifdef _USE_FMINCON_
    conv = fmincon(flc, curgen_db.entries, p[gen], data.TolCOV, &xmin, &fmin,
                   data.options);
    if (display) printf("calculate_statistics: \
                fmincon conv=%d xmin=%.16lf fmin=%.16lf\n", conv, xmin, fmin);
#endif

#ifdef _USE_FMINSEARCH_
    if (!conv) {
        conv = fminsearch(flc, curgen_db.entries, p[gen], data.TolCOV, &xmin, &fmin,
                          data.options);
        if (display) printf("calculate_statistics: \
                    fminsearch conv=%d xmin=%.16lf fmin=%.16lf\n", conv, xmin, fmin);
    }
#endif

#ifdef _USE_FZEROFIND_
    if (!conv) {
        conv = fzerofind(flc, curgen_db.entries, p[gen], data.TolCOV, &xmin, &fmin,
                         data.options);
        if (display) printf("calculate_statistics: \
                    fzerofind conv=%d xmin=%.16lf fmin=%.16lf\n", conv, xmin, fmin);
    }
#endif


    /* gen: next generation number */
    unsigned int j = gen+1;

    if ( conv && (xmin > p[gen]) ) {
        p[j]       = xmin;
        coefVar[j] = pow(fmin, 0.5) + data.TolCOV;
    } else {
        p[j]       = p[gen] + data.MinStep;
        coefVar[j] = pow(tmcmc_objlogp(p[j], flc, curgen_db.entries,
                                       p[j-1], data.TolCOV), 0.5) +
                     data.TolCOV;
    }

    if (p[j] > 1) {
        p[j]       = 1;
        coefVar[j] = pow(tmcmc_objlogp(p[j], flc, curgen_db.entries,
                                       p[j-1], data.TolCOV), 0.5) +
                     data.TolCOV;
    }

    /* Compute weights and normalize*/
    unsigned int i;

    double *flcp  = new double[curgen_db.entries];
    for (i = 0; i < curgen_db.entries; ++i)
        flcp[i] = flc[i]*(p[j]-p[j-1]);


    const double fjmax = gsl_stats_max(flcp, 1, curgen_db.entries);
    double *weight     = new double[curgen_db.entries];
    for (i = 0; i < curgen_db.entries; ++i)
        weight[i] = exp( flcp[i] - fjmax );
    if (display>2) print_matrix("weight", weight, curgen_db.entries);

    delete [] flcp;

    //TODO: logselections[gen+1] or ..[gen]? (DW)
    //logselection is log(Sj), what is logselections[0] if we switch to former?
    double sum_weight = std::accumulate(weight, weight+curgen_db.entries, 0.0);
    logselections[gen]  = log(sum_weight) + fjmax - log(curgen_db.entries);
    if (display) print_matrix("logselections", logselections, gen);

    double *q = new double[curgen_db.entries];
    for (i = 0; i < curgen_db.entries; ++i)
        q[i] = weight[i]/sum_weight;
    if (display>2) print_matrix("runinfo_q", q, curgen_db.entries);

    delete [] weight;

    if (display) print_matrix("CoefVar", coefVar, gen+1);

    unsigned int N = 1;

    unsigned int *nn = new unsigned int[curgen_db.entries];

    for (i = 0; i < curgen_db.entries; ++i) sel[i] = 0;

    if (nselections == 0) nselections = curgen_db.entries;
    N = nselections;
    multinomialrand (curgen_db.entries, N, q, nn, range);
    for (i = 0; i < curgen_db.entries; ++i) sel[i]+=nn[i];

    delete [] nn;

    if (display>2) {
        printf("\n s = [");
        for (i = 0; i < curgen_db.entries; ++i) printf("%d ", sel[i]);
        printf("]\n");
    }

    /* compute SS */
    unsigned int PROBDIM = _problem->_parameterCount;

    double mean_of_theta[PROBDIM];

    for (i = 0; i < PROBDIM; ++i) {
        mean_of_theta[i] = 0;
        for (j = 0; j < curgen_db.entries; ++j) mean_of_theta[i]+=curgen_db.entry[j].point[i]*q[j];

        runinfo.meantheta[gen][i] = mean_of_theta[i];
    }

    if (display) print_matrix("mean_of_theta", mean_of_theta, PROBDIM);

    double meanv[PROBDIM];
    for (i = 0; i < PROBDIM; ++i) {
        meanv[i] = mean_of_theta[i];
    }

    for (i = 0; i < PROBDIM; ++i) {
        for (j = i; j < PROBDIM; ++j) {
            double s = 0;
            for (unsigned int k = 0; k < curgen_db.entries; ++k) {
                s += q[k]*(curgen_db.entry[k].point[i]-meanv[i])*(curgen_db.entry[k].point[j]-meanv[j]);
            }
            runinfo.SS[i][j] = runinfo.SS[j][i] = s;
        }
    }

    delete [] q;

    if (display) print_matrix_2d("runinfo.SS", runinfo.SS, PROBDIM, PROBDIM);

}

void Korali::KoraliTMCMC::precompute_chain_covariances(const cgdbp_t* leader,double** init_mean, double** chain_cov, int newchains)
{
    bool display = data.options.Display;

    printf("Precomputing chain covariances for the current generation...\n");

    int D = _problem->_parameterCount;
    int N = curgen_db.entries;

    double my_time = clock();

    // allocate space
    int* nn_ind        = new int[newchains];
    int* nn_count      = new int[newchains];
    double* diam       = new double[D];
    double* chain_mean = new double[D];
    gsl_matrix* work   = gsl_matrix_alloc(D, D);

    // find diameters
    for (int d = 0; d < D; ++d) {
        double d_min = +1e6;
        double d_max = -1e6;
        for (int pos = 0; pos < N; ++pos) {
            double s = curgen_db.entry[pos].point[d];
            if (d_min > s) d_min = s;
            if (d_max < s) d_max = s;
        }
        diam[d] = d_max-d_min;
        if (display) printf("Diameter %d: %.6lf\n", d, diam[d]);
    }

    int ind, pos;
    int status = 0;
    double ds = 0.05;
    for (double scale = 0.1; scale <= 1.0; scale += ds) {
        // find neighbors in a rectangle - O(N^2)
        for (pos = 0; pos < newchains; ++pos) {
            nn_count[pos] = 0;
            double* curr = leader[pos].point;
            for (int i = 0; i < N; ++i) {
                double* s = curgen_db.entry[i].point;
                if (in_rect(curr, s, diam, scale, D)) {
                    nn_ind[pos*N+nn_count[pos]] = i;
                    nn_count[pos]++;
                }
            }
        }

        // compute the covariances
        for (pos = 0; pos < newchains; ++pos) {
            for (int d = 0; d < D; ++d) {
                chain_mean[d] = 0;
                for (int k = 0; k < nn_count[pos]; ++k) {
                    ind = nn_ind[pos*N+k];
                    chain_mean[d] += curgen_db.entry[ind].point[d];
                }
                chain_mean[d] /= nn_count[pos];
            }

            for (int i = 0; i < D; ++i)
                for (int j = 0; j < D; ++j) {
                    double s = 0;
                    for (int k = 0; k < nn_count[pos]; k++) {
                        ind = nn_ind[pos*N+k];
                        s  += (curgen_db.entry[ind].point[i]-chain_mean[i]) *
                              (curgen_db.entry[ind].point[j]-chain_mean[j]);
                    }
                    chain_cov[pos][i*D+j] = chain_cov[pos][j*D+i] = s/nn_count[pos];
                }

            // check if the matrix is positive definite
            for (int i = 0; i < D; ++i)
                for (int j = 0; j < D; ++j) {
                    double s = chain_cov[pos][i*D+j];
                    gsl_matrix_set(work, i, j, s);
                }
            gsl_set_error_handler_off();
            status = gsl_linalg_cholesky_decomp(work);
            if (status == GSL_SUCCESS) break;
        }
    }

    if (status != GSL_SUCCESS) {
        for (int i = 0; i < D; ++i)
            for (int j = 0; j < D; ++j)
                chain_cov[pos][i*D+j] = data.bbeta*runinfo.SS[i][j];
    }

    // deallocate space
    delete[] nn_ind;
    delete[] nn_count;
    delete[] diam;
    delete[] chain_mean;
    gsl_matrix_free(work);

    my_time = (clock() - my_time) / CLOCKS_PER_SEC;
    printf("Covariance computation time: %.2lf sec\n", my_time);
}

double Korali::KoraliTMCMC::tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero)
{

    const double fjmax = gsl_stats_max(fj, 1, fn);
#ifdef LARGE_SCALE_POPS
    double *weight = new double[fn];
#else
    double weight[fn];
#endif

    for(int i = 0; i <fn; ++i)
        weight[i] = exp((fj[i]-fjmax)*(x-pj));

    double sum_weight = std::accumulate(weight, weight+fn, 0.0);

#ifdef LARGE_SCALE_POPS
    double *q = new double[fn];
#else
    double q[fn];
#endif

    for(int i = 0; i < fn; ++i) {
        q[i] = weight[i]/sum_weight;
    }

    double mean_q = gsl_stats_mean(q, 1, fn);
    double std_q  = gsl_stats_sd_m(q, 1, fn, mean_q);
    double cov2   = pow(std_q/mean_q-zero, 2);

#ifdef LARGE_SCALE_POPS
    delete[] weight;
    delete[] q;
#endif

    return cov2;
}


double Korali::KoraliTMCMC::tmcmc_objlogp_gsl(double x, void *param)
{
    fparam_t *fp = (fparam_t *) param;
    return Korali::KoraliTMCMC::tmcmc_objlogp(x, fp->fj, fp->fn, fp->pj, fp->tol);
}


double Korali::KoraliTMCMC::tmcmc_objlogp_gsl2(const gsl_vector *v, void *param)
{
    double x = gsl_vector_get(v, 0);
    return Korali::KoraliTMCMC::tmcmc_objlogp_gsl(x, param);
}


int Korali::KoraliTMCMC::fmincon(const double *fj, int fn, double pj, double objTol,  double *xmin, double *fmin, const optim_options& opt)
{

    int status;
    size_t iter     = 0;
    size_t max_iter = opt.MaxIter;
    double tol      = opt.Tol;
    bool display    = opt.Display;
    double x_lo     = opt.LowerBound;
    double x_hi     = opt.UpperBound;

    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;
    gsl_function F;
    gsl_vector *x;

    double m  = 0.5*(x_hi-x_lo);
    double fm = 0.0;

    bool converged = false;

    x = gsl_vector_alloc (1);

    fparam_t fp;
    fp.fj = fj;
    fp.fn = fn;
    fp.pj = pj;
    fp.tol = objTol;

    F.function = Korali::KoraliTMCMC::tmcmc_objlogp_gsl;
    F.params = &fp;

    // SELECT ONE MINIMIZER STRATEGY
    T = gsl_min_fminimizer_brent;
    /*	T = gsl_min_fminimizer_goldensection;*/
    /*	T = gsl_min_fminimizer_quad_golden;; */
    s = gsl_min_fminimizer_alloc (T);

    double f_lo = tmcmc_objlogp_gsl(x_lo, &fp);
    double f_hi = tmcmc_objlogp_gsl(x_hi, &fp);
    if (f_lo < f_hi) {
        m  = x_lo;
        fm = f_lo;
    } else {
        m  = x_hi;
        fm = f_hi;
    }

    int i;
    for (i = 1; i < max_iter; ++i) {
        double x = x_lo + i*(x_hi-x_lo)/max_iter;
        double fx = tmcmc_objlogp_gsl(x, &fp);
        if (fx < fm) {
            m  = x;
            fm = fx;
        }
    }

    if (fm <= tol) {
        converged = true;
        gsl_vector_free(x);
        gsl_min_fminimizer_free (s);
        if (display)
            printf("fmincon: Early return with m = %.16f fm = %.16f\n (%dtries).", m, fm, i);
        return converged;
    }

    if ((fm < f_lo) && (fm < f_hi)) {
        if (display)
            printf("fmincon: Initialized with %d tries and m = %f (fm = %f)\n", i, m, fm);
    } else {
        if (display)
            printf("fmincon: Failed to initialize (%.16f, %.16f) (%d tries)!\n", f_lo, f_hi, i);
        return 0;
    }

    gsl_min_fminimizer_set (s, &F, m, x_lo, x_hi);

    if (display) {
        printf ("fmincon: Using %s method\n", gsl_min_fminimizer_name (s));
        printf ("%5s [%18s, %18s] %18s %18s %18s\n", "iter", "lower", "upper", "min", "fmin", "err(est)");
        printf ("%5zu [%.16f, %.16f] %.16f %.16f %.16f\n", iter, x_lo, x_hi, m, fm, x_hi - x_lo);
    }

    do {
        iter++;
        status = gsl_min_fminimizer_iterate (s);

        m    = gsl_min_fminimizer_x_minimum (s);
        x_lo = gsl_min_fminimizer_x_lower (s);
        x_hi = gsl_min_fminimizer_x_upper (s);

        status = gsl_min_test_interval (x_lo, x_hi, tol, tol);
        if (status == GSL_SUCCESS) {
            if (gsl_min_fminimizer_f_minimum(s) <= tol) {
                if (display) printf ("fmincon: Converged:\n");
                converged = true;
            } else {
                if (display) printf ("fmincon: Converged but did not find minimum:\n");
            }
        } else if (gsl_min_fminimizer_f_minimum(s) <= tol) {
            converged = true;
            status = GSL_SUCCESS;
            if (display) printf ("fmincon: Did not converge but found minimum at\n");
        }

        if (display)
            printf ("%5zu [%.16f, %.16f] %.16f f()=%.16f %.16f\n",
                    iter, x_lo, x_hi, m, gsl_min_fminimizer_f_minimum(s), x_hi - x_lo);

    } while (status == GSL_CONTINUE && iter < max_iter);

    if (converged) {
        converged = 1;
        gsl_vector_set (x, 0, m);
        *fmin = tmcmc_objlogp_gsl(m, &fp);
        *xmin = m;
    } else {
        *fmin = 0;
        *xmin = 0.0;
    }

    gsl_vector_free(x);
    gsl_min_fminimizer_free (s);

    return converged;
}


int Korali::KoraliTMCMC::fminsearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt)
{
    const gsl_multimin_fminimizer_type *T;
    gsl_multimin_fminimizer *s = NULL;
    gsl_vector *ss, *x;
    gsl_multimin_function minex_func;
    bool converged = 0;

    size_t iter     = 0;
    size_t max_iter = opt.MaxIter;
    double tol      = opt.Tol;
    bool display    = opt.Display;
    double step     = opt.Step;

    int status;
    double size;

    fparam_t fp;
    fp.fj = fj;
    fp.fn = fn;
    fp.pj = pj;
    fp.tol = objTol;

    x = gsl_vector_alloc (1);
    gsl_vector_set (x, 0, pj);

    ss = gsl_vector_alloc (1);
    gsl_vector_set_all (ss, step);

    minex_func.n      = 1;
    minex_func.f      = tmcmc_objlogp_gsl2;
    minex_func.params = &fp;

    // SELECT ONE MINIMIZER STRATEGY
    /*	T = gsl_multimin_fminimizer_nmsimplex;*/
    T = gsl_multimin_fminimizer_nmsimplex2;
    /*	T = gsl_multimin_fminimizer_nmsimplex2rand;*/
    s = gsl_multimin_fminimizer_alloc (T, 1);
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);

    if (display) printf ("fminsearch: Using %s method\n", gsl_multimin_fminimizer_name (s));

    do {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);

        if (status) break;

        size   = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, tol);

        if (status == GSL_SUCCESS) {
            if(s->fval <= tol) {
                converged = true;
                if (display) printf ("fminsearch: Converged to minimum at\n");
            } else {
                converged = false;
                if (display) printf ("fminsearch: Converged but did not find minimum.\n");
            }
        } else if (s->fval <= tol) {
            converged = true;
            status = GSL_SUCCESS;
            if (display)
                printf ("fminsearch: NOT converged but found minimum at\n");
        } else {
            converged = false;
            if (display)
                printf("fminsearch: NOT converged and did not find minimum.\n");
        }
        if (display) printf ("%3ld x =  %.16lf f() = %.16f size = %.16f\n",
                                 iter, gsl_vector_get (s->x, 0), s->fval, size);

    } while (status == GSL_CONTINUE && iter < max_iter);

    if (converged) {
        *fmin = s->fval;
        *xmin = gsl_vector_get(s->x, 0);
    } else {
        *fmin = 0;
        *xmin = 0.0;
    }

    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free (s);

    return converged;
}

/* simple min search, check stepwise for x < opt.Tol; if unsuccessfull
 * adjust range and refine steps (DW: could be optimized) */
int Korali::KoraliTMCMC::fzerofind(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt)
{

    bool display = opt.Display;
    double tol   = opt.Tol;
    double step  = opt.Step;
    double x_lo  = opt.LowerBound;
    double x_hi  = opt.UpperBound;

    int first_try = true;

    FILE *fp = NULL;
    char fname[64];

    static int counter = 0;

    size_t iter;
    size_t niters;

    double min  = 0;
    double fm = std::numeric_limits<double>::max();

    bool converged = false;

    counter++;
    while (converged == false && 1e-16 < step) {

        if(display) printf("fzerofind: x_lo %e x_hi %ei step %e\n", x_lo, x_hi, step);
        niters = (size_t) ((x_hi-x_lo)/step);

        if (first_try) {
            first_try = false;
            //sprintf(fname, "fzero_%03d.txt", counter);
            //fp = fopen(fname, "w");
            //fprintf(fp, "%-19s%-19s\n","x","fx=(cov-TolCOV)^2");
        }


        for (iter = 0; iter < niters; ++iter) {
            double x  = x_lo + iter*step;
            double fx = tmcmc_objlogp(x, fj, fn, pj, objTol);

            //if (dump) fprintf(fp, "%.16f %.16f\n", x, fx);

            if (fx < fm) {
                fm  = fx;
                min = x;
            }
            if (fx <= tol) {
                converged = true;
                break;
            }
        }

        if (converged) {
            if (display) printf("fzerofind converged: m=%.16f fm=%.16f iter=%ld\n", min, fm, niters);
        } else {
            x_lo = min - 10*step;
            if (x_lo < 0) x_lo = opt.LowerBound;
            x_hi = min + 10*step;
            if (x_hi > 4) x_hi = opt.UpperBound;
            step *= 0.1;
            if (display) printf("fzerofind (%e): m=%.16f fm=%.16f iter=%ld\n",
                                    step, min, fm, niters);
        }

    }

    *xmin = min;
    *fmin = fm;


    return converged;
}
