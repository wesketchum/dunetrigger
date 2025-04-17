/**
 * @file TriggerPrimitive.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERPRIMITIVE_HPP_
#define TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERPRIMITIVE_HPP_

#include "detdataformats/trigger/TriggerPrimitive.hpp"

namespace triggeralgs {
// TriggerPrimitive looks a bit difference to TriggerActivity and
// TriggerCandidate: the latter two have distinct overlay and
// non-overlay versions, but TriggerPrimitive does not. So whereas TA
// and TC have (non-overlay) definitions in triggeralgs, TP has a
// definition only in detdataformats. So we just copy it here so that
// references to TP, TA and TC in triggeralgs and downstream code
// "look" the same
using TriggerPrimitive = dunedaq::trgdataformats::TriggerPrimitive;

} // namespace triggeralgs

#endif // TRIGGERALGS_INCLUDE_TRIGGERALGS_TRIGGERPRIMITIVE_HPP_
