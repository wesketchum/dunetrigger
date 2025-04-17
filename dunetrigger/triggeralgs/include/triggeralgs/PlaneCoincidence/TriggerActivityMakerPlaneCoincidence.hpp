/**
 * @file TriggerActivityMakerPlaneCoincidence.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_PLANECOINCIDENCE_TRIGGERACTIVITYMAKERPLANECOINCIDENCE_HPP_
#define TRIGGERALGS_PLANECOINCIDENCE_TRIGGERACTIVITYMAKERPLANECOINCIDENCE_HPP_

#include "dunetrigger/channelmaps/OfflineTPCChannelMap.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TPWindow.hpp"
#include <fstream>
#include <vector>

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

namespace triggeralgs {
class TriggerActivityMakerPlaneCoincidence : public TriggerActivityMaker
{
public:
  void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta);
  void configure(const nlohmann::json& config);

private:
  TriggerActivity construct_ta(TPWindow m_current_window) const;
  uint16_t check_adjacency(TPWindow window) const; // Returns longest string of adjacent collection hits in window

  TPWindow m_current_window;             // Possibly redundant for this alg?
  uint64_t m_primitive_count = 0;
  int check_tot(TPWindow m_current_window) const;
  //void clearWindows(TriggerPrimitive const input_tp); // Function to clear or reset all windows, according to TP channel 
 
  // Make 3 instances of the Window class. One for each view plane.
  TPWindow m_collection_window; // Z
  TPWindow m_induction1_window; // U
  TPWindow m_induction2_window; // Y

  // Configurable parameters.
  std::string m_channel_map_name = "VDColdboxChannelMap";  // Default is coldbox
  bool m_trigger_on_adc = true;
  bool m_trigger_on_n_channels = true;
  bool m_trigger_on_adjacency = true;    // Default use of the triggering
  uint16_t m_adjacency_threshold = 15;   // Default is 15 wire track for testing
  int m_max_adjacency = 0;               // The maximum adjacency seen so far in any window
  uint32_t m_tot_threshold = 2000;       // Work out good values for this
  uint32_t m_adc_threshold = 300000;     // AbsRunningSum HF Alg Finds Induction ADC ~10x higher
  uint16_t m_n_channels_threshold = 20;  // Set this to ~80 for frames.bin, ~150-300 for tps_link_11.txt
  uint16_t m_adj_tolerance = 5;          // Adjacency tolerance - default is 3 from coldbox testing.
  int index = 0;
  uint16_t ta_adc = 0;
  uint16_t ta_channels = 0;
  timestamp_t m_window_length = 3000;    // Shouldn't exceed the max drift

  // Channel map object, for separating TPs by the plane view they come from
  std::shared_ptr<dunedaq::detchannelmaps::TPCChannelMap> channelMap = dunedaq::detchannelmaps::make_map(m_channel_map_name);

  // For debugging and performance study purposes.
  void add_window_to_record(TPWindow window);
  void dump_window_record();
  void dump_tp(TriggerPrimitive const& input_tp);
  std::vector<TPWindow> m_window_record;
};
} // namespace triggeralgs
#endif // TRIGGERALGS_PLANECOINCIDENCE_TRIGGERACTIVITYMAKERPLANECOINCIDENCE_HPP_
