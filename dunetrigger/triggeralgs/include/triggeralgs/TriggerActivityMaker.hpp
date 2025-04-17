/**
 * @file TriggerActivityMaker.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERACTIVITYMAKER_HPP_
#define TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERACTIVITYMAKER_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/Issues.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Logging.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivity.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerPrimitive.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Types.hpp"

#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>
#include <vector>

namespace triggeralgs {

class TriggerActivityMaker
{
public:
  virtual ~TriggerActivityMaker() = default;
  virtual void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta) = 0;
  virtual void flush(timestamp_t /* until */, std::vector<TriggerActivity>&) {}
  virtual void configure(const nlohmann::json&) {}
};

} // namespace triggeralgs

#endif // TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERACTIVITYMAKER_HPP_
