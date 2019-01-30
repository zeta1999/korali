#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmaes_utils.h"


#include <sys/time.h>
double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec*1.0E-6;
}


#include <sys/stat.h>

int cmaes_utils_file_exists(const char *fname) {
	struct stat buffer;
	return (stat (fname, &buffer) == 0);
}


double cmaes_utils_load_pop_from_file(int verbose, int step, double * const* pop, double *arFunvals, int dim, int lambda, int * checkp) {	
    char filename[256];
    sprintf(filename, "curgen_db_%03d.txt", step);
    FILE *fp = fopen(filename, "r");
    double tt0, tt1 ;
     
    tt0 = get_time();

    if( fp != NULL ) {
   
        for( int i = 0; i < lambda; i++ ){
            for( int j = 0; j < dim; j++ ){
                int r = fscanf(fp, "%le", &pop[i][j]);
   
                if (verbose) printf("[%d] pop[%d][%d]=%f\n", r, i, j, pop[i][j]);
                 
                if (r < 0){
                    printf("Error occured while reading the (%d,%d) element from %s. Exit...\n",i,j,filename);
                    exit(1);
                }
            }
         	
            int r = fscanf(fp, "%le", &arFunvals[i]);
         	
            if (verbose) printf("[%d] arFunvals[%d] = %f\n", r, i, arFunvals[i]);
			
            if (r < 0){
                printf("Error occured while reading the (%d) function value from %s. Exit...\n",i, filename);
                exit(1);
            }
        }
    	fclose(fp);
    }
    else
        *checkp = 0;

    tt1 = get_time();

    return tt1-tt0;
}


void cmaes_utils_print_the_best( cmaes_t evo, int step ) {
    int dim    = cmaes_Get( &evo, "dim");
    	
    const double *xbever = cmaes_GetPtr(&evo, "xbestever");
    double        fbever = cmaes_Get(   &evo, "fbestever");

    printf("BEST @ %5d: ", step);
    for( int i = 0; i < dim; i++ )
        printf("%25.16lf ", xbever[i]);
    printf("%25.16lf\n", fbever);
}

void cmaes_utils_write_pop_to_file( cmaes_t evo, double *arFunvals, double * const* pop, int step ){
    int dim    = cmaes_Get( &evo, "dim");
    int lambda = cmaes_Get( &evo, "lambda");
    	
    char filename[256];
    sprintf(filename, "curgen_db_%03d.txt", step);
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < lambda; i++) {
        for ( int j = 0; j < dim; j++) 
            fprintf(fp, "%.6le ", pop[i][j]);
        fprintf(fp, "%.6le\n", arFunvals[i]);
    }
    fclose(fp);
}

int cmaes_utils_is_there_enough_time(long job_max_time, double gt0, double dt ) {
	
    if (job_max_time <= 0) return 1;
	
    double lastgen_time = dt;
    long runt, remt;
    	
    long maxt = job_max_time;    //get_job_maxTime(); // from lsf or provided by the user
    printf("job maxtime = %ld\n", maxt);

    runt = get_time()-gt0;    //runt = get_job_runTime(); // from lsf or provided by the application: runt = omp_get_wtime()-gt0;
    remt = maxt - runt;
    printf("job runtime = %ld remtime = %ld\n", runt, remt);

    if ((lastgen_time*1.1) > remt){
        printf("No more available time, exiting...\n");
        return 0;
    }

    return 1;
}
