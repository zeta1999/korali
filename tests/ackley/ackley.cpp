#include "engine_cmaes.hpp"
extern "C" {
    #include "fitfun_tests.h"
}

int main(int argc, char* argv[])
{
    fitfun_initialize_simple("Ackley");
    auto engine = CmaesEngine(&fitfun, "./");
    engine.run();
}

