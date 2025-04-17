/**
 * @file TriggerDecisionMakerSupernova.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_SRC_TRIGGERALGS_SUPERNOVA_TRIGGERDECISIONMAKERSUPERNOVA_HPP_
#define TRIGGERALGS_SRC_TRIGGERALGS_SUPERNOVA_TRIGGERDECISIONMAKERSUPERNOVA_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerDecisionMaker.hpp"
#include "detdataformats/trigger/Types.hpp"

#include <algorithm>
#include <atomic>
#include <limits>
#include <vector>

namespace triggeralgs {
class TriggerDecisionMakerSupernova : public TriggerDecisionMaker
{
  /// This decision maker just spits out the trigger candidates

public:
  /// The function that returns the final trigger decision (copy of a candidate for now)
  void operator()(const TriggerCandidate&, std::vector<TriggerDecision>&);

protected:
  std::vector<TriggerCandidate> m_candidate;
  /// Sliding time window to count activities
  std::atomic<dunedaq::trgdataformats::timestamp_t> m_time_window = { 500'000'000 };
  /// Minimum number of activities in the time window to issue a trigger
  std::atomic<uint16_t> m_threshold = { 1 }; // NOLINT(build/unsigned)
  /// Minimum number of primities in an activity
  std::atomic<uint16_t> m_hit_threshold = { 1 }; // NOLINT(build/unsigned)

  /// this function gets rid of the old activities
  void FlushOldCandidate(dunedaq::trgdataformats::timestamp_t time_now)
  {
    dunedaq::trgdataformats::timestamp_diff_t how_far = time_now - m_time_window;
    auto end = std::remove_if(
                              m_candidate.begin(), m_candidate.end(), [how_far, this](auto& c) -> bool { return (static_cast<dunedaq::trgdataformats::timestamp_diff_t>(c.time_start) < how_far); });
    m_candidate.erase(end, m_candidate.end());
  }
};

} // namespace triggeralgs

#endif // TRIGGERALGS_SRC_TRIGGERALGS_SUPERNOVA_TRIGGERDECISIONMAKERSUPERNOVA_HPP_
