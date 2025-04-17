/**
 * @file TriggerDecisionMaker.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERPRIMITIVEMAKER_HPP_
#define TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERPRIMITIVEMAKER_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/Issues.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/Logging.hpp"
#include "detdataformats/trigger/TriggerPrimitive.hpp"

#include <nlohmann/json.hpp>
#include <vector>

namespace triggeralgs {

class TriggerPrimitiveMaker
{
public:
  virtual ~TriggerPrimitiveMaker() = default;
  virtual void operator()(const void* input_rawdata, std::vector<dunedaq::trgdataformats::TriggerPrimitive>& output_tp) = 0;
  virtual void flush(std::vector<dunedaq::trgdataformats::TriggerPrimitive>&) {}
  virtual void configure(const nlohmann::json&) {}
};

} // namespace triggeralgs

#endif // TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERPRIMITIVEMAKER_HPP_
