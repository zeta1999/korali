#include <stdexcept>
#include <chrono>
#include "engine_cmaes.hpp"
#include "engine_cmaes_utils.hpp"



CmaesEngine::CmaesEngine(int dim, double (*fun) (double*, int), int restart) : dim_(dim)
{

}



