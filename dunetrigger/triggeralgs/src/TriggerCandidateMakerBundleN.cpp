/**
 * @file TriggerCandidateMakerBundleN.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/BundleN/TriggerCandidateMakerBundleN.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "TriggerCandidateMakerBundleNPlugin"

namespace triggeralgs {

using Logging::TLVL_IMPORTANT;
using Logging::TLVL_DEBUG_HIGH;

void TriggerCandidateMakerBundleN::set_tc_attributes() {
    // Using the first TA as reference.
    dunedaq::trgdataformats::TriggerActivityData front_ta = m_current_tc.inputs.front();

    m_current_tc.time_start = front_ta.time_start;
    m_current_tc.time_end = m_current_tc.inputs.back().time_end;
    m_current_tc.time_candidate = front_ta.time_start; // TODO: Conforming. Do we change this?
    m_current_tc.detid = front_ta.detid;
    m_current_tc.type = TriggerCandidate::Type::kBundle;
    m_current_tc.algorithm = TriggerCandidate::Algorithm::kBundle;
    return;
}

bool TriggerCandidateMakerBundleN::bundle_condition() {
  return m_current_tc.inputs.size() == m_bundle_size;
}

void
TriggerCandidateMakerBundleN::operator()(const TriggerActivity& input_ta, std::vector<TriggerCandidate>& output_tcs)
{
  // Expect that TAs are inherently time ordered.
  m_current_tc.inputs.push_back(input_ta);

  if (bundle_condition()) {
    TLOG_DEBUG(TLVL_DEBUG_HIGH) << "[TC:BN] Emitting BundleN TriggerCandidate with " << m_current_tc.inputs.size() << " TAs.";
    set_tc_attributes();
    output_tcs.push_back(m_current_tc);

    // Reset the current.
    m_current_tc = TriggerCandidate();
  }

  // Should never reach this step. In this case, send it out.
  if (m_current_tc.inputs.size() > m_bundle_size) {
    TLOG_DEBUG(TLVL_IMPORTANT) << "[TC:BN] Emitting large BundleN TriggerCandidate with " << m_current_tc.inputs.size() << " TAs.";
    set_tc_attributes();
    output_tcs.push_back(m_current_tc);

    // Reset the current.
    m_current_tc = TriggerCandidate();
  }
}

void
TriggerCandidateMakerBundleN::configure(const nlohmann::json& config)
{
  if (config.is_object() && config.contains("bundle_size")) {
    m_bundle_size = config["bundle_size"];
  }
}

REGISTER_TRIGGER_CANDIDATE_MAKER(TRACE_NAME, TriggerCandidateMakerBundleN)
} // namespace triggeralgs

