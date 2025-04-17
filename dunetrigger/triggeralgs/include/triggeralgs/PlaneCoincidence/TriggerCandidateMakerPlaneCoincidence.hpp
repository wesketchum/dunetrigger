/**
 * @file TriggerCandidateMakerPlaneCoincidence.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_PLANECOINCIDENCE_TRIGGERCANDIDATEMAKERPLANECOINCIDENCE_HPP_
#define TRIGGERALGS_PLANECOINCIDENCE_TRIGGERCANDIDATEMAKERPLANECOINCIDENCE_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TAWindow.hpp"
#include <fstream>
#include <vector>

namespace triggeralgs {
class TriggerCandidateMakerPlaneCoincidence : public TriggerCandidateMaker
{

public:
  // The function that gets called when there is a new activity
  void operator()(const TriggerActivity&, std::vector<TriggerCandidate>&);
  void configure(const nlohmann::json& config);

private:

  TriggerCandidate construct_tc() const;
  bool check_adjacency() const;

  TAWindow m_current_window;
  uint64_t m_activity_count = 0; // NOLINT(build/unsigned)

  // Configurable parameters.
  bool m_trigger_on_adc = false;
  bool m_trigger_on_n_channels = false;
  uint32_t m_adc_threshold = 1200000;
  uint16_t m_n_channels_threshold = 600; // 80ish for frames, O(200 - 600) for tpslink
  timestamp_t m_window_length = 80000;
  timestamp_t m_readout_window_ticks_before = 30000;
  timestamp_t m_readout_window_ticks_after = 30000;
  int tc_number = 0;

  // For debugging and performance study purposes.
  void add_window_to_record(TAWindow window);
  void dump_window_record();
  std::vector<TAWindow> m_window_record;
};
} // namespace triggeralgs
#endif // TRIGGERALGS_PLANECOINCIDENCE_TRIGGERCANDIDATEMAKERPLANECOINCIDENCE_HPP_
