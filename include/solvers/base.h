#ifndef _BASESOLVER_H_
#define _BASESOLVER_H_

#include <stdlib.h>
#include "problems/base.h"
#include <chrono>
#include "json.hpp"

enum verbosity { KORALI_SILENT = 0, KORALI_MINIMAL = 1, KORALI_NORMAL = 2, KORALI_DETAILED = 3 };

namespace Korali::Conduit {
    class Base;
}

namespace Korali::Solver
{

class Base {
 public:
 // Korali Runtime Variables
 size_t _currentGeneration;
 size_t _maxGens;                  // Max number of Conduit Generations
 size_t _sampleCount;
 size_t  N; // Parameter Count

 Korali::Conduit::Base* _conduit;
 Korali::Problem::Base* _problem;

 // Reporting Settings
 size_t _reportFrequency;
 int _verbosity;
 std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
 std::chrono::time_point<std::chrono::system_clock> t0, t1, t2, t3;

 Base(Korali::Problem::Base* problem);
 void setPopulationSize(int size) { _sampleCount = size; }
 void setMaxGenerations(int maxGens) { _maxGens = maxGens; }
 void setReportVerbosity(int verbosity) { _verbosity = verbosity; }

 // Running Methods
 void run();
 virtual void runSolver() = 0;
 virtual void processSample(size_t sampleId, double fitness) = 0;

 // Serialization Methods
 virtual nlohmann::json serialize();
 virtual void deserialize(nlohmann::json js);
};

} // namespace Korali

#endif // _BASESOLVER_H_
