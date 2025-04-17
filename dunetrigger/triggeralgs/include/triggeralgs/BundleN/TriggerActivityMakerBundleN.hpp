/**
 * @file TriggerActivityMakerBundleN.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_BUNDLEN_TRIGGERACTIVITYMAKERBUNDLEN_HPP_
#define TRIGGERALGS_BUNDLEN_TRIGGERACTIVITYMAKERBUNDLEN_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"

#include <vector>

namespace triggeralgs {

class TriggerActivityMakerBundleN : public TriggerActivityMaker
{
  public:
    void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_tas);
    void configure(const nlohmann::json& config);
    bool bundle_condition();

  private:
      uint64_t m_bundle_size = 1;
      TriggerActivity m_current_ta;
      void set_ta_attributes();
};

} // namespace triggeralgs

#endif // TRIGGERALGS_BUNDLEN_TRIGGERACTIVITYMAKERBUNDLEN_HPP_
