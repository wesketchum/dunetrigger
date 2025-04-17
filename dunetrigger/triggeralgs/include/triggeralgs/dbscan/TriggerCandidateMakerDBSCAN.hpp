/**
 * @file TriggerCandidateMakerDBSCAN.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_DBSCAN_TRIGGERCANDIDATEMAKERDBSCAN_HPP_
#define TRIGGERALGS_DBSCAN_TRIGGERCANDIDATEMAKERDBSCAN_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/dbscan/dbscan.hpp"

#include <memory>
#include <vector>

namespace triggeralgs {
class TriggerCandidateMakerDBSCAN : public TriggerCandidateMaker
{

public:
  void operator()(const TriggerActivity& input_ta, std::vector<TriggerCandidate>& output_tc);
  void configure(const nlohmann::json &config);

private:
  void set_new_tc(const TriggerActivity& input_ta);
  void set_tc_attributes();
  TriggerCandidate m_current_tc;
  uint16_t m_current_tp_count;
  uint16_t m_max_tp_count = 1000; // Produce a TC when this count is exceeded. AEO: Arbitrary choice of 1000.
};
} // namespace triggeralgs

#endif // TRIGGERALGS_DBSCAN_TRIGGERCANDIDATEMAKERPRESCALE_HPP_
