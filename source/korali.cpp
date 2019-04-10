#include "korali.h"
#include <chrono>

Korali::Engine* Korali::_k;

Korali::Engine::Engine()
{

}

void Korali::Engine::run()
{
 _k = this;

 setConfiguration(_config);

 // Initializing Modules
 _problem->initialize();
 for(size_t i = 0; i < _parameters.size(); i++)  _parameters[i]->initialize(_seed++);
 _conduit->initialize();
 _solver->initialize();

 _conduit->run();

 #ifdef _KORALI_USE_UPCXX
 upcxx::finalize();
 #endif

 return;
}
