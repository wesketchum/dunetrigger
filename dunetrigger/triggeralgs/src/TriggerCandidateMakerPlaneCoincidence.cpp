/**
 * @file TriggerCandidateMakerPlaneCoincidence.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/PlaneCoincidence/TriggerCandidateMakerPlaneCoincidence.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "TriggerCandidateMakerPlaneCoincidencePlugin"

#include <vector>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_HIGH;
using Logging::TLVL_DEBUG_MEDIUM;
using Logging::TLVL_DEBUG_LOW;
using Logging::TLVL_DEBUG_INFO;
using Logging::TLVL_VERY_IMPORTANT;

void
TriggerCandidateMakerPlaneCoincidence::operator()(const TriggerActivity& activity,
                                                std::vector<TriggerCandidate>& output_tc)
{

  std::vector<TriggerActivity::TriggerActivityData> ta_list = { static_cast<TriggerActivity::TriggerActivityData>(
    activity) };

  // The first time operator is called, reset window object.
  if (m_current_window.is_empty()) {
    m_current_window.reset(activity);
    m_activity_count++;
    // Trivial TC Logic:
    // If the request has been made to not trigger on number of channels or
    // total adc, simply construct a trigger candidate from any single activity.
    if ((!m_trigger_on_adc) && (!m_trigger_on_n_channels)) {

      // add_window_to_record(m_current_window);
      // dump_window_record();
      TLOG_DEBUG(TLVL_DEBUG_LOW) << "[TCM:PC] Constructing TC.";
      TLOG_DEBUG(TLVL_DEBUG_HIGH) << "[TCM:PC] Activity count: " << m_activity_count;
      TriggerCandidate tc = construct_tc();
      output_tc.push_back(tc);

      // Clear the current window (only has a single TA in it)
      m_current_window.clear();
    }
    return;
  }

  // FIX ME: Only want to call this if running in debug mode.
  // add_window_to_record(m_current_window);

  // If the difference between the current TA's start time and the start of the window
  // is less than the specified window size, add the TA to the window.
  if ((activity.time_start - m_current_window.time_start) < m_window_length) {
    m_current_window.add(activity);
  }
  // If the addition of the current TA to the window would make it longer
  // than the specified window length, don't add it but check whether the sum of all adc in
  // the existing window is above the specified threshold. If it is, and we are triggering on ADC,
  // make a TA and start a fresh window with the current TP.
  else if (m_current_window.adc_integral > m_adc_threshold && m_trigger_on_adc) {
    TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TCM:PC] ADC integral in window is greater than specified threshold.";
    TriggerCandidate tc = construct_tc();

    output_tc.push_back(tc);
    TLOG_DEBUG(TLVL_DEBUG_HIGH) << "[TCM:PC] Resetting window with activity.";
    m_current_window.reset(activity);
  }
  // If the addition of the current TA to the window would make it longer
  // than the specified window length, don't add it but check whether the number of hit channels in
  // the existing window is above the specified threshold. If it is, and we are triggering on channels,
  // make a TC and start a fresh window with the current TA.
  else if (m_current_window.n_channels_hit() > m_n_channels_threshold && m_trigger_on_n_channels) {
    // TODO 04-2024: This case appears unsupported. Throwing error for now, but should this be removed?
    tc_number++;
    //   output_tc.push_back(construct_tc());
    m_current_window.reset(activity);
    TLOG_DEBUG(TLVL_DEBUG_INFO) << "[TCM:PC] Should not see this!";
  }
  // If it is not, move the window along.
  else {
    TLOG_DEBUG(TLVL_DEBUG_HIGH) << "[TCM:PC] TAWindow is at required length but specified threshold not met, shifting window along.";
    m_current_window.move(activity, m_window_length);
  }

  m_activity_count++;

  return;
}

void
TriggerCandidateMakerPlaneCoincidence::configure(const nlohmann::json& config)
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
  if (m_trigger_on_n_channels) {
    TLOG_DEBUG(TLVL_VERY_IMPORTANT) << "[TCM:PC] Using trigger_on_n_channels is not supported.";
//     throw BadConfiguration(ERS_HERE, TRACE_NAME);
  }
  if (!m_trigger_on_adc && !m_trigger_on_n_channels) {
    TLOG_DEBUG(TLVL_DEBUG_LOW) << "[TCM:PC] Both trigger flags are false. Passing TAs through 1:1.";
  }

  return;
}

TriggerCandidate
TriggerCandidateMakerPlaneCoincidence::construct_tc() const
{
  TriggerActivity latest_ta_in_window = m_current_window.inputs.back();

  TriggerCandidate tc;
  tc.time_start = m_current_window.time_start - m_readout_window_ticks_before;
  tc.time_end =
    latest_ta_in_window.inputs.back().time_start + latest_ta_in_window.inputs.back().time_over_threshold + m_readout_window_ticks_after;
  tc.time_candidate = m_current_window.time_start;
  tc.detid = latest_ta_in_window.detid;
  tc.type = TriggerCandidate::Type::kPlaneCoincidence;
  tc.algorithm = TriggerCandidate::Algorithm::kPlaneCoincidence;

  // Take the list of triggeralgs::TriggerActivity in the current
  // window and convert them (implicitly) to detdataformats'
  // TriggerActivityData, which is the base class of TriggerActivity
  for (auto& ta : m_current_window.inputs) {
    tc.inputs.push_back(ta);
  }

  return tc;
}

bool
TriggerCandidateMakerPlaneCoincidence::check_adjacency() const
{
  // FIX ME: An adjacency check on the channels which have hits.
  return true;
}

// Functions below this line are for debugging purposes.
void
TriggerCandidateMakerPlaneCoincidence::add_window_to_record(TAWindow window)
{
  m_window_record.push_back(window);
  return;
}

void
TriggerCandidateMakerPlaneCoincidence::dump_window_record()
{
  // FIX ME: Need to index this outfile in the name by detid or something similar.
  std::ofstream outfile;
  outfile.open("window_record_tcm.csv", std::ios_base::app);

  for (auto window : m_window_record) {
    outfile << window.time_start << ",";
    outfile << window.inputs.back().time_start << ",";
    outfile << window.inputs.back().time_start - window.time_start << ",";
    outfile << window.adc_integral << ",";
    outfile << window.n_channels_hit() << ",";
    outfile << window.inputs.size() << std::endl;
  }

  outfile.close();

  m_window_record.clear();

  return;
}

REGISTER_TRIGGER_CANDIDATE_MAKER(TRACE_NAME, TriggerCandidateMakerPlaneCoincidence)
