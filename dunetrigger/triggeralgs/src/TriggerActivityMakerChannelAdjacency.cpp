/**
 * @file TriggerActivityMakerChannelAdjacency.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/ChannelAdjacency/TriggerActivityMakerChannelAdjacency.hpp"
#include "TRACE/trace.h"
#include "dunetrigger/triggeralgs/include/triggeralgs/Logging.hpp"
#define TRACE_NAME "TriggerActivityMakerChannelAdjacencyPlugin"
#include <math.h>
#include <vector>
#include <set>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_LOW;

void
TriggerActivityMakerChannelAdjacency::operator()(const TriggerPrimitive& input_tp,
                                                 std::vector<TriggerActivity>& output_ta)
{

  // Add useful info about recived TPs here for FW and SW TPG guys.
  if (m_print_tp_info) {
    TLOG_DEBUG(TLVL_DEBUG_LOW) << " ########## m_current_window is reset ##########\n"
                               << " TP Start Time: " << input_tp.time_start << ", TP ADC Sum: " << input_tp.adc_integral
                               << ", TP TOT: " << input_tp.time_over_threshold << ", TP ADC Peak: " << input_tp.adc_peak
                               << ", TP Offline Channel ID: " << input_tp.channel << "\n";
  }

  // 0) FIRST TP =====================================================================
  // The first time operator() is called, reset the window object.
  if (m_current_window.is_empty()) {
    m_current_window.reset(input_tp);
    return;
  }

  // If the difference between the current TP's start time and the start of the window
  // is less than the specified window size, add the TP to the window.
  if ((input_tp.time_start - m_current_window.time_start) < m_window_length) {
    m_current_window.add(input_tp);
    TLOG_DEBUG(TLVL_DEBUG_LOW) << "m_current_window.time_start " << m_current_window.time_start << "\n";
  }

  else {
    TPWindow win_adj_max;

    bool ta_found = 1;
    while (ta_found) {
      // Each pass of this loop removes the TPs that formed a track from m_current_window

      // make m_current_window_tmp a copy of m_current_window and clear m_current_window
      TPWindow m_current_window_tmp = m_current_window;
      m_current_window.clear();

      // make m_current_window a new window of non-overlapping tps (of m_current_window_tmp and win_adj_max)
      for (auto tp : m_current_window_tmp.inputs) {
        bool new_tp = 1;
        for (auto tp_sel : win_adj_max.inputs) {
          if (tp.channel == tp_sel.channel && tp.time_start == tp_sel.time_start) {
            new_tp = 0;
            break;
          }
        }
        if (new_tp)
          m_current_window.add(tp);
      }

      // check adjacency -> win_adj_max now contains only those tps that make the track
      win_adj_max = check_adjacency();
      if (win_adj_max.inputs.size() > 0) {

        ta_found = 1;
        m_ta_count++;
        if (m_ta_count % m_prescale == 0) {
          output_ta.push_back(construct_ta(win_adj_max));
        }
      } else
        ta_found = 0;
    }
    // The rest of the TPs should moved along and considered with input_tp when we receive the next TP.
    m_current_window.move(input_tp, m_window_length);
  }

  return;
}

void
TriggerActivityMakerChannelAdjacency::configure(const nlohmann::json& config)
{
  if (config.is_object()) {
    if (config.contains("window_length"))
      m_window_length = config["window_length"];
    if (config.contains("adj_tolerance"))
      m_adj_tolerance = config["adj_tolerance"];
    if (config.contains("adjacency_threshold"))
      m_adjacency_threshold = config["adjacency_threshold"];
    if (config.contains("print_tp_info"))
      m_print_tp_info = config["print_tp_info"];
    if (config.contains("prescale"))
      m_prescale = config["prescale"];
  }
}

TriggerActivity
TriggerActivityMakerChannelAdjacency::construct_ta(TPWindow win_adj_max) const
{

  TriggerActivity ta;

  TriggerPrimitive last_tp = win_adj_max.inputs.back();

  ta.detid = last_tp.detid;
  ta.type = TriggerActivity::Type::kTPC;
  ta.algorithm = TriggerActivity::Algorithm::kChannelAdjacency;
  ta.inputs = win_adj_max.inputs;
  
  // Set information of first TP to prevent INVALID_CHANNEL and INVALID_TIMESTAMP messing up comparisons.
  TriggerPrimitive first_tp = ta.inputs.at(0);
  ta.time_start = first_tp.time_start;
  ta.time_end = first_tp.time_start + first_tp.time_over_threshold;
  ta.time_activity = first_tp.time_over_threshold;
  ta.channel_start = first_tp.channel;
  ta.channel_end = first_tp.channel;
  ta.adc_peak = 0;

  for (const TriggerPrimitive& tp : ta.inputs) {
    ta.time_start = std::min(ta.time_start, tp.time_start);
    ta.time_end = std::max(ta.time_end, tp.time_start + tp.time_over_threshold);
    ta.time_activity = std::max(ta.time_activity, tp.time_over_threshold);
    ta.channel_start = std::min(ta.channel_start, tp.channel);
    ta.channel_end = std::max(ta.channel_end, tp.channel);
    if (tp.adc_peak > ta.adc_peak) {
      ta.time_peak = tp.time_peak;
      ta.adc_peak = tp.adc_peak;
      ta.channel_peak = tp.channel;
    }
  }
  ta.adc_integral = win_adj_max.adc_integral;

  return ta;
}

// std::vector<TriggerPrimitive>
TPWindow
TriggerActivityMakerChannelAdjacency::check_adjacency()
{
  // This function deals with tp window (m_current_window), select adjacent tps (with a channel gap from 0 to 5; sum of
  // all gaps < m_adj_tolerance), checks if track length > m_adjacency_threshold: return the tp window (win_adj_max,
  // which is subset of the input tp window)

  channel_t channel = 0;      // Current channel ID
  channel_t next_channel = 0; // Next channel ID
  size_t next = 0;         // The next position in the hit channels vector
  unsigned int tol_count = 0;    // Tolerance count, should not pass adj_tolerance

  // Map all channel numbers to TPs.
  // Also create a list of channel numebrs to do logic on
  std::map<channel_t, std::vector<TriggerPrimitive>> chanTPMap;
  std::set<channel_t> channel_id_set;
  for (TriggerPrimitive tp: m_current_window.inputs) {
    chanTPMap[tp.channel].push_back(tp);
    channel_id_set.insert(tp.channel);
  }
  std::vector<channel_t> channel_id_list;
  // sorted since coming from a set
  channel_id_list.assign(channel_id_set.begin(), channel_id_set.end());

  // ADAJACENCY LOGIC ====================================================================
  // =====================================================================================
  // Adjcancency Tolerance = Number of times prepared to skip missed hits before resetting
  // the adjacency count (win_adj). This accounts for things like dead channels / missed TPs.

  // add first tp, and then if tps are on next channels (check code below to understand the definition)
  std::vector<channel_t> ch_adj;
  std::vector<channel_t> ch_adj_max; // if track length > m_adjacency_threshold, set win_adj_max = win_adj; return win_adj_max;

  for (size_t current = 0; current < channel_id_list.size(); ++current) {

    ch_adj_max.clear();

    next = (current + 1) % channel_id_list.size(); // Loops back when outside of channel list range
    channel = channel_id_list.at(current);
    next_channel = channel_id_list.at(next); // Next channel with a hit

    // End of vector condition.
    if (next == 0) {
      next_channel = channel - 1;
    }

    // Skip same channel hits. Should never happen. 
    if (next_channel == channel)
      continue;

    // If win_adj size == zero, add current tp
    if (ch_adj.size() == 0)
      ch_adj.push_back(channel);

    // If next hit is on next channel, increment the adjacency count
    if (next_channel - channel == 1) {
      ch_adj.push_back(next_channel);
    }

    // Allow a max gap of 5 channels (e.g., 45 and 50; 46, 47, 48, 49 are missing); increment the adjacency count
    // Sum of gaps should be < adj_tolerance (e.g., if toleance is 30, the max total gap can vary from 0 to 29+4 = 33)
    else if (next_channel - channel > 0 && next_channel - channel <= 5 && tol_count < m_adj_tolerance) {
      ch_adj.push_back(next_channel);
      tol_count += next_channel - channel - 1;
    }

    // if track length > m_adjacency_threshold, set win_adj_max = win_adj;
    else if (ch_adj.size() > m_adjacency_threshold) {
      ch_adj_max = ch_adj;
      break;
    }

    // If track length < m_adjacency_threshold, reset variables for next iteration.
    else {
      tol_count = 0;
      ch_adj.clear();
    }
  }

  TPWindow win_adj_max;
  for (channel_t chid : ch_adj_max) {
    for (const TriggerPrimitive &tp: chanTPMap.at(chid)) {
      win_adj_max.add(tp);
    }
  }
  return win_adj_max;
}

// =====================================================================================
// Functions below this line are for debugging purposes.
// =====================================================================================
void
TriggerActivityMakerChannelAdjacency::add_window_to_record(TPWindow window)
{
  m_window_record.push_back(window);
  return;
}

// Register algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerChannelAdjacency)
