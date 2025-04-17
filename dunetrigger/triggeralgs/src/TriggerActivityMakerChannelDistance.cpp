/**
 * @file TriggerActivityMakerChannelDistance.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/ChannelDistance/TriggerActivityMakerChannelDistance.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "TriggerActivityMakerChannelDistancePlugin"

namespace triggeralgs {

void
TriggerActivityMakerChannelDistance::set_new_ta(const TriggerPrimitive& input_tp)
{
  m_current_ta = TriggerActivity();
  m_current_ta.inputs.push_back(input_tp);
  m_current_lower_bound = input_tp.channel - m_max_channel_distance;
  m_current_upper_bound = input_tp.channel + m_max_channel_distance;
  return;
}

void
TriggerActivityMakerChannelDistance::operator()(const TriggerPrimitive& input_tp,
                                                std::vector<TriggerActivity>& output_tas)
{

  // Start a new TA if not already going.
  if (m_current_ta.inputs.empty()) {
    set_new_ta(input_tp);
    return;
  }

  // Check to close the TA based on time.
  if (input_tp.time_start - m_current_ta.inputs.front().time_start > m_window_length) {
    // Check to block the TA based on min TPs.
    if (m_current_ta.inputs.size() >= m_min_tps) {
      set_ta_attributes();
      output_tas.push_back(m_current_ta);
    }
    set_new_ta(input_tp);
    return;
  }

  // Check to skip the TP if it's outside the current channel bounds.
  if (input_tp.channel > m_current_upper_bound || input_tp.channel < m_current_lower_bound)
    return;

  m_current_ta.inputs.push_back(input_tp);
  m_current_lower_bound = std::min(m_current_lower_bound, input_tp.channel - m_max_channel_distance);
  m_current_upper_bound = std::max(m_current_upper_bound, input_tp.channel + m_max_channel_distance);
}

void
TriggerActivityMakerChannelDistance::configure(const nlohmann::json& config)
{
  if (config.contains("min_tps"))
    m_min_tps = config["min_tps"];
  if (config.contains("window_length"))
    m_window_length = config["window_length"];
  if (config.contains("max_channel_distance"))
    m_max_channel_distance = config["max_channel_distance"];

  return;
}

void
TriggerActivityMakerChannelDistance::set_ta_attributes()
{
  TriggerPrimitive first_tp = m_current_ta.inputs.front();
  TriggerPrimitive last_tp = m_current_ta.inputs.back();

  m_current_ta.channel_start = first_tp.channel;
  m_current_ta.channel_end = last_tp.channel;

  m_current_ta.time_start = first_tp.time_start;
  m_current_ta.time_end = last_tp.time_start;

  m_current_ta.detid = first_tp.detid;

  m_current_ta.algorithm = TriggerActivity::Algorithm::kChannelDistance;
  m_current_ta.type = TriggerActivity::Type::kTPC;

  m_current_ta.adc_peak = 0;
  for (const TriggerPrimitive& tp : m_current_ta.inputs) {
    m_current_ta.adc_integral += tp.adc_integral;
    if (tp.adc_peak <= m_current_ta.adc_peak)
      continue;
    m_current_ta.adc_peak = tp.adc_peak;
    m_current_ta.channel_peak = tp.channel;
    m_current_ta.time_peak = tp.time_peak;
  }
  m_current_ta.time_activity = m_current_ta.time_peak;
}

// Register algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerChannelDistance)

} // namespace triggeralgs
