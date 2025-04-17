/**
 * @file TriggerCandidateMakerChannelDistance.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_CHANNELDISTANCE_TRIGGERCANDIDATEMAKERCHANNELDISTANCE_HPP_
#define TRIGGERALGS_CHANNELDISTANCE_TRIGGERCANDIDATEMAKERCHANNELDISTANCE_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"
#include <algorithm>

namespace triggeralgs {

class TriggerCandidateMakerChannelDistance : public TriggerCandidateMaker {
  public:
    void operator()(const TriggerActivity& input_ta, std::vector<TriggerCandidate>& output_tcs);
    void configure(const nlohmann::json& config);
    void set_tc_attributes();

  private:
    void set_new_tc(const TriggerActivity& input_ta);
    TriggerCandidate m_current_tc;
    uint16_t m_current_tp_count;
    uint16_t m_max_tp_count = 1000; // Produce a TC when this count is exceeded. AEO: Arbitrary choice of 1000.
};

} // namespace triggeralgs

#endif // TRIGGERALGS_CHANNELDISTANCE_TRIGGERCANDIDATEMAKERCHANNELDISTANCE_HPP_
