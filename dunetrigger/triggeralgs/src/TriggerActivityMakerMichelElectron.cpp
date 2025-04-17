/**
 * @file TriggerActivityMakerMichelElectron.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/MichelElectron/TriggerActivityMakerMichelElectron.hpp"
#include "TRACE/trace.h"
#define TRACE_NAME "TriggerActivityMakerMichelElectronPlugin"
#include <vector>
#include <algorithm>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_MEDIUM;

void
TriggerActivityMakerMichelElectron::operator()(const TriggerPrimitive& input_tp,
                                               std::vector<TriggerActivity>& output_ta)
{

  // The first time operator() is called, reset the window object.
  if (m_current_window.is_empty()) {
    m_current_window.reset(input_tp);
    m_primitive_count++;
    return;
  }

  // If the difference between the current TP's start time and the start of the window
  // is less than the specified window size, add the TP to the window.
  if ((input_tp.time_start - m_current_window.time_start) < m_window_length) {
    m_current_window.add(input_tp);
  }

  // Check Michel Candidate ========================================================
  // We've filled the window, now require a sufficient length track AND that the track
  // has a potential Bragg P, and then a kink.
  else if (longest_activity().size() > m_adjacency_threshold) {
     
     
     // We have a good length acitivity, now search for Bragg peak and kinks
     std::vector<TriggerPrimitive> trackHits = longest_activity();
     
     if (check_bragg_peak(trackHits)){
       if (check_kinks(trackHits)){
         TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:ME] Emitting a trigger for candidate Michel event.";
         output_ta.push_back(construct_ta());
         m_current_window.reset(input_tp);
       } // Kinks 
     } // Bragg peak

  }

  // Otherwise, slide the window along using the current TP.
  else {
    m_current_window.move(input_tp, m_window_length);
  }

  m_primitive_count++;
  return;
}

void
TriggerActivityMakerMichelElectron::configure(const nlohmann::json& config)
{
  // FIX ME: Use some schema here. Also can't work out how to pass booleans.
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
TriggerActivityMakerMichelElectron::construct_ta() const
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
  ta.algorithm = TriggerActivity::Algorithm::kMichelElectron;
  ta.inputs = m_current_window.inputs;

  return ta;
}

std::vector<TriggerPrimitive>
TriggerActivityMakerMichelElectron::longest_activity() const
{
  // This function attempts to return a vector of hits that correspond to the longest
  // piece of activity in the current window. The logic follows that from the HMA
  // check_adjacency() function and further details can be found there.
  std::vector<TriggerPrimitive> trackHits;
  std::vector<TriggerPrimitive> finalHits; // The vector of track hits, which we return

  uint16_t adj = 1;              // Initialise adjacency, 1 for the first wire.
  uint16_t max = 0;
  unsigned int channel = 0;      // Current channel ID
  unsigned int next_channel = 0; // Next channel ID
  unsigned int next = 0;         // The next position in the hit channels vector
  unsigned int tol_count = 0;    // Tolerance count, should not pass adj_tolerance

  // Generate a channelID ordered list of hit channels for this window
  std::vector<TriggerPrimitive> hitList;
  for (auto tp : m_current_window.inputs) {
    hitList.push_back(tp);
  }
  std::sort(hitList.begin(), hitList.end(), [](TriggerPrimitive a, TriggerPrimitive b)
            { return a.channel < b.channel; });

  // ADAJACENCY LOGIC ====================================================================
  // =====================================================================================
  // Adjcancency Tolerance = Number of times prepared to skip missed hits before resetting 
  // the adjacency count. This accounts for things like dead channels / missed TPs. The 
  // maximum gap is 4 which comes from tuning on December 2021 coldbox data, and June 2022 
  // coldbox runs.
  for (int i = 0; i < hitList.size(); ++i) {

    next = (i + 1) % hitList.size(); // Loops back when outside of channel list range
    channel = hitList.at(i).channel;
    next_channel = hitList.at(next).channel; // Next channel with a hit

    if (trackHits.size() == 0 ){ trackHits.push_back(hitList.at(i)); }

    // End of vector condition.
    if (next_channel == 0) { next_channel = channel - 1; }

    // Skip same channel hits for adjacency counting, but add to the track!
    if (next_channel == channel) { 
      trackHits.push_back(hitList.at(next));
      continue; }

    // If next hit is on next channel, increment the adjacency count.
    else if (next_channel == channel + 1){ 
      trackHits.push_back(hitList.at(next));
      ++adj; }

    // If next channel is not on the next hit, but the 'second next', increase adjacency 
    // but also tally up with the tolerance counter.
    else if (((next_channel == channel + 2) || (next_channel == channel + 3) ||
              (next_channel == channel + 4) || (next_channel == channel + 5))
             && (tol_count < m_adj_tolerance)) {
      trackHits.push_back(hitList.at(next));
      ++adj;
      for (int i = 0 ; i < next_channel-channel ; ++i){ ++tol_count; }
    }

    // If next hit isn't within reach, end the adjacency count and check for a new max.
    // Reset variables for next iteration.
    else {
      if (adj > max) { 
        max = adj;
        finalHits.clear(); // Clear previous track
        for (auto h : trackHits){
          finalHits.push_back(h);
        }  
      }
      adj = 1;
      tol_count = 0;
      trackHits.clear();
    }
  }

  return finalHits;
}


// Function that tries to identify a Bragg peak via a running
// mean average of the ADC values. We use the running mean as it's less susceptible to
// spikes of activity that might trick the algorithm. We establish a baseline, then
// count up clusters of charge deposition above that baseline. If the largest is at
// one of the ends of that collection, signal a potential Bragg peak.
bool
TriggerActivityMakerMichelElectron::check_bragg_peak(std::vector<TriggerPrimitive> trackHits)
{
  bool bragg = false; 
  std::vector<float> adc_means_list;
  uint16_t convolve_value = 6;

  // Loop over hits that correspond to high adjacency activity
  for (uint16_t i = 0; i < trackHits.size(); ++i){
    float adc_sum = 0;
    float adc_mean = 0;

    // Calculate running ADC mean of this track 
    for (uint16_t j = i; j < i+convolve_value; ++j){
       int hit = (j) % trackHits.size(); 
       adc_sum += trackHits.at(hit).adc_integral;
    }

    adc_mean = adc_sum / convolve_value;
    adc_means_list.push_back(adc_mean);
    adc_sum = 0;
  } 

  // We now have a list of convolved adc means. 
  float ped = std::accumulate(adc_means_list.begin(), adc_means_list.end(), 0.0) / adc_means_list.size();
  float charge = 0;
  std::vector<float> charge_dumps;

  // Now go through the list, picking up clusters of charge above the baseline/ped
  for (auto a : adc_means_list){
    if (a > ped){
       charge += a;
    }
    else if( a < ped && charge !=0 ){
     charge_dumps.push_back(charge);
     charge = 0; 
    } 
  } 

  // If the maximum of that list of charge dumps is near(at?) either end of it
  float max_charge = *max_element(charge_dumps.begin(), charge_dumps.end()); 
  if(max_charge == charge_dumps.front() || max_charge == charge_dumps.back()){ bragg=true; }

  return bragg;
 }

bool
TriggerActivityMakerMichelElectron::check_kinks(std::vector<TriggerPrimitive> finalHits)
{
    bool kinks = false;  // We actually required two kinks in the coldbox, the michel kink and the wes kink
    std::vector<float> runningGradient;
    std::vector<float> runningMeanGradient;

    // Choice to be made here. Do we want to scane in collection (z) or time (x) direction when calculating gradient between hits. I
    // would say if we have already made the request to pass a track of length specific threshold which is longer than the drift
    // direction for the coldbox, it makes sense to scan across channels a little more.
    std::sort(finalHits.begin(), finalHits.end(), [](TriggerPrimitive a, TriggerPrimitive b) { return a.channel < b.channel; });

    // Populate the runningGradient with the track hits. Do this between ith and i+kth TPs, to small scale fluctuations of the track
    // Yet k should be kept small, so that enough gradient information is preserved at the end of the track to identify kinks
    for (int i=0 ; i < finalHits.size()-2; i++){
   
      // Skip same channel hits or if the start times are the same - no div by zero! 
      if (finalHits.at(i+2).channel == finalHits.at(i).channel || (finalHits.at(i+2).time_start == finalHits.at(i).time_start) ) { continue; }

      // Check that the next TP is closeby; enough in space and time directions so as to avoid obtaining a gradient value from
      // same channel hits at large time difference or vice versa due to kink topology or showers. Clearly we shouldn't be very far in 
      // channel number, but since we might later try to do this check in the time direction, leave the condition in.
      int diff = finalHits.at(i+2).time_start - finalHits.at(i).time_start;
      if((std::abs(diff) > 1000) || ((std::abs(finalHits.at(i+2).channel - finalHits.at(i).channel) > 6))) { continue; } 

      // Gradient is just change in z (collection) over change in x (drift). x is admitedly roughly converted from
      // hit start time, but I don't think diffusion effects are a huge concern over 20cm. Using mm for readability/visualisation 
      float dz = (finalHits.at(i+2).channel - finalHits.at(i).channel)*4.67; // Change in collection wire z to separation in mm
      long long int dt = finalHits.at(i+2).time_start - finalHits.at(i).time_start;
      float dx = dt*0.028; // Change time to separation in x mm
      float g = dz/dx;

      runningGradient.push_back(g); 
    }
 
    // Require a decent length of the gradients vector. Otherwise some adjacent events are showers and the conditions above mean
    // we don't get enough entries. In essence, this provides some confidence that it's track-like rather than shower-like. Which
    // is what we want for a Michel event.
    if ( runningGradient.size() > 10 ){
      
      // Now lets take a running mean of the gradients between TPs, less susceptible to wild changes due to deltas/etc
      for(int g=0 ; g < runningGradient.size()-1 ; g++){
        float gsum = runningGradient.at(g) + runningGradient.at(g+1);
        runningMeanGradient.push_back(gsum/2);
      } 
   
      // We have a list of gradients, now just demand that the two ends have gradients that differ significantly
      // from the mean gradient of the activity at both ends. This aims to pick out wesKinks and michelKinks
      if(runningMeanGradient.size() > 10 ){

        float mean = (std::abs(std::accumulate(runningMeanGradient.begin(), runningMeanGradient.end(), 0.0)))/(runningMeanGradient.size());

        // If you're testing on simulation or december data, you won't see the wes kink, so use an || instead of &&
        if((std::abs(runningMeanGradient.front()) + mean > 2.5*mean)  ||  ((std::abs(runningMeanGradient.back() + mean)) > 2.5*mean)){ kinks=true; }   
      } 
    } 

  return kinks;
}

// ===============================================================================================
// ===============================================================================================
// Functions below this line are for debugging purposes.
// ===============================================================================================

void
TriggerActivityMakerMichelElectron::add_window_to_record(Window window)
{
  m_window_record.push_back(window);
  return;
}


// Function to dump the details of the TA window currently on record
void
TriggerActivityMakerMichelElectron::dump_window_record()
{
  // FIX ME: Need to index this outfile in the name by detid or something similar.
  std::ofstream outfile;
  outfile.open("window_record_tam.csv", std::ios_base::app);

  for (auto window : m_window_record) {
    outfile << window.time_start << ",";
    outfile << window.inputs.back().time_start << ",";
    outfile << window.inputs.back().time_start - window.time_start << ","; // window_length - from TP start times
    outfile << window.adc_integral << ",";
    outfile << window.n_channels_hit() << ",";       // Number of unique channels with hits
    outfile << window.inputs.size() << ",";          // Number of TPs in Window
    outfile << window.inputs.back().channel << ",";  // Last TP Channel ID
    outfile << window.inputs.front().channel << ","; // First TP Channel ID
    outfile << longest_activity().size() << std::endl;             // New adjacency value for the window
  }

  outfile.close();

  m_window_record.clear();

  return;
}

// Function to add current TP details to a text file for testing and debugging.
void
TriggerActivityMakerMichelElectron::dump_tp(TriggerPrimitive const& input_tp)
{
  std::ofstream outfile;
  outfile.open("coldbox_tps.txt", std::ios_base::app);

  // Output relevant TP information to file
  outfile << input_tp.time_start << " ";          // Start time of TP
  outfile << input_tp.time_over_threshold << " "; // in multiples of 25
  outfile << input_tp.time_peak << " ";           //
  outfile << input_tp.channel << " ";             // Offline channel ID
  outfile << input_tp.adc_integral << " ";        // ADC Sum
  outfile << input_tp.adc_peak << " ";            // ADC Peak Value
  outfile << input_tp.detid << " ";               // Det ID - Identifies detector element, APA or PDS part etc...
  outfile << input_tp.type << std::endl;          // This should now write out TPs in the same 'coldBox' way.
  outfile.close();

  return;
}

/*
void
TriggerActivityMakerMichelElectron::flush(timestamp_t, std::vector<TriggerActivity>& output_ta)
{
  // Check the status of the current window, construct TA if conditions are met. Regardless
  // of whether the conditions are met, reset the window.
  if(m_current_window.adc_integral > m_adc_threshold && m_trigger_on_adc){
  //else if(m_current_window.adc_integral > m_conf.adc_threshold && m_conf.trigger_on_adc){
    //TLOG_DEBUG(TRACE_NAME) << "ADC integral in window is greater than specified threshold.";
    output_ta.push_back(construct_ta());
  }
  else if(m_current_window.n_channels_hit() > m_n_channels_threshold && m_trigger_on_n_channels){
  //else if(m_current_window.n_channels_hit() > m_conf.n_channels_threshold && m_conf.trigger_on_n_channels){
    //TLOG_DEBUG(TRACE_NAME) << "Number of channels hit in the window is greater than specified threshold.";
    output_ta.push_back(construct_ta());
  }

  //TLOG_DEBUG(TRACE_NAME) << "Clearing the current window, on the arrival of the next input_tp, the window will be
reset."; m_current_window.clear();

  return;
}*/

// Register algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerMichelElectron)
