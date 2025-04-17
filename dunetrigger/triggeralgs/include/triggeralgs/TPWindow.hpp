/* @file: TPWindow.hpp
 *
 * This is party of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_TPWINDOW_HPP_
#define TRIGGERALGS_TPWINDOW_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerPrimitive.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Types.hpp"

#include <ostream>
#include <unordered_map>
#include <vector>

namespace triggeralgs {

class TPWindow
{
public:
  bool is_empty() const;

  void add(TriggerPrimitive const& input_tp);

  void clear();

  uint16_t n_channels_hit();

  void move(TriggerPrimitive const& input_tp, timestamp_t const& window_length);

  void reset(TriggerPrimitive const& input_tp);

  friend std::ostream& operator<<(std::ostream& os, const TPWindow& window);

  timestamp_t time_start = 0;
  uint32_t adc_integral = 0;
  std::unordered_map<channel_t, uint16_t> channel_states;
  std::vector<TriggerPrimitive> inputs;
};
} // namespace triggeralgs

#endif // TRIGGERALGS_TPWINDOW_HPP_
