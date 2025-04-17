/**
 * @file TriggerActivityMakerBundleN.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/BundleN/TriggerActivityMakerBundleN.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "TriggerActivityMakerBundleNPlugin"

namespace triggeralgs {

using Logging::TLVL_IMPORTANT;
using Logging::TLVL_DEBUG_HIGH;

void TriggerActivityMakerBundleN::set_ta_attributes() {
    // Using the first TA as reference.
    TriggerPrimitive first_tp = m_current_ta.inputs.front();
    TriggerPrimitive last_tp = m_current_ta.inputs.back();

    m_current_ta.channel_start = first_tp.channel;
    m_current_ta.channel_end = last_tp.channel;

    m_current_ta.time_start = first_tp.time_start;
    m_current_ta.time_end = last_tp.time_start;

    m_current_ta.detid = first_tp.detid;

    m_current_ta.algorithm = TriggerActivity::Algorithm::kBundle;
    m_current_ta.type = TriggerActivity::Type::kTPC;

    m_current_ta.adc_peak = 0;
    for (const TriggerPrimitive& tp : m_current_ta.inputs) {
      m_current_ta.adc_integral += tp.adc_integral;
      if (tp.adc_peak <= m_current_ta.adc_peak) continue;
      m_current_ta.adc_peak = tp.adc_peak;
      m_current_ta.channel_peak = tp.channel;
      m_current_ta.time_peak = tp.time_peak;
    }
    m_current_ta.time_activity = m_current_ta.time_peak;
    return;
}

bool TriggerActivityMakerBundleN::bundle_condition() {
  return m_current_ta.inputs.size() == m_bundle_size;
}

void
TriggerActivityMakerBundleN::operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_tas)
{
  // Expect that TPs are inherently time ordered.
  m_current_ta.inputs.push_back(input_tp);

  if (bundle_condition()) {
    TLOG_DEBUG(TLVL_DEBUG_HIGH) << "[TA:BN] Emitting BundleN TA with " << m_current_ta.inputs.size() << " TPs.";
    set_ta_attributes();
    output_tas.push_back(m_current_ta);

    // Reset the current.
    m_current_ta = TriggerActivity();
  }

  // Should never reach this step. In this case, send it out.
  if (m_current_ta.inputs.size() > m_bundle_size) {
    TLOG_DEBUG(TLVL_IMPORTANT) << "[TA:BN] Emitting large BundleN TriggerActivity with " << m_current_ta.inputs.size() << " TPs.";
    set_ta_attributes();
    output_tas.push_back(m_current_ta);

    // Reset the current.
    m_current_ta = TriggerActivity();
  }
}

void
TriggerActivityMakerBundleN::configure(const nlohmann::json& config)
{
  if (config.is_object() && config.contains("bundle_size")) {
    m_bundle_size = config["bundle_size"];
  }
}

REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerBundleN)
} // namespace triggeralgs

