/**
 * @file TriggerCandidateMakerChannelAdjacency.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/ChannelAdjacency/TriggerCandidateMakerChannelAdjacency.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Logging.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "TriggerCandidateMakerChannelAdjacencyPlugin"

#include <math.h>
#include <vector>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_ALL;
using Logging::TLVL_DEBUG_HIGH;
using Logging::TLVL_DEBUG_MEDIUM;
using Logging::TLVL_VERY_IMPORTANT;

void
TriggerCandidateMakerChannelAdjacency::operator()(const TriggerActivity& activity,
                                                  std::vector<TriggerCandidate>& output_tc)
{

  // The first time operator is called, reset window object.
  if (m_current_window.is_empty()) {
    m_current_window.reset(activity);
    m_activity_count++;
  }

  // If the difference between the current TA's start time and the start of the window
  // is less than the specified window size, add the TA to the window.
  else if ((activity.time_start - m_current_window.time_start) < m_window_length) {
    TLOG_DEBUG(TLVL_DEBUG_HIGH) << "[TCM:CA] Window not yet complete, adding the activity to the window.";
    m_current_window.add(activity);
  }
  // If it is not, move the window along.
  else {
    TLOG_DEBUG(TLVL_DEBUG_ALL)
      << "[TCM:CA] TAWindow is at required length but specified threshold not met, shifting window along.";
    m_current_window.move(activity, m_window_length);
  }

  // If the addition of the current TA to the window would make it longer
  // than the specified window length, don't add it but check whether the sum of all adc in
  // the existing window is above the specified threshold. If it is, and we are triggering on ADC,
  // make a TA and start a fresh window with the current TP.
  if (m_current_window.adc_integral > m_adc_threshold && m_trigger_on_adc) {
    TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TCM:CA] m_current_window.adc_integral " << m_current_window.adc_integral
                                  << " - m_adc_threshold " << m_adc_threshold;
    m_tc_number++;
    TriggerCandidate tc = construct_tc();
    TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TCM:CA] tc.time_start=" << tc.time_start << " tc.time_end=" << tc.time_end
                                  << " len(tc.inputs) " << tc.inputs.size();

    for (const auto& ta : tc.inputs) {
      TLOG_DEBUG(TLVL_DEBUG_ALL) << "[TCM:CA] [TA] ta.time_start=" << ta.time_start << " ta.time_end=" << ta.time_end
                                 << " ta.adc_integral=" << ta.adc_integral;
    }

    output_tc.push_back(tc);
    m_current_window.clear();
  }

  // If the addition of the current TA to the window would make it longer
  // than the specified window length, don't add it but check whether the number of hit channels in
  // the existing window is above the specified threshold. If it is, and we are triggering on channels,
  // make a TC and start a fresh window with the current TA.
  else if (m_current_window.n_channels_hit() > m_n_channels_threshold && m_trigger_on_n_channels) {
    m_tc_number++;

    output_tc.push_back(construct_tc());
    m_current_window.clear();
  }

  m_activity_count++;
  return;
}

void
TriggerCandidateMakerChannelAdjacency::configure(const nlohmann::json& config)
{
  if (config.is_object()) {
    if (config.contains("trigger_on_adc"))
      m_trigger_on_adc = config["trigger_on_adc"];
    if (config.contains("trigger_on_n_channels"))
      m_trigger_on_n_channels = config["trigger_on_n_channels"];
    if (config.contains("adc_threshold"))
      m_adc_threshold = config["adc_threshold"];
    if (config.contains("n_channels_threshold"))
      m_n_channels_threshold = config["n_channels_threshold"];
    if (config.contains("window_length"))
      m_window_length = config["window_length"];
    if (config.contains("readout_window_ticks_before"))
      m_readout_window_ticks_before = config["readout_window_ticks_before"];
    if (config.contains("readout_window_ticks_after"))
      m_readout_window_ticks_after = config["readout_window_ticks_after"];
  }

  // Both trigger flags were false. This will never trigger.
  if (!m_trigger_on_adc && !m_trigger_on_n_channels) {
    TLOG_DEBUG(TLVL_VERY_IMPORTANT) << "[TCM:CA] Not triggering! All trigger flags are false!";
//     throw BadConfiguration(ERS_HERE, TRACE_NAME);
  }

  return;
}

TriggerCandidate
TriggerCandidateMakerChannelAdjacency::construct_tc() const
{
  TriggerActivity latest_ta_in_window = m_current_window.inputs.back();

  TriggerCandidate tc;
  tc.time_start = m_current_window.time_start - m_readout_window_ticks_before;
  tc.time_end = m_current_window.time_start + m_readout_window_ticks_after;
  // tc.time_end = latest_ta_in_window.inputs.back().time_start + latest_ta_in_window.inputs.back().time_over_threshold;
  tc.time_candidate = m_current_window.time_start;
  tc.detid = latest_ta_in_window.detid;
  tc.type = TriggerCandidate::Type::kChannelAdjacency;
  tc.algorithm = TriggerCandidate::Algorithm::kChannelAdjacency;

  // Take the list of triggeralgs::TriggerActivity in the current
  // window and convert them (implicitly) to detdataformats'
  // TriggerActivityData, which is the base class of TriggerActivity
  for (auto& ta : m_current_window.inputs) {
    tc.inputs.push_back(ta);
  }

  return tc;
}

// Functions below this line are for debugging purposes.
void
TriggerCandidateMakerChannelAdjacency::add_window_to_record(TAWindow window)
{
  m_window_record.push_back(window);
  return;
}

REGISTER_TRIGGER_CANDIDATE_MAKER(TRACE_NAME, TriggerCandidateMakerChannelAdjacency)
