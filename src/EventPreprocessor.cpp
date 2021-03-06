/**
 * @file  EventPreprocessor.cpp
 * @brief EventPreprocessor
 *
 * Class implementation for EventPreprocessor
 *
 * @author     Clay Freeman
 * @date       February 27, 2015
 */

#include <string>
#include "../include/EventPreprocessor.hpp"
#include "../include/Logger.hpp"

/**
 * @brief Constructor
 *
 * Prepares the EventPreprocessor class with the provided arguments
 *
 * @param parentModule The name of the parent Module
 * @param callback     Pointer to the callback function
 */
EventPreprocessor::EventPreprocessor(const std::string& parentMod,
  bool (*call)(const std::string&)): parentModule{parentMod}, callback{call} {}

/**
 * @brief Get Parent Module
 *
 * Returns the name of the module that owns this registration
 *
 * @return std::string name of parent module
 */
const std::string& EventPreprocessor::getParentModule() const {
  return this->parentModule;
}

/**
 * @brief Call
 *
 * Calls the internal callback pointer with the provided Event name and data
 *
 * @param name The name of the Event
 * @param data A pointer to some data
 *
 * @return the bool value returned by the callee
 */
bool EventPreprocessor::call(const std::string& name) const {
  return (this->callback != nullptr ? this->callback(name) : false);
}
