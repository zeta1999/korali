#include "engine_cmaes.hpp"
#include "korali.h"

extern "C" {
    #include "fitfun_tests.h"
}

int main(int argc, char* argv[])
{
	Korali::Engine eng;

    fitfun_initialize_simple("Ackley");
    auto engine = CmaesEngine(&fitfun, "./");
    engine.run();
}

