/**
 * @file TriggerDecision.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERDECISION_HPP_
#define TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERDECISION_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidate.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Types.hpp"

#include <cstdint>
#include <vector>

namespace triggeralgs {
struct TriggerDecision
{
  timestamp_t time_start = { 0 };
  timestamp_t time_end = { 0 };
  timestamp_t time_trigger = { 0 };
  trigger_number_t trigger_number = { 0 };
  uint32_t run_number = { 0 };     // NOLINT(build/unsigned)
  uint32_t subrun_number = { 0 };  // NOLINT(build/unsigned)
  uint32_t type = { 0 };           // NOLINT(build/unsigned)
  uint32_t algorithm = { 0 };      // NOLINT(build/unsigned)
  version_t version = { 0 };        // NOLINT(build/unsigned)

  std::vector<TriggerCandidate> tc_list;
};
} // namespace triggeralgs

#endif // TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERDECISION_HPP_
