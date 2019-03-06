#include <stdlib.h>

struct optim_options {
    size_t MaxIter;             /* Max number of search iterations */
    double Tol;                 /* Tolerance for root finding */
    bool    Display;             /* Print output */
    double Step;                /* Search stepsize */
    double LowerBound;          /* Lower bound for root finding (fmincon & fzerosearch)*/
    double UpperBound;          /* Upper bound for root finding (fmincon & fzerosearch)*/
};

typedef struct data_t {
    int MaxStages;                  /* Max number of tmcmc generations */
    int nChains;

    double *compositeprior_distr;   /*[PROBDIM]*/

    double *prior_mu;
    double *prior_sigma;

    int    auxil_size;
    double *auxil_data;

    int MinChainLength;         /* MinChainLength > 0: setting MinChainLength */
    int MaxChainLength;         /* MaxChainLength > 0: splitting long chains */

    double lb, ub;              /* generic lower and upper bound*/

    double TolCOV;              /* Target coefficient of variation of weights */
    double MinStep;             /* Min update of rho */
    double bbeta;               /* Covariance scaling parameter */
    int    burn_in;             /* Number of burn in iterations */

    optim_options options;      /* Optimization options (see above) */

    int prior_type;             /* 0: uniform, 1: gaussian, 3: composite */

    int *Num;               /*[MAXGENS];*/
    int    use_proposal_cma;
    double **init_mean;     /* [DATANUM][PROBDIM] */
    double **local_cov;     /* [DATANUM][PROBDIM*PROBDIM] */
    bool use_local_cov;
    double local_scale;
} data_t;


typedef struct runinfo_t {
    int    Gen;
    double *CoefVar;        /*[MAXGENS];*/
    double *p;              /*[MAXGENS];*/
    int    *currentuniques; /*[MAXGENS];*/
    double *logselections;  /*[MAXGENS];*/
    double *acceptance;     /*[MAXGENS];*/
    double **SS;            /*[PROBDIM][PROBDIM];*/
    double **meantheta;     /*[MAXGENS][PROBDIM]*/
} runinfo_t;


typedef struct cgdbp_s {
    double *point;      /*[PROBDIM];*/
    double F;
    double prior;

    int counter;        /* not used (?)*/
    int nsel;           /* for selection of leaders only*/
    int queue;          /* for submission of leaders only*/
    int surrogate;      //TODO: used? (DW)
    double error;       //TODO: used? (DW)

    int error_flg;
    int posdef;         //TODO: can we combine this with error_flg? (DW)
    double *gradient;   /*[PROBDIM]*/
    double *cov;        /*[PROBDIM]*/
    double *evec;       /*[PROBDIM][PROBDIM]*/
    double *eval;       /*[PROBDIM]*/
} cgdbp_t;


typedef struct cgdb_s {
    int     entries;
    cgdbp_t *entry;     /*[MAX_DB_ENTRIES];*/
    pthread_mutex_t m;
} cgdb_t;


typedef struct dbp_s {
    double *point; /*[PROBDIM];*/
    double F;
    int    nG;
    double G[64];    /* maxG*/
    int    surrogate;
} dbp_t;


typedef struct db_s {
    int   entries;
    dbp_t *entry; /*[MAX_DB_ENTRIES];*/        /* */
    pthread_mutex_t m;
} db_t;


typedef struct resdbp_s {
    double *point;    /*[EXPERIMENTAL_RESULTS+1]; // +1 for the result (F)*/
    double F;
    int counter;    /* not used (?)*/
    int nsel;    /* for selection of leaders only*/
} resdbp_t;


typedef struct resdb_s {
    int      entries;
    resdbp_t *entry; /*[MAX_DB_ENTRIES];*/
    pthread_mutex_t m;
} resdb_t;


typedef struct fparam_s {
    const double *fj;
    int           fn;
    double        pj;
    double        tol;
} fparam_t;


typedef struct sort_s {
    int idx;
    int nsel;
    double F;
} sort_t;

