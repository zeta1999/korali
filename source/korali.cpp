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

 _conduit->run();

 printf("%s\n", getConfiguration().dump(2).c_str());

 #ifdef _KORALI_USE_UPCXX
 upcxx::finalize();
 #endif

 return;
}
