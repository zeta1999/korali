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
 for(size_t i = 0; i < _parameters.size(); i++)  _parameters[i]->initialize(_seed++);
 _conduit->initialize();

 _conduit->run();

 printf("%s\n", getConfiguration().dump(2).c_str());

 #ifdef _KORALI_USE_UPCXX
 upcxx::finalize();
 #endif

 return;
}
