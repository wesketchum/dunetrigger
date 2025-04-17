/**
 * @file TriggerActivityMakerPlaneCoincidence.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/PlaneCoincidence/TriggerActivityMakerPlaneCoincidence.hpp"
#include "TRACE/trace.h"
#define TRACE_NAME "TriggerActivityMakerPlaneCoincidencePlugin"
#include <vector>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_MEDIUM;

void
TriggerActivityMakerPlaneCoincidence::operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta)
{

  // Get the plane from which this hit arrived: 
  // U (induction) = 0, Y (induction) = 1, Z (collection) = 2, unconnected channel = 9999  
  uint plane = channelMap->get_plane_from_offline_channel(input_tp.channel);
  bool isU = plane == 0;  // Induction1 = U
  bool isY = plane == 1;  // Induction2 = Y
  bool isZ = plane == 2;  // Collection = Z
 
  // The first time operator() is called, reset the window object(s).
  if (m_induction1_window.is_empty() && isU) { m_induction1_window.reset(input_tp); m_primitive_count++; return; }
  if (m_induction2_window.is_empty() && isY) { m_induction2_window.reset(input_tp); m_primitive_count++; return; }
  if (m_collection_window.is_empty() && isZ) { m_collection_window.reset(input_tp); m_primitive_count++; return; }
  
  // If the difference between the current TP's start time and the start of the window
  // is less than the specified window size, add the TP to the corresponding window.
  if (isU && (input_tp.time_start - m_induction1_window.time_start) < m_window_length) m_induction1_window.add(input_tp);
  if (isY && (input_tp.time_start - m_induction2_window.time_start) < m_window_length) m_induction2_window.add(input_tp);
  if (isZ && (input_tp.time_start - m_collection_window.time_start) < m_window_length) m_collection_window.add(input_tp); 

  // ISSUE - We are checking the collection plane window too early: Every time we are NOT receiving a collection plane
  // TP, we're checking the trigger conditions. Fix this immediately and rerun trigger runs to test.

  // ===================================================================================
  // Below this line, we begin our hierarchy of checks for a low energy event,
  // taking advantage of the newly gained induction hits window.
  // ===================================================================================

  // Our windows have ADC Sum, Time Over Threshold, Multiplicity & Adjacency properties.
  // Take advantage of these to screen the activities passed to more involved checks.

  // 1) REQUIRE ADC SPIKE FROM INDUCTION AND CHECK ADJACENCY ===========================
  // We're looking for a localised spike of ADC (short time window) and then a short
  // adjacency corresponding to an electron track/shower.
  
  // ISSUE - We are checking the collection plane window too early and too frequently probably:
  // Every time we are NOT receiving a collection plane TP, we're checking the trigger conditions. 
  // Fix this immediately and rerun trigger runs to test.
  // Introduce bool to check for collection window completeness:
  bool collectionComplete = (input_tp.time_start - m_collection_window.time_start) > m_window_length;
  // Then require that the collection window be complete in the adjacency checks
  if (!collectionComplete) { } // Do nothing
  else if (collectionComplete && (m_induction1_window.adc_integral + m_induction2_window.adc_integral + m_collection_window.adc_integral)
            > m_adc_threshold && check_adjacency(m_collection_window) >= m_adjacency_threshold){

          TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:PC] Emitting low energy trigger with " << m_induction1_window.adc_integral << " U "
                  << m_induction2_window.adc_integral << " Y induction ADC sums and "
                  << check_adjacency(m_collection_window) << " adjacent collection hits.";
   
          // Initial studies - output the TPs of the collection plane window that caused this trigger
          add_window_to_record(m_collection_window);
          dump_window_record();
          m_window_record.clear();     

          // Initial studies - Also dump the TPs that have contributed to this TA decision
          for(auto tp : m_collection_window.inputs) dump_tp(tp);
 
          // We have fulfilled our trigger condition, construct a TA and reset/flush the windows
          // to ensure they're all in the same "time zone"!
          output_ta.push_back(construct_ta(m_collection_window));
          if (isZ) m_collection_window.reset(input_tp);
          else m_collection_window.clear();
          if (isU) m_induction1_window.reset(input_tp); 
          else m_induction1_window.clear();
          if (isY) m_induction2_window.reset(input_tp);   
          else m_induction2_window.clear();
  }

  // Otherwise, slide the relevant window along using the current TP.
  else {
    if (isU) m_induction1_window.move(input_tp, m_window_length);
    else if (isY) m_induction2_window.move(input_tp, m_window_length);
    else if (isZ) m_collection_window.move(input_tp, m_window_length);
  }
  m_primitive_count++;

  return;
}

void
TriggerActivityMakerPlaneCoincidence::configure(const nlohmann::json& config)
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
  }

}

TriggerActivity
TriggerActivityMakerPlaneCoincidence::construct_ta(TPWindow m_current_window) const
{

  TriggerPrimitive latest_tp_in_window = m_current_window.inputs.back();

  TriggerActivity ta;
  ta.time_start = m_current_window.time_start;
  ta.time_end = latest_tp_in_window.time_start + latest_tp_in_window.time_over_threshold;
  ta.time_peak = latest_tp_in_window.time_peak;
  ta.time_activity = latest_tp_in_window.time_peak;
  ta.channel_start = latest_tp_in_window.channel;
  ta.channel_end = latest_tp_in_window.channel;
  ta.channel_peak = latest_tp_in_window.channel;
  ta.adc_integral = m_current_window.adc_integral;
  ta.adc_peak = latest_tp_in_window.adc_peak;
  ta.detid = latest_tp_in_window.detid;
  ta.type = TriggerActivity::Type::kTPC;
  ta.algorithm = TriggerActivity::Algorithm::kPlaneCoincidence;
  ta.inputs = m_current_window.inputs;

  return ta;
}

uint16_t
TriggerActivityMakerPlaneCoincidence::check_adjacency(TPWindow window) const
{
  /* This function returns the adjacency value for the current window, where adjacency
  *  is defined as the maximum number of consecutive wires containing hits. It accepts
  *  a configurable tolerance paramter, which allows up to adj_tolerance missing hits
  *  on adjacent wires before restarting the adjacency count. */

  uint16_t adj = 1;              // Initialise adjacency, 1 for the first wire.
  uint16_t max = 0;              // Maximum adjacency of window, which this function returns
  unsigned int channel = 0;      // Current channel ID
  unsigned int next_channel = 0; // Next channel ID
  unsigned int next = 0;         // The next position in the hit channels vector
  unsigned int tol_count = 0;    // Tolerance count, should not pass adj_tolerance

  /* Generate a channelID ordered list of hit channels for this window */
  std::vector<int> chanList;
  for (auto tp : window.inputs) {
    chanList.push_back(tp.channel);
  }
  std::sort(chanList.begin(), chanList.end());

  /* ADAJACENCY LOGIC ====================================================================
  *  Adjcancency Tolerance = Number of times prepared to skip missed hits before resetting 
  *  the adjacency count. This accounts for things like dead channels / missed TPs. */
  for (size_t i = 0; i < chanList.size(); ++i) {

    next = (i + 1) % chanList.size(); // Loops back when outside of channel list range
    channel = chanList.at(i);
    next_channel = chanList.at(next); // Next channel with a hit

    // End of vector condition.
    if (next_channel == 0) { next_channel = channel - 1; }

    // Skip same channel hits.
    if (next_channel == channel) { continue; }

    // If next hit is on next channel, increment the adjacency count.
    else if (next_channel == channel + 1){ ++adj; }

    // If next channel is not on the next hit, but the 'second next', increase adjacency 
    // but also tally up with the tolerance counter.
    else if ((next_channel == channel + 2 || next_channel == channel + 3) && (tol_count < m_adj_tolerance)) { 
	++adj;
        for (size_t i = 0 ; i < next_channel-channel ; ++i) ++tol_count;
    }

    // If next hit isn't within reach, end the adjacency count and check for a new max.
    // Reset variables for next iteration.
    else {
      if (adj > max) { max = adj; } 
      adj = 1;
      tol_count = 0;
    }
  }

  return max;
}

// =====================================================================================
// Functions below this line are for debugging and performance study purposes.
// =====================================================================================
void
TriggerActivityMakerPlaneCoincidence::add_window_to_record(TPWindow window)
{
  m_window_record.push_back(window);
  return;
}

// Function to dump the details of the TA window currently on record
void
TriggerActivityMakerPlaneCoincidence::dump_window_record()
{
  std::ofstream outfile;
  outfile.open("window_record_tam.csv", std::ios_base::app);

  for (auto window : m_window_record) {
    outfile << window.time_start << ",";
    outfile << window.inputs.back().time_start << ",";
    outfile << window.inputs.back().time_start - window.time_start << ",";
    outfile << window.adc_integral << ",";
    outfile << window.n_channels_hit() << ",";             // Number of unique channels with hits
    outfile << window.inputs.size() << ",";                // Number of TPs in TPWindow
    outfile << window.inputs.back().channel << ",";        // Last TP Channel ID
    outfile << window.inputs.back().time_start << ",";     // Last TP start time
    outfile << window.inputs.front().channel << ",";       // First TP Channel ID
    outfile << window.inputs.front().time_start << ",";    // First TP start time 
    outfile << check_adjacency(window) << ",";             // New adjacency value for the window
    outfile << check_tot(window) << std::endl;             // Summed window TOT
  }

  outfile.close();
  m_window_record.clear();

  return;
}

// Function to add current TP details to a text file for testing and debugging.
void
TriggerActivityMakerPlaneCoincidence::dump_tp(TriggerPrimitive const& input_tp)
{
  std::ofstream outfile;
  outfile.open("triggered_coldbox_tps.txt", std::ios_base::app);

  // Output relevant TP information to file
  outfile << input_tp.time_start << " ";          
  outfile << input_tp.time_over_threshold << " "; // 50MHz ticks
  outfile << input_tp.time_peak << " ";           
  outfile << input_tp.channel << " ";             // Offline channel ID
  outfile << input_tp.adc_integral << " ";        
  outfile << input_tp.adc_peak << " ";            
  outfile << input_tp.detid << " ";               // Det ID - Identifies detector element
  outfile << input_tp.type << std::endl;        
  outfile.close();

  return;
}

int
TriggerActivityMakerPlaneCoincidence::check_tot(TPWindow m_current_window) const
{
  // Here, we just want to sum up all the tot values for each TP within window,
  // and return this tot of the window.
  int window_tot = 0; 
  for (auto tp : m_current_window.inputs) {
    window_tot += tp.time_over_threshold;
  }

  return window_tot;
}

// Regiser algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerPlaneCoincidence)
// END OF TA MAKER - LOW ENERGY EVENTS
