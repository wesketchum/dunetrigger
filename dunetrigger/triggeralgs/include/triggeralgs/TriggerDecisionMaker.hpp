/**
 * @file TriggerDecisionMaker.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERDECISIONMAKER_HPP_
#define TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERDECISIONMAKER_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/Issues.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Logging.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidate.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerDecision.hpp"

#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>
#include <vector>

namespace triggeralgs {

class TriggerDecisionMaker
{
public:
  virtual ~TriggerDecisionMaker() = default;
  virtual void operator()(const TriggerCandidate& input_tc, std::vector<TriggerDecision>& output_tds) = 0;
  virtual void flush(std::vector<TriggerDecision>&) {}
  virtual void configure(const nlohmann::json&) {}
};

} // namespace triggeralgs

#endif // TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERDECISIONMAKER_HPP_
