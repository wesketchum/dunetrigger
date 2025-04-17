#include "dunetrigger/triggeralgs/include/triggeralgs/TAWindow.hpp"

#include <vector>

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

namespace triggeralgs {

//---
void
TAWindow::add(const TriggerActivity& input_ta)
{

  adc_integral += input_ta.adc_integral;
  for (TriggerPrimitive tp : input_ta.inputs) {
    channel_states[tp.channel]++;
  }
  // Perform binary search based on time_start.
  uint16_t insert_at = 0;
  for (auto ta : inputs) {
    if (input_ta.time_start < ta.time_start)
      break;
    insert_at++;
  }
  inputs.insert(inputs.begin() + insert_at, input_ta);
}

//---
void
TAWindow::clear()
{
  inputs.clear();
  channel_states.clear();
  time_start = 0;
  adc_integral = 0;
};

//---
void
TAWindow::move(TriggerActivity const& input_ta, timestamp_t const& window_length)
{
  uint32_t n_tas_to_erase = 0;
  for (auto ta : inputs) {
    if (!(input_ta.time_start - ta.time_start < window_length)) {
      n_tas_to_erase++;
      adc_integral -= ta.adc_integral;
      for (TriggerPrimitive tp : ta.inputs) {
        channel_states[tp.channel]--;
        // If a TA being removed from the window results in a channel no longer having
        // any hits, remove from the states map so map.size() can be used for number
        // channel hits.
        if (channel_states[tp.channel] == 0)
          channel_states.erase(tp.channel);
      }
    } else
      break;
  }
  // Erase the TAs from the window.
  inputs.erase(inputs.begin(), inputs.begin() + n_tas_to_erase);
  // Make the window start time the start time of what is now the
  // first TA.
  if (inputs.size() != 0) {
    time_start = inputs.front().time_start;
    add(input_ta);
  } else {
    reset(input_ta);
  }
  // add(input_ta);
  // time_start = inputs.front().time_start;
}

//---
void
TAWindow::reset(TriggerActivity const& input_ta)
{
  // Empty the channel and TA lists.
  channel_states.clear();
  inputs.clear();
  // Set the start time of the window to be the start time of the
  // input_ta.
  time_start = input_ta.time_start;
  // Start the total ADC integral.
  adc_integral = input_ta.adc_integral;
  // Start hit count for the hit channels.
  for (TriggerPrimitive tp : input_ta.inputs) {
    channel_states[tp.channel]++;
  }
  // Add the input TA to the TA list.
  inputs.push_back(input_ta);
}

std::ostream&
operator<<(std::ostream& os, const TAWindow& window)
{
  if (window.is_empty())
    os << "Window is empty!\n";
  else {
    os << "Window start: " << window.time_start << ", end: " << window.inputs.back().time_start;
    os << ". Total of: " << window.adc_integral << " ADC counts with " << window.inputs.size() << " TPs.\n";
    os << window.channel_states.size() << " independent channels have hits.\n";
  }
  return os;
};

//---
//---
//---
//---
//---
//---

} // namespace triggeralgs