/**
 * @file TriggerActivityMakerPrescale.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/Prescale/TriggerActivityMakerPrescale.hpp"

#include "TRACE/trace.h"
#define TRACE_NAME "TriggerActivityMakerPrescalePlugin"

#include <vector>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_MEDIUM;
using Logging::TLVL_IMPORTANT;

void
TriggerActivityMakerPrescale::operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta)
{
  if ((m_primitive_count++) % m_prescale == 0) {

    TLOG_DEBUG(TLVL_DEBUG_MEDIUM) << "[TAM:Pr] Emitting prescaled TriggerActivity " << (m_primitive_count - 1);
    std::vector<TriggerPrimitive> tp_list;
    tp_list.push_back(input_tp);

    TriggerActivity ta;
    ta.time_start = input_tp.time_start;
    ta.time_end = input_tp.time_start + input_tp.time_over_threshold;
    ta.time_peak = input_tp.time_peak;
    ta.time_activity = 0;
    ta.channel_start = input_tp.channel;
    ta.channel_end = input_tp.channel;
    ta.channel_peak = input_tp.channel;
    ta.adc_integral = input_tp.adc_integral;
    ta.adc_peak = input_tp.adc_peak;
    ta.detid = input_tp.detid;
    ta.type = TriggerActivity::Type::kTPC;
    ta.algorithm = TriggerActivity::Algorithm::kPrescale;

    ta.inputs = tp_list;

    output_ta.push_back(ta);
  }
}

void
TriggerActivityMakerPrescale::configure(const nlohmann::json& config)
{
  // FIXME use some schema here
  if (config.is_object() && config.contains("prescale")) {
    m_prescale = config["prescale"];
  }
  TLOG_DEBUG(TLVL_IMPORTANT) << "[TAM:Pr] Using activity prescale " << m_prescale;
}

// Register algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerPrescale)
