#!/bin/bash -l
#SBATCH --job-name="tmcmc_hochmuth01"
#SBATCH --output="tmcmc_hochmuth01.%j.out"
#SBATCH --time=02:00:00
#SBATCH --nodes=513
#SBATCH --ntasks-per-node=2
#SBATCH --partition=normal
#SBATCH --constraint=gpu
#SBATCH --account=s929

set -eu

date
cd ..
pushd results; ./backup.sh; popd;
nranks=`expr $SLURM_JOB_NUM_NODES \* 2 - 1`
echo "Running Korali+Mirheo on $nranks ranks..."
mir.run --runargs "-n $nranks" ./run-tmcmc.py --exp hochmuth01 --lower 8000 --upper 32000 --tend 0.4 --inimesh_fname "stretch_H1979_d01.off" --pop 512
date
