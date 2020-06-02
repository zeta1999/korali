/** \file
* @brief Contains the definition of a Korali Sample
*****************************************************************************************************/

#ifndef __KORALI_SAMPLE_HPP_
#define __KORALI_SAMPLE_HPP_

#include "auxiliar/koraliJson.hpp"
#include "auxiliar/logger.hpp"
#include "libco.h"
#include <string>

#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

namespace korali
{
class Experiment;
class Engine;

/**
 * @brief Stores all functions inserted as parameters to experiment's configuration
 */
extern std::vector<std::function<void(korali::Sample &)> *> _functionVector;

/**
* @brief Execution states of a given sample.
*/
enum class SampleState
{
  uninitialized,
  initialized,
  running,
  waiting,
  finished
};

/**
* \class Sample
* @brief Contains input/output data to computational models.
*/
class Sample
{
  public:
  /**
 * @brief Pointer to the C++ object containing the sample.
 * Necessary for integration with Python, because Python only passes objects by reference, and we need to
 * access the original pointer when working on the C++ side. Therefore, we need to store the pointer as a variable.
 */
  Sample *_self;

  /**
  * @brief Current state of the sample
  */
  SampleState _state;

  /**
  * @brief User-Level thread (coroutine) containing the CPU execution state of the current Sample.
  */
  cothread_t _sampleThread;

  /**
  * @brief Determines whether the thread memory has been allocated.
  * Necessary to make sure we do not double-allocate or free unallocated space when re-executing sample.
  */
  bool _isAllocated;

  /**
  * @brief JSON object containing the sample's configuration and input/output data.
  */
  korali::KoraliJson _js;

  /**
  * @brief Constructs Sample. Stores its own pointer, sets ID to zero, state as uninitialized, and isAllocated to false.
  */
  Sample();

  /**
  * @brief Runs a computational model by reinterpreting a numerical pointer to a function(sample) object to an actual function pointer and calls it.
  * @param functionPosition Number containing a pointer to a function.
  */
  void run(size_t functionPosition);

  /**
  * @brief Handles the execution thread of individual samples on the worker's side
  */
  static void sampleLauncher();

  /**
  * @brief Rreturns results to engine without finishing the sample.
  */
  void update();

  /**
  * @brief Checks whether the sample contains the given key.
  * @param key Key (String) to look for.
  * @return True, if it is contained; false, otherwise.
  */
  bool contains(const std::string &key);

  /**
  * @brief Accesses the value of a given key in the sample.
  * @param key Key (String) to look for.
  * @return JSON object for the given key.
  */
  knlohmann::json &operator[](const std::string &key);

  /**
   * @brief Accesses the value of a given key in the sample.
   * @param key Key (number) to look for.
   * @return JSON object for the given key.
   */
  knlohmann::json &operator[](const unsigned long int &key);

  /**
  * @brief Gets the value of a given key in the sample.
  * @param key Key (pybind11 object) to look for.
  * @return Pybind11 object for the given key.
  */
  pybind11::object getItem(pybind11::object key);

  /**
  * @brief Sets the value of a given key in the sample.
  * @param val Value to assign.
  * @param key Key (pybind11 object) to look for.
  */
  void setItem(pybind11::object key, pybind11::object val);
};

} // namespace korali

#endif // __KORALI_SAMPLE_HPP_
