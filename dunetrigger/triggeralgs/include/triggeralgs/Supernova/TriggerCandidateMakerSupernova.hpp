/**
 * @file TriggerCandidateMakerSupernova.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_SRC_TRIGGERALGS_SUPERNOVA_TRIGGERCANDIDATEMAKERSUPERNOVA_HPP_
#define TRIGGERALGS_SRC_TRIGGERALGS_SUPERNOVA_TRIGGERCANDIDATEMAKERSUPERNOVA_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Types.hpp"

#include <algorithm>
#include <atomic>
#include <limits>
#include <vector>

namespace triggeralgs {
class TriggerCandidateMakerSupernova : public TriggerCandidateMaker
{
  /// This decision maker just counts the number of activities in the time_window and triggers
  /// if the number of activities exceeds that.

public:
  /// The function that gets call when there is a new activity
  void operator()(const TriggerActivity&, std::vector<TriggerCandidate>&);

protected:
  std::vector<TriggerActivity::TriggerActivityData> m_activity;
  /// Slinding time window to count activities
  std::atomic<int64_t> m_time_window = { 500'000'000 };
  /// Minimum number of activities in the time window to issue a trigger
  std::atomic<uint16_t> m_threshold = { 3 }; // NOLINT(build/unsigned)
  /// Minimum number of primities in an activity
  std::atomic<uint16_t> m_hit_threshold = { 2 }; // NOLINT(build/unsigned)

  /// this function gets rid of the old activities
  void FlushOldActivity(timestamp_t time_now)
  {
    timestamp_diff_t how_far = time_now - m_time_window;
    auto end = std::remove_if(
                              m_activity.begin(), m_activity.end(), [how_far, this](auto& c) -> bool { return (static_cast<dunedaq::trgdataformats::timestamp_diff_t>(c.time_start) < how_far); });
    m_activity.erase(end, m_activity.end());
  }
};

} // namespace triggeralgs

#endif // TRIGGERALGS_SRC_TRIGGERALGS_SUPERNOVA_TRIGGERCANDIDATEMAKERSUPERNOVA_HPP_
