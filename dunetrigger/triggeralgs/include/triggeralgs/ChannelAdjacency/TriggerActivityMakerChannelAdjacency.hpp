/**
 * @file TriggerActivityMakerChannelAdjacency.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_CHANNELADJACENCY_TRIGGERACTIVITYMAKERCHANNELADJACENCY_HPP_
#define TRIGGERALGS_CHANNELADJACENCY_TRIGGERACTIVITYMAKERCHANNELADJACENCY_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TPWindow.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include <fstream>
#include <vector>

namespace triggeralgs {
class TriggerActivityMakerChannelAdjacency : public TriggerActivityMaker
{
public:
  void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta);
  void configure(const nlohmann::json& config);

private:
  TriggerActivity construct_ta(TPWindow) const;

  TPWindow check_adjacency();

  TPWindow m_current_window;

  // Configurable parameters.
  bool m_print_tp_info = false;        // Prints out some information on every TP received
  uint16_t m_adjacency_threshold = 15; // Default is 15 wire track for testing
  uint16_t m_adj_tolerance = 3;        // Adjacency tolerance - default is 3 from coldbox testing.
  timestamp_t m_window_length = 8000;  // Shouldn't exceed the max drift which is ~9375 62.5 MHz ticks for VDCB
  uint16_t m_ta_count = 0;             // Use for prescaling
  uint16_t m_prescale = 1;             // Prescale value, defult is one, trigger every TA

  // For debugging and performance study purposes.
  void add_window_to_record(TPWindow window);
  std::vector<TPWindow> m_window_record;
};
} // namespace triggeralgs
#endif // TRIGGERALGS_CHANNELADJACENCY_TRIGGERACTIVITYMAKERCHANNELADJACENCY_HPP_
