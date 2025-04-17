/**
 * @file TriggerActivityMakerHorizontalMuon.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_HORIZONTALMUON_TRIGGERACTIVITYMAKERHORIZONTALMUON_HPP_
#define TRIGGERALGS_HORIZONTALMUON_TRIGGERACTIVITYMAKERHORIZONTALMUON_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TPWindow.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include <fstream>
#include <vector>

namespace triggeralgs {
class TriggerActivityMakerHorizontalMuon : public TriggerActivityMaker
{
public:
  void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta);
  void configure(const nlohmann::json& config);

private:
  TriggerActivity construct_ta() const;
  uint16_t check_adjacency() const; // Returns longest string of adjacent collection hits in window

  TPWindow m_current_window; // Holds collection hits only
  int check_tot() const;

  // Configurable parameters.
  bool m_trigger_on_adc = false;
  bool m_trigger_on_n_channels = false;
  bool m_trigger_on_adjacency = true; // Default use of the horizontal muon triggering
  bool m_trigger_on_tot = false;
  uint16_t m_tot_threshold = 5000;       // Time over threshold - threshold to exceed.
  bool m_print_tp_info = false;          // Prints out some information on every TP received
  uint16_t m_adjacency_threshold = 15;   // Default is 15 wire track for testing
  int m_max_adjacency = 0;               // The maximum adjacency seen so far in any window
  uint32_t m_adc_threshold = 3000000;    // Not currently triggering on this
  uint16_t m_n_channels_threshold = 400; // Set this to ~80 for frames.bin, ~150-300 for tps_link_11.txt
  uint16_t m_adj_tolerance = 3;          // Adjacency tolerance - default is 3 from coldbox testing.
  int index = 0;
  uint16_t ta_adc = 0;
  uint16_t ta_channels = 0;
  timestamp_t m_window_length = 8000; // Shouldn't exceed the max drift which is ~9375 62.5 MHz ticks for VDCB
  uint16_t ta_count = 0;              // Use for prescaling
  uint16_t m_prescale = 1;            // Prescale value, defult is one, trigger every TA

  // For debugging and performance study purposes.
  void add_window_to_record(TPWindow window);
  void dump_window_record();
  void dump_tp(TriggerPrimitive const& input_tp);
  std::vector<TPWindow> m_window_record;
};
} // namespace triggeralgs
#endif // TRIGGERALGS_HORIZONTALMUON_TRIGGERACTIVITYMAKERHORIZONTALMUON_HPP_
