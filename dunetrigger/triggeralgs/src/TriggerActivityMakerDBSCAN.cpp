/**
 * @file TriggerActivityMakerDBSCAN.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dunetrigger/triggeralgs/include/triggeralgs/dbscan/TriggerActivityMakerDBSCAN.hpp"
#include "dbscan/Point.hpp"

#include "TRACE/trace.h"
#include "dunetrigger/triggeralgs/include/triggeralgs/Types.hpp"
#include <chrono>
#include <limits>
#define TRACE_NAME "TriggerActivityMakerDBSCANPlugin"

#include <vector>

using namespace triggeralgs;

using Logging::TLVL_DEBUG_LOW;

void
TriggerActivityMakerDBSCAN::operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta)
{
  if(input_tp.time_start < m_prev_timestamp){
    TLOG_DEBUG(TLVL_DEBUG_LOW) << "[TAM:DBS] Out-of-order TPs: prev " << m_prev_timestamp << ", current " << input_tp.time_start;
    return;
  }
  
  m_dbscan_clusters.clear();
  m_dbscan->add_primitive(input_tp, &m_dbscan_clusters);

  uint64_t t0=m_dbscan->get_first_prim_time();
  
  for(auto const& cluster : m_dbscan_clusters){
    auto& ta=output_ta.emplace_back();

    ta.time_start = std::numeric_limits<timestamp_t>::max();
    ta.time_end = 0;
    ta.channel_start = std::numeric_limits<channel_t>::max();
    ta.channel_end = 0;
    ta.adc_integral =  0;
    
    for(auto const& hit : cluster.hits){
      auto const& prim=hit->primitive;

      ta.inputs.push_back(prim);
      
      ta.time_start = std::min(prim.time_start, ta.time_start);
      ta.time_end = std::max(prim.time_start + prim.time_over_threshold, ta.time_end);

      ta.channel_start = std::min(prim.channel, ta.channel_start);
      ta.channel_end = std::max(prim.channel, ta.channel_end);

      ta.adc_integral += prim.adc_integral;

      ta.detid = prim.detid;
      if (prim.adc_peak > ta.adc_peak) {
        ta.adc_peak = prim.adc_peak;
        ta.channel_peak = prim.channel;
        ta.time_peak = prim.time_peak;
      }
    }
    ta.time_activity = ta.time_peak;

    ta.type = TriggerActivity::Type::kTPC;
    ta.algorithm = TriggerActivity::Algorithm::kDBSCAN;
  }

  m_dbscan->trim_hits();
}

void
TriggerActivityMakerDBSCAN::configure(const nlohmann::json &config)
{
  if (config.is_object())
  {
    if (config.contains("min_pts"))
      m_min_pts = config["min_pts"];
    if (config.contains("eps"))
      m_eps = config["eps"];
  }
  m_dbscan=std::make_unique<dbscan::IncrementalDBSCAN>(m_eps, m_min_pts, 10000);
}

// Register algo in TA Factory
REGISTER_TRIGGER_ACTIVITY_MAKER(TRACE_NAME, TriggerActivityMakerDBSCAN)
