/**
 * @file TriggerActivity.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERACTIVITY_HPP_
#define TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERACTIVITY_HPP_

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerPrimitive.hpp"

#include <vector>

namespace triggeralgs {

struct TriggerActivity : public dunedaq::trgdataformats::TriggerActivityData
{
  std::vector<TriggerPrimitive> inputs;
};

} // namespace triggeralgs

#endif // TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERACTIVITY_HPP_
