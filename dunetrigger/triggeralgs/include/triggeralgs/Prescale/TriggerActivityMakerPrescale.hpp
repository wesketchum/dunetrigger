/**
 * @file TriggerActivityMakerPrescale.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_PRESCALE_TRIGGERACTIVITYMAKERPRESCALE_HPP_
#define TRIGGERALGS_PRESCALE_TRIGGERACTIVITYMAKERPRESCALE_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"

#include <vector>

namespace triggeralgs {
class TriggerActivityMakerPrescale : public TriggerActivityMaker
{

public:
  void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta);
  
  void configure(const nlohmann::json &config);
  
private:  
  uint64_t m_primitive_count = 0;   // NOLINT(build/unsigned)
  uint64_t m_prescale = 1;          // NOLINT(build/unsigned)
};
} // namespace triggeralgs

#endif // TRIGGERALGS_PRESCALE_TRIGGERACTIVITYMAKERPRESCALE_HPP_
