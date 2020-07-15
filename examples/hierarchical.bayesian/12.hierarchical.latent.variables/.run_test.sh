#!/bin/bash

###### Auxiliar Functions and Variables #########

source ../../../tests/functions.sh

##### Deleting Previous Results

echo "  + Deleting previous results..."
rm -rf _korali_result*; check_result

##### Running Tests

python3 ./run-saem-population-simple.py; check_result
python3 ./run-saem-hierarchical.py; check_result
python3 ./run-saem-hierarchical-n-d.py; check_result
python3 ./run-saem-logistic.py; check_result
python3 ./run-saem-normal.py; check_result

  
