/**
 * @file TriggerActivityMakerChannelDistance.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_CHANNELDISTANCE_TRIGGERACTIVITYMAKERCHANNELDISTANCE_HPP_
#define TRIGGERALGS_CHANNELDISTANCE_TRIGGERACTIVITYMAKERCHANNELDISTANCE_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include <algorithm>

namespace triggeralgs {

class TriggerActivityMakerChannelDistance : public TriggerActivityMaker {
  public:
    void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_tas);
    void configure(const nlohmann::json& config);
    void set_ta_attributes();

  private:
    void set_new_ta(const TriggerPrimitive& input_tp);
    TriggerActivity m_current_ta;
    uint32_t m_max_channel_distance = 50;
    uint64_t m_window_length = 8000;
    uint16_t m_min_tps = 20; // AEO: Type is arbitrary. Surprised even asking for 2^8 TPs.
    uint32_t m_current_lower_bound;
    uint32_t m_current_upper_bound;
};

} // namespace triggeralgs

#endif // TRIGGERALGS_CHANNELDISTANCE_TRIGGERACTIVITYMAKERCHANNELDISTANCE_HPP_
