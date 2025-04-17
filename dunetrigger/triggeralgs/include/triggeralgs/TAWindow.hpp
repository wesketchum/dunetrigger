/* @file: TAWindow.hpp
 *
 * This is party of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_TAWINDOW_HPP_
#define TRIGGERALGS_TAWINDOW_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivity.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Types.hpp"

#include <ostream>
#include <unordered_map>

namespace triggeralgs {

class TAWindow
{
public:
  bool is_empty() const { return inputs.empty(); };

  /// @brief
  /// Add the input TA's contribution to the total ADC, increase the hit count of 
  /// all of the channels which feature and add it to the TA list keeping the TA
  /// list time ordered by time_start. Preserving time order makes moving easier.
  /// @param input_ta
  void add(const TriggerActivity& input_ta);

  /// @brief Clear all inputs
  void clear();

  uint16_t n_channels_hit() { return channel_states.size(); };

  /// @brief 
  /// Find all of the TAs in the window that need to be removed
  /// if the input_ta is to be added and the size of the window
  /// is to be conserved.
  /// Subtract those TAs' contribution from the total window ADC and remove their
  /// contributions to the hit counts.
  /// @param input_ta 
  /// @param window_length 
  void move(TriggerActivity const& input_ta, timestamp_t const& window_length);

  /// @brief Reset window content on the input
  /// @param input_ta 
  void reset(TriggerActivity const& input_ta);

  friend std::ostream& operator<<(std::ostream& os, const TAWindow& window);

  timestamp_t time_start = 0;
  uint64_t adc_integral = 0;
  std::unordered_map<channel_t, uint16_t> channel_states;
  std::vector<TriggerActivity> inputs;
};

} // namespace triggeralgs

#endif // TRIGGERALGS_TAWINDOW_HPP_
