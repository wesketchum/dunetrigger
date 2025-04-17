/**
 * @file TriggerActivityMakerHorizontalMuon.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/HorizontalMuon/TriggerActivityMakerHorizontalMuon.hpp"
#include "TRACE/trace.h"
#define TRACE_NAME "TriggerActivityMakerHorizontalMuonPlugin"
#include <math.h>
#include <vector>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_ALL;
using Logging::TLVL_DEBUG_MEDIUM;

void
TriggerActivityMakerHorizontalMuon::operator()(const TriggerPrimitive& input_tp,
                                               std::vector<TriggerActivity>& output_ta)
{

  uint16_t adjacency;

  // Add useful info about recived TPs here for FW and SW TPG guys.
  if (m_print_tp_info) {
    TLOG_DEBUG(TLVL_DEBUG_ALL) << "[TAM:HM] TP Start Time: " << input_tp.time_start
                               << ", TP ADC Sum: " << input_tp.adc_integral
                               << ", TP TOT: " << input_tp.time_over_threshold << ", TP ADC Peak: " << input_tp.adc_peak
                               << ", TP Offline Channel ID: " << input_tp.channel;
    TLOG_DEBUG(TLVL_DEBUG_ALL) << "[TAM:HM] Adjacency of current window is: " << check_adjacency();
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
  }

  // 1) ADC THRESHOLD EXCEEDED =======================================================
  // If the addition of the current TP to the window would make it longer specified
  // window length, don't add it but check whether the ADC integral if the existing
  // window is above the configured threshold. If it is, and we are triggering on ADC,
  // make a TA and start a fresh window with the current TP.
  else if (m_current_window.adc_integral > m_adc_threshold && m_trigger_on_adc) {

    ta_count++;
    if (ta_count % m_prescale == 0) {
      auto ta = construct_ta();
      TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:HM]: Emitting ADC threshold trigger with " << m_current_window.adc_integral
                                    << " window ADC integral. ta.time_start=" << ta.time_start
                                    << " ta.time_end=" << ta.time_end;
      output_ta.push_back(ta);
      m_current_window.reset(input_tp);
    }
  }

  // 2) MULTIPLICITY - N UNQIUE CHANNELS EXCEEDED =====================================
  // If the addition of the current TP to the window would make it longer than the
  // specified window length, don't add it but check whether the number of hit channels
  // in the existing window is above the specified threshold. If it is, and we are triggering
  // on channel multiplicity, make a TA and start a fresh window with the current TP.
  else if (m_current_window.n_channels_hit() > m_n_channels_threshold && m_trigger_on_n_channels) {

    ta_count++;
    if (ta_count % m_prescale == 0) {

      TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:HM] Emitting multiplicity trigger with "
                                    << m_current_window.n_channels_hit() << " unique channels hit.";

      output_ta.push_back(construct_ta());
      m_current_window.reset(input_tp);
    }
  }

  // 3) ADJACENCY THRESHOLD EXCEEDED ==================================================
  // If the addition of the current TP to the window would make it longer than the
  // specified window length, don't add it but check whether the adjacency of the
  // current window exceeds the configured threshold. If it does, and we are triggering
  // on adjacency, then create a TA and reset the window with the new/current TP.
  else if ((adjacency = check_adjacency()) > m_adjacency_threshold && m_trigger_on_adjacency) {

    ta_count++;
    if (ta_count % m_prescale == 0) {

      // for (auto tp : m_current_window.inputs){ dump_tp(tp); }

      // Check for a new maximum, display the largest seen adjacency in the log.
      // uint16_t adjacency = check_adjacency();
      if (adjacency > m_max_adjacency) {
        m_max_adjacency = adjacency;
      }
      TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:HM] Emitting track and multiplicity TA with adjacency "
                                    << check_adjacency() << " and multiplicity " << m_current_window.n_channels_hit()
                                    << ". The ADC integral of this TA is " << m_current_window.adc_integral
                                    << " and the largest longest track seen so far is " << m_max_adjacency;

      output_ta.push_back(construct_ta());
      m_current_window.reset(input_tp);
    }
  }

  // Temporary triggering logic for Adam's large TOT TPs. Trigger on very large TOT TPs.
  else if (m_trigger_on_tot && input_tp.time_over_threshold > m_tot_threshold) {

    // If the incoming TP has a large time over threshold, we might have a cluster of
    // interesting physics activity surrounding it. Trigger on that.
    TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:HM] Emitting a TA due to a TP with a very large time over threshold: "
                                  << input_tp.time_over_threshold << " ticks and offline channel: " << input_tp.channel
                                  << ", where the ADC integral of that TP is " << input_tp.adc_integral;
    output_ta.push_back(construct_ta());
    m_current_window.reset(input_tp);
  }

  // 4) Otherwise, slide the window along using the current TP.
  else {
    m_current_window.move(input_tp, m_window_length);
  }

  return;
}

void
TriggerActivityMakerHorizontalMuon::configure(const nlohmann::json& config)
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
    if (config.contains("trigger_on_adjacency"))
      m_trigger_on_adjacency = config["trigger_on_adjacency"];
    if (config.contains("adj_tolerance"))
      m_adj_tolerance = config["adj_tolerance"];
    if (config.contains("adjacency_threshold"))
      m_adjacency_threshold = config["adjacency_threshold"];
    if (config.contains("print_tp_info"))
      m_print_tp_info = config["print_tp_info"];
    if (config.contains("prescale"))
      m_prescale = config["prescale"];
    if (config.contains("trigger_on_tot"))
      m_trigger_on_tot = config["trigger_on_tot"];
    if (config.contains("tot_threshold"))
      m_tot_threshold = config["tot_threshold"];
  }
}

TriggerActivity
TriggerActivityMakerHorizontalMuon::construct_ta() const
{

  TriggerActivity ta;

  TriggerPrimitive last_tp = m_current_window.inputs.back();

  ta.time_start = last_tp.time_start;
  ta.time_end = last_tp.time_start + last_tp.time_over_threshold;
  ta.time_peak = last_tp.time_peak;
  ta.time_activity = last_tp.time_peak;
  ta.channel_start = last_tp.channel;
  ta.channel_end = last_tp.channel;
  ta.channel_peak = last_tp.channel;
  ta.adc_integral = m_current_window.adc_integral;
  ta.adc_peak = last_tp.adc_integral;
  ta.detid = last_tp.detid;
  ta.type = TriggerActivity::Type::kTPC;
  ta.algorithm = TriggerActivity::Algorithm::kHorizontalMuon;
  ta.inputs = m_current_window.inputs;

  for (const auto& tp : ta.inputs) {
    ta.time_start = std::min(ta.time_start, tp.time_start);
    ta.time_end = std::max(ta.time_end, tp.time_start + tp.time_over_threshold);
    ta.channel_start = std::min(ta.channel_start, tp.channel);
    ta.channel_end = std::max(ta.channel_end, tp.channel);
    if (tp.adc_peak > ta.adc_peak) {
      ta.time_peak = tp.time_peak;
      ta.adc_peak = tp.adc_peak;
      ta.channel_peak = tp.channel;
    }
  }

  return ta;
}

uint16_t
TriggerActivityMakerHorizontalMuon::check_adjacency() const
{
  // This function returns the adjacency value for the current window, where adjacency
  // is defined as the maximum number of consecutive wires containing hits. It accepts
  // a configurable tolerance paramter, which allows up to adj_tolerance missing hits
  // on adjacent wires before restarting the adjacency count. The maximum gap is 4 which
  // comes from tuning on December 2021 coldbox data, and June 2022 coldbox runs.

  uint16_t adj = 1;              // Initialise adjacency, 1 for the first wire.
  uint16_t max = 0;              // Maximum adjacency of window, which this function returns
  unsigned int channel = 0;      // Current channel ID
  unsigned int next_channel = 0; // Next channel ID
  unsigned int next = 0;         // The next position in the hit channels vector
  unsigned int tol_count = 0;    // Tolerance count, should not pass adj_tolerance

  // Generate a channelID ordered list of hit channels for this window
  std::vector<int> chanList;
  for (auto tp : m_current_window.inputs) {
    chanList.push_back(tp.channel);
  }
  std::sort(chanList.begin(), chanList.end());

  // ADAJACENCY LOGIC ====================================================================
  // =====================================================================================
  // Adjcancency Tolerance = Number of times prepared to skip missed hits before resetting
  // the adjacency count. This accounts for things like dead channels / missed TPs. The
  // maximum gap is 4 which comes from tuning on December 2021 coldbox data, and June 2022
  // coldbox runs.
  for (int i = 0; i < chanList.size(); ++i) {

    next = (i + 1) % chanList.size(); // Loops back when outside of channel list range
    channel = chanList.at(i);
    next_channel = chanList.at(next); // Next channel with a hit

    // End of vector condition.
    if (next_channel == 0) {
      next_channel = channel - 1;
    }

    // Skip same channel hits.
    if (next_channel == channel) {
      continue;
    }

    // If next hit is on next channel, increment the adjacency count.
    else if (next_channel == channel + 1) {
      ++adj;
    }

    // If next channel is not on the next hit, but the 'second next', increase adjacency
    // but also tally up with the tolerance counter.
    else if (((next_channel == channel + 2) || (next_channel == channel + 3) || (next_channel == channel + 4) ||
              (next_channel == channel + 5)) &&
             (tol_count < m_adj_tolerance)) {
      ++adj;
      for (int i = 0; i < next_channel - channel; ++i) {
        ++tol_count;
      }
    }

    // If next hit isn't within reach, end the adjacency count and check for a new max.
    // Reset variables for next iteration.
    else {
      if (adj > max) {
        max = adj;
      }
      adj = 1;
      tol_count = 0;
    }
  }

  return max;
}

// =====================================================================================
// Functions below this line are for debugging purposes.
// =====================================================================================
void
TriggerActivityMakerHorizontalMuon::add_window_to_record(TPWindow window)
{
  m_window_record.push_back(window);
  return;
}

// Function to dump the details of the TA window currently on record
void
TriggerActivityMakerHorizontalMuon::dump_window_record()
{
  std::ofstream outfile;
  outfile.open("window_record_tam.csv", std::ios_base::app);

  for (auto window : m_window_record) {
    outfile << window.time_start << ",";
    outfile << window.inputs.back().time_start << ",";
    outfile << window.inputs.back().time_start - window.time_start << ",";
    outfile << window.adc_integral << ",";
    outfile << window.n_channels_hit() << ",";       // Number of unique channels with hits
    outfile << window.inputs.size() << ",";          // Number of TPs in TPWindow
    outfile << window.inputs.back().channel << ",";  // Last TP Channel ID
    outfile << window.inputs.front().channel << ","; // First TP Channel ID
    outfile << check_adjacency() << ",";             // New adjacency value for the window
    outfile << check_tot() << std::endl;             // Summed window TOT
  }

  outfile.close();
  m_window_record.clear();

  return;
}

// Function to add current TP details to a text file for testing and debugging.
void
TriggerActivityMakerHorizontalMuon::dump_tp(TriggerPrimitive const& input_tp)
{
  std::ofstream outfile;
  outfile.open("coldbox_tps.txt", std::ios_base::app);

  // Output relevant TP information to file
  outfile << input_tp.time_start << " ";
  outfile << input_tp.time_over_threshold << " "; // 50MHz ticks
  outfile << input_tp.time_peak << " ";
  outfile << input_tp.channel << " "; // Offline channel ID
  outfile << input_tp.adc_integral << " ";
  outfile << input_tp.adc_peak << " ";
  outfile << input_tp.detid << " "; // Det ID - Identifies detector element
  outfile << input_tp.type << std::endl;
  outfile.close();

  return;
}

int
TriggerActivityMakerHorizontalMuon::check_tot() const
{
  // Here, we just want to sum up all the tot values for each TP within window,
  // and return this tot of the window.
  int window_tot = 0;
  for (auto tp : m_current_window.inputs) {
    window_tot += tp.time_over_threshold;
  }

  return window_tot;
}

// Register algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerHorizontalMuon)
