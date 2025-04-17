/**
 * @file TriggerCandidateMakerBundleN.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_BUNDLEN_TRIGGERCANDIDATEMAKERBUNDLEN_HPP_
#define TRIGGERALGS_BUNDLEN_TRIGGERCANDIDATEMAKERBUNDLEN_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"

#include <vector>

namespace triggeralgs {

class TriggerCandidateMakerBundleN : public TriggerCandidateMaker
{
  public:
    void operator()(const TriggerActivity& input_ta, std::vector<TriggerCandidate>& output_tcs);
    void configure(const nlohmann::json& config);
    bool bundle_condition();

  private:
      uint64_t m_bundle_size = 1;
      TriggerCandidate m_current_tc;
      void set_tc_attributes();
};

} // namespace triggeralgs

#endif // TRIGGERALGS_BUNDLEN_TRIGGERCANDIDATEMAKERBUNDLEN_HPP_
