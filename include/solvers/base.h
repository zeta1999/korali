#ifndef _BASESOLVER_H_
#define _BASESOLVER_H_

#include <stdlib.h>
#include "problems/base.h"
#include <chrono>
#include "json.hpp"

enum verbosity { KORALI_SILENT = 0, KORALI_MINIMAL = 1, KORALI_NORMAL = 2, KORALI_DETAILED = 3 };

namespace Korali::Solver
{

class Base {
 public:

  // Korali Runtime Variables
 size_t  N; // Parameter Count
 size_t _currentGeneration;

 // Reporting Settings
 std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
 std::chrono::time_point<std::chrono::system_clock> t0, t1, t2, t3;

 Base();

 // Running Methods
 virtual void run();
 virtual void processSample(size_t sampleId, double fitness) = 0;

 // Serialization Methods
 virtual nlohmann::json getConfiguration();
 virtual void setConfiguration(nlohmann::json js);
 virtual nlohmann::json getState();
 virtual void setState(nlohmann::json js);
};

} // namespace Korali

#endif // _BASESOLVER_H_
