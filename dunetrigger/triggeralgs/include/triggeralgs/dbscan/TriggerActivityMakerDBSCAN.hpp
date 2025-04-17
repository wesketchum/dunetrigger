/**
 * @file TriggerActivityMakerDBSCAN.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_DBSCAN_TRIGGERACTIVITYMAKERDBSCAN_HPP_
#define TRIGGERALGS_DBSCAN_TRIGGERACTIVITYMAKERDBSCAN_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/dbscan/dbscan.hpp"

#include <memory>
#include <vector>

namespace triggeralgs {
class TriggerActivityMakerDBSCAN : public TriggerActivityMaker
{

public:
  void operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity>& output_ta);
  
  void configure(const nlohmann::json &config);
  
private:  
  int m_eps{10};
  int m_min_pts{3}; // Minimum number of points to form a cluster
  timestamp_t m_first_timestamp{0};
  timestamp_t m_prev_timestamp{0};
  std::vector<dbscan::Cluster> m_dbscan_clusters;
  std::unique_ptr<dbscan::IncrementalDBSCAN> m_dbscan;
};
} // namespace triggeralgs

#endif // TRIGGERALGS_PRESCALE_TRIGGERACTIVITYMAKERPRESCALE_HPP_
