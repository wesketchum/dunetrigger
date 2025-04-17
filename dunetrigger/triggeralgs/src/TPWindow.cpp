
#include "dunetrigger/triggeralgs/include/triggeralgs/TPWindow.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

namespace triggeralgs {

bool
TPWindow::is_empty() const
{
  return inputs.empty();
}

void
TPWindow::add(TriggerPrimitive const& input_tp)
{
  // Add the input TP's contribution to the total ADC, increase hit
  // channel's hit count and add it to the TP list.
  adc_integral += input_tp.adc_integral;
  channel_states[input_tp.channel]++;
  inputs.push_back(input_tp);
}

void
TPWindow::clear()
{
  inputs.clear();
  channel_states.clear();
  time_start = 0;
  adc_integral = 0;
}

uint16_t
TPWindow::n_channels_hit()
{
  return channel_states.size();
}

void
TPWindow::move(TriggerPrimitive const& input_tp, timestamp_t const& window_length)
{
  // Find all of the TPs in the window that need to be removed
  // if the input_tp is to be added and the size of the window
  // is to be conserved.
  // Substract those TPs' contribution from the total window ADC and remove their
  // contributions to the hit counts.
  uint32_t n_tps_to_erase = 0;
  for (auto tp : inputs) {
    if (!(input_tp.time_start - tp.time_start < window_length)) {
      n_tps_to_erase++;
      adc_integral -= tp.adc_integral;
      channel_states[tp.channel]--;
      // If a TP being removed from the window results in a channel no longer having
      // any hits, remove from the states map so map.size() can be used for number
      // channels hit.
      if (channel_states[tp.channel] == 0)
        channel_states.erase(tp.channel);
    } else
      break;
  }
  // Erase the TPs from the window.
  inputs.erase(inputs.begin(), inputs.begin() + n_tps_to_erase);
  // Make the window start time the start time of what is now the first TP.

  if (inputs.size() != 0) {
    time_start = inputs.front().time_start;
    add(input_tp);
  } else {
    reset(input_tp);
  }
}

void
TPWindow::reset(TriggerPrimitive const& input_tp)
{

  // Empty the channel and TP lists.
  channel_states.clear();
  inputs.clear();
  // Set the start time of the window to be the start time of theinput_tp.
  time_start = input_tp.time_start;
  // Start the total ADC integral.
  adc_integral = input_tp.adc_integral;
  // Start hit count for the hit channel.
  channel_states[input_tp.channel]++;
  // Add the input TP to the TP list.
  inputs.push_back(input_tp);
  // std::cout << "Number of channels hit: " << n_channels_hit() << std::endl;
}

std::ostream&
operator<<(std::ostream& os, const TPWindow& window)
{
  if (window.is_empty()) {
    os << "Window is empty!\n";
  } else {
    os << "Window start: " << window.time_start << ", end: " << window.inputs.back().time_start;
    os << ". Total of: " << window.adc_integral << " ADC counts with " << window.inputs.size() << " TPs.\n";
    os << window.channel_states.size() << " independent channels have hits.\n";
  }
  return os;
}

} // namespace triggeralgs
